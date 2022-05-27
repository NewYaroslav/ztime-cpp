#ifndef ZTIME_NTP_HPP_INCLUDED
#define ZTIME_NTP_HPP_INCLUDED

#include <winsock2.h>
#include <windows.h>

#include <deque>
#include <string>
#include <cstring>
#include <ctime>

#include <random>
#include <algorithm>
#include <mutex>
#include <atomic>
#include <future>

namespace ztime {

	/** \brief Клиент NTP
	 * Данный класс позволяет замерять смещение локального времени компьютера
	 */
	class NtpClient {
	private:

		SOCKET sockfd = INVALID_SOCKET;
		struct sockaddr_in serv_addr;
		struct hostent *server = nullptr;

		//int64_t offset_us = 0;			  /**< Смещение метки времени в микросекундах */
		//std::mutex offset_us_mutex;

		//uint64_t next_timestamp = 0;		  /**< Следующая метка времени для опроса */
		//std::mutex next_timestamp_mutex;

		std::atomic<int64_t>	offset_us = ATOMIC_VAR_INIT(0);
		std::atomic<uint64_t>	next_timestamp = ATOMIC_VAR_INIT(0);
		std::atomic<bool>		is_init = ATOMIC_VAR_INIT(false);

		std::mutex make_measurement_mutex;

		/** \brief Конфигурация для работы с NTP сервером
		 */
		class Config {
		public:
			std::string host_name;								/**< Имя хоста */
			int port = 0;										/**< Порт */
			const int64_t ntp_timestamp_delta = 2208988800ll;	/**< Смещение времени */

			const struct timeval timeout = {
				.tv_sec = 5,
				.tv_usec = 0
			};
		} config;	/**< Конфигурация для работы с NTP сервером */

		/** \brief Структура пакета NTP
		 */
		typedef struct {
			uint8_t li_vn_mode;			// Eight bits. li, vn, and mode.
										// li.	 Two bits.	 Leap indicator.
										// vn.	 Three bits. Version number of the protocol.
										// mode. Three bits. Client will pick mode 3 for client.
			uint8_t stratum;			// Eight bits. Stratum level of the local clock.
			uint8_t poll;				// Eight bits. Maximum interval between successive messages.
			uint8_t precision;			// Eight bits. Precision of the local clock.

			uint32_t rootDelay;			// 32 bits. Total round trip delay time.
			uint32_t rootDispersion;	// 32 bits. Max error aloud from primary clock source.
			uint32_t refId;				// 32 bits. Reference clock identifier.

			uint32_t refTm_s;			// 32 bits. Reference time-stamp seconds.
			uint32_t refTm_f;			// 32 bits. Reference time-stamp fraction of a second.

			uint32_t origTm_s;			// 32 bits. Originate time-stamp seconds.
			uint32_t origTm_f;			// 32 bits. Originate time-stamp fraction of a second.

			uint32_t rxTm_s;			// 32 bits. Received time-stamp seconds.
			uint32_t rxTm_f;			// 32 bits. Received time-stamp fraction of a second.

			uint32_t txTm_s;			// 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
			uint32_t txTm_f;			// 32 bits. Transmit time-stamp fraction of a second.
		} ntp_packet;			// Total: 384 bits or 48 bytes.

		/** \brief Оболочка для работы с WSAStartup и WSACleanup
		 */
		class WsaShell {
		private:
			bool is_error = false;
			int err_code = 0;
		public:
			WSADATA wsa_data;

			WsaShell() {
				err_code = WSAStartup(MAKEWORD(2,2), &wsa_data);
				if (err_code != NO_ERROR) is_error = true;
			};

			~WsaShell() {
				if (!is_error) {
					WSACleanup();
				}
			}

			const inline bool check_error() noexcept {
				return is_error;
			}

			const inline int get_error() noexcept {
				return err_code;
			}
		};

		/** \brief Подготовить пакет данных
		 */
		inline void pack(ntp_packet &packet) noexcept {
			const uint8_t leap_indicator	= 0; // индикатор коррекции, 0 - нет коррекции
			const uint8_t version_number	= 3; // номер версии протокола NTP (1-4)
			const uint8_t mode				= 3; // Режим клиента

			std::memset(&packet, 0, sizeof(ntp_packet));
			packet.li_vn_mode = (leap_indicator << 6) + (version_number << 3) + mode;

			const struct timespec ts = get_timespec();

			// В Transmit необходимо указать текущее время на локальной машине (количество секунд с 1 января 1900 г)
			packet.txTm_s = ts.tv_sec + config.ntp_timestamp_delta;
			packet.txTm_f = (uint32_t)(((uint64_t)ts.tv_nsec * (uint64_t)0xFFFFFFFF) / 1000000000ull);

			packet.txTm_s = htonl(packet.txTm_s);
			packet.txTm_f = htonl(packet.txTm_f);
		}

		/** \brief Распаковать пакет данных
		 */
		inline void unpack(ntp_packet &packet) noexcept {
			const struct timespec ts = get_timespec();

			packet.txTm_s = ntohl(packet.txTm_s);
			packet.txTm_f = ntohl(packet.txTm_f);
			packet.rxTm_s = ntohl(packet.rxTm_s);
			packet.rxTm_f = ntohl(packet.rxTm_f);
			packet.origTm_s = ntohl(packet.origTm_s);
			packet.origTm_f = ntohl(packet.origTm_f);

			const uint64_t delay_s = std::pow(2, packet.poll);
			next_timestamp = ts.tv_sec + delay_s;

			const uint64_t time_arrive_us = (uint64_t)ts.tv_sec * 1000000ull + (uint64_t)ts.tv_nsec / 1000ull;
			const uint64_t time_originate_us = (((uint64_t)packet.origTm_s - config.ntp_timestamp_delta) * 1000000ull) + ((uint64_t)packet.origTm_f * 1000000ull / (uint64_t)0xFFFFFFFF);
			const uint64_t time_receive_us = (((uint64_t)packet.rxTm_s - config.ntp_timestamp_delta) * 1000000ull) + ((uint64_t)packet.rxTm_f * 1000000ull / (uint64_t)0xFFFFFFFF);
			const uint64_t time_transmit_us = (((uint64_t)packet.txTm_s - config.ntp_timestamp_delta) * 1000000ull) + ((uint64_t)packet.txTm_f * 1000000ull / (uint64_t)0xFFFFFFFF);
			const int64_t time_diff_us = (int64_t)time_receive_us - (int64_t)time_originate_us - (int64_t)time_arrive_us + (int64_t)time_transmit_us;

			offset_us = time_diff_us;
			if (on_update != nullptr) on_update(time_diff_us);
		}

		bool request() {
			// инициализируем
			sockfd = INVALID_SOCKET;
			int res = 0;

			WsaShell wsa_init;
			if (wsa_init.check_error()) {
				if (on_error != nullptr) {
					on_error (wsa_init.get_error(), "WSAStartup failed: " + std::to_string(wsa_init.get_error()));
				}
				return false;
			}

			sockfd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP ); // Create a UDP socket.
			if ( sockfd == INVALID_SOCKET ) {
				if (on_error != nullptr) {
					on_error (res, "Error at socket: " + std::to_string(WSAGetLastError()));
				}
				return false;
			}

			server = gethostbyname(config.host_name.c_str()); // Convert URL to IP.
			if (server == nullptr) {
				if (on_error != nullptr) {
					on_error (0, "ERROR, no such host");
				}
				closesocket(sockfd);
				return false;
			}

			std::memset((char*)&serv_addr, 0, sizeof(serv_addr));
			serv_addr.sin_family = AF_INET;

			// Copy the server's IP address to the server address structure.
			std::memcpy((char*)&serv_addr.sin_addr.s_addr, (char*)server->h_addr, server->h_length);

			// Convert the port number integer to network big-endian style and save it to the server address structure.
			serv_addr.sin_port = htons(config.port);

			// подключаемся по сокету
			if (sockfd == INVALID_SOCKET) return false;
			if (::connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
				closesocket(sockfd);
				if (on_error != nullptr) {
					on_error (WSAGetLastError(), "Unable to connect to server: " + std::to_string(WSAGetLastError()));
				}
				return false;
			}

			#if defined (_WIN32) || defined (__CYGWIN__) || defined (__MINGW32__) || defined (__MINGW64__) || defined (__GNUC__)
			DWORD dw = (config.timeout.tv_sec * 1000) + ((config.timeout.tv_usec + 999) / 1000);
			setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *) &dw, sizeof(dw));
			#else
			setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *) &config.timeout, sizeof(struct timeval));
			#endif

			// подготавлвиаем пакет и делаем запрос
			ntp_packet packet;// = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			pack(packet);

			// Send it the NTP packet it wants. If n == -1, it failed.
			res = send(sockfd, (char*)&packet, sizeof(ntp_packet), 0);
			if (res == SOCKET_ERROR) {
				if (on_error != nullptr) {
					on_error (0, "ERROR writing to socket");
				}
				return false;
			}

			// Wait and receive the packet back from the server. If n == -1, it failed.
			res = recv(sockfd, (char*)&packet, sizeof(ntp_packet), 0);
			if (res < 0) {
				if (on_error != nullptr) {
					on_error (0, "ERROR reading from socket");
				}
				return false;
			}

			unpack(packet);

			res = closesocket(sockfd);
			if (res == SOCKET_ERROR) {
				if (on_error != nullptr) {
					on_error (WSAGetLastError(), "closesocket failed with error");
				}
				return false;
			}
			return true;
		}

		bool check_poll() noexcept {
			if (next_timestamp == 0) return true;
			const struct timespec ts = get_timespec();
			if ((uint64_t)ts.tv_sec > next_timestamp) return true;
			return false;
		}

		inline const struct timespec get_timespec() noexcept {
			struct timespec ts;
#			if defined(CLOCK_REALTIME)
			clock_gettime(CLOCK_REALTIME, &ts); // Версия для POSIX
#			else
			timespec_get(&ts, TIME_UTC);
#			endif
			return ts;
		}

	public:

		std::function<void(const int64_t code, const std::string &message)> on_error = nullptr; /**< Callback-функция для события ошибок */
		std::function<void(const int64_t offset)>							on_update = nullptr;/**< Callback-функция для события измерения смещения времени */

		NtpClient(const std::string ntp_host = "pool.ntp.org", const int ntp_port = 123) {
			set_host(ntp_host, ntp_port);
		};

		/** \brief Установить хост NTP сервера
		 * \param ntp_host	Хост NTP сервера
		 * \param ntp_port	Порт NTP сервера
		 */
		inline void set_host(
				const std::string ntp_host = "pool.ntp.org",
				const int ntp_port = 123) noexcept {
			std::lock_guard<std::mutex> lock(make_measurement_mutex);
			config.host_name = ntp_host;
			config.port = ntp_port;
		}

		/** \brief Сделать замер смещения времени
		 * \return Вернет true в случае успешного замера
		 */
		inline bool make_measurement() noexcept {
			std::lock_guard<std::mutex> lock(make_measurement_mutex);
			if (!check_poll()) return false;
			return request();
		}

		/** \brief Проверить занятость подключения к NTP
		 * \return Если с момента последнего запроса прошло недостаточное количество времени, вернет true
		 */
		inline bool busy() noexcept {
			return !check_poll();
		}

		/** \brief Проверить инициализацию смещения времени
		 * \return Вернет true в случае инициализации смещения времени
		 */
		inline bool init() noexcept {
			return is_init;
		}

		/** \brief Получить смещение времени в микросекундах
		 * \return Время смещения в микросекундах
		 */
		inline const int64_t get_offset_us() noexcept {
			return offset_us;
		}

		/** \brief Получить метку времени в микросекундах
		 * \return Метка времени в микросекундах
		 */
		inline const uint64_t get_timestamp_us() noexcept {
			const struct timespec ts = get_timespec();
			return (uint64_t)(1000000ll * (int64_t)ts.tv_sec + ((int64_t)ts.tv_nsec / 1000ll) + get_offset_us());
		}

		/** \brief Получить метку времени в миллисекундах
		 * \return Метка времени в миллисекундах
		 */
		inline const uint64_t get_timestamp_ms() noexcept {
			const struct timespec ts = get_timespec();
			return (uint64_t)(1000ll * (int64_t)ts.tv_sec + (((int64_t)ts.tv_nsec / 1000ll) + get_offset_us()) / 1000ll);
		}

		inline const uint64_t get_timestamp() noexcept {
			const struct timespec ts = get_timespec();
			return (uint64_t)((int64_t)ts.tv_sec + (((int64_t)ts.tv_nsec / 1000ll) + get_offset_us()) / 1000000ll);
		}

		inline const double get_ftimestamp() noexcept {
			return (double)get_timestamp_us() / 1000000.0d;
		}
	}; // NtpClient

	/** \brief Класс пула клиентов NTP
	 */
	class NtpClientPool {
	private:
		std::deque<NtpClient>	clients;
		std::deque<int64_t>		time_measurements;
		std::atomic<uint64_t>	next_timestamp = ATOMIC_VAR_INIT(0);
		std::atomic<uint64_t>	last_timestamp_us = ATOMIC_VAR_INIT(0);
		std::atomic<uint64_t>	last_timestamp_ms = ATOMIC_VAR_INIT(0);
		std::atomic<uint64_t>	last_timestamp = ATOMIC_VAR_INIT(0);
		std::atomic<int64_t>	offset_us = ATOMIC_VAR_INIT(0);
		std::atomic<bool>		is_init = ATOMIC_VAR_INIT(false);

		class Config {
		public:
			std::deque<std::string> hosts;	/**< Список хостов */
			int delay_measurements	= 1024;	/**< Период опроса пула */
			int stop_measurements	= 3;	/**< Количество запросов для остановки */
			int max_measurements	= 100;	/**< Максимальное количество измерений для медианного фильтра */
			bool is_hosts_shuffle		= true;
		} config;

		std::mutex config_mutex;
		std::mutex make_measurement_mutex;

		inline const struct timespec get_timespec() noexcept {
			struct timespec ts;
#			if defined(CLOCK_REALTIME)
			clock_gettime(CLOCK_REALTIME, &ts); // Версия для POSIX
#			else
			timespec_get(&ts, TIME_UTC);
#			endif
			return ts;
		}

	public:

		NtpClientPool() {
			set_default_hosts();
		};

		inline void set_delay_measurements(const int value) noexcept {
			std::lock_guard<std::mutex> lock(config_mutex);
			config.delay_measurements = value;
		}

		inline void set_stop_measurements(const int value) noexcept {
			std::lock_guard<std::mutex> lock(config_mutex);
			config.stop_measurements = value;
		}

		inline void set_max_measurements(const int value) noexcept {
			std::lock_guard<std::mutex> lock(config_mutex);
			config.max_measurements = value;
		}

		inline void set_hosts_shuffle(const bool value) noexcept {
			std::lock_guard<std::mutex> lock(config_mutex);
			config.is_hosts_shuffle = value;
		}

		inline void set_hosts(const std::deque<std::string> &hosts) noexcept {
			std::lock_guard<std::mutex> lock(config_mutex);
			config.hosts = hosts;
		}

		inline void add_host(const std::string &host) noexcept {
			std::lock_guard<std::mutex> lock(config_mutex);
			config.hosts.push_back(host);
		}

		inline void set_default_hosts() noexcept {
			std::lock_guard<std::mutex> lock(config_mutex);
			// https://gist.github.com/mutin-sa/eea1c396b1e610a2da1e5550d94b0453#file-public_time_servers-md
			config.hosts = {
				// Google Public NTP
				"time.google.com",
				"time1.google.com",
				"time2.google.com",
				"time3.google.com",
				"time4.google.com",
				// Cloudflare NTP
				"time.cloudflare.com",
				// Facebook NTP
				"time.facebook.com",
				"time1.facebook.com",
				"time2.facebook.com",
				"time3.facebook.com",
				"time4.facebook.com",
				"time5.facebook.com",
				// Microsoft NTP server
				"time.windows.com",
				// Apple NTP server
				"time.apple.com",
				"time1.apple.com",
				"time2.apple.com",
				"time3.apple.com",
				"time4.apple.com",
				"time5.apple.com",
				"time6.apple.com",
				"time7.apple.com",
				"time.euro.apple.com",
				// NIST Internet Time Service
				"time-a-g.nist.gov",
				"time-b-g.nist.gov",
				"time-c-g.nist.gov",
				"time-d-g.nist.gov",
				"time-a-wwv.nist.gov",
				"time-b-wwv.nist.gov",
				"time-c-wwv.nist.gov",
				"time-d-wwv.nist.gov",
				"time-a-b.nist.gov",
				"time-b-b.nist.gov",
				"time-c-b.nist.gov",
				"time-d-b.nist.gov",
				"time.nist.gov",
				"utcnist.colorado.edu",
				"utcnist2.colorado.edu",
				// VNIIFTRI
				// Stratum 1:
				"ntp1.vniiftri.ru",
				"ntp2.vniiftri.ru",
				"ntp3.vniiftri.ru",
				"ntp4.vniiftri.ru",
				"ntp1.niiftri.irkutsk.ru",
				"ntp2.niiftri.irkutsk.ru",
				"vniiftri.khv.ru",
				"vniiftri2.khv.ru",
				// Stratum 2:
				"ntp21.vniiftri.ru",
				// Mobatime
				// Stratum 1
				"ntp.mobatime.ru",
				// NTP SERVERS
				// Stratum 1:
				"ntp1.stratum1.ru",
				"ntp2.stratum1.ru",
				"ntp3.stratum1.ru",
				"ntp4.stratum1.ru",
				"ntp5.stratum1.ru",
				// Stratum 2:
				"ntp2.stratum2.ru",
				"ntp3.stratum2.ru",
				"ntp4.stratum2.ru",
				"ntp5.stratum2.ru",
				// Stratum1
				"stratum1.net",
				// time.in.ua
				// Stratum 1:
				"ntp.time.in.ua",
				"ntp2.time.in.ua",
				// Stratum 2:
				"ntp3.time.in.ua",
				// Company Delfa Co. Ltd
				"ntp.ru",
				// ACO.net
				"ts1.aco.net",
				"ts2.aco.net",
				// Berkeley
				// Stratum 1:
				"ntp1.net.berkeley.edu",
				"ntp2.net.berkeley.edu",
				// Georgia State University
				"ntp.gsu.edu",
				// University of Saskatchewan
				"tick.usask.ca",
				"tock.usask.ca",
				// NSU
				"ntp.nsu.ru",
				// RSU
				"ntp.rsu.edu.ru",
				// National Institute of Information and Communications Technology
				"ntp.nict.jp",
				// NTT
				"x.ns.gin.ntt.net",
				"y.ns.gin.ntt.net",
				// HE.net
				"clock.nyc.he.net",
				"clock.sjc.he.net",
				// TRC Fiord
				"ntp.fiord.ru",
				// Netnod NTP service
				// Stratum 1:
				// Göteborg:
				"gbg1.ntp.se"
				"gbg2.ntp.se"
				// Malmö:
				"mmo1.ntp.se",
				"mmo2.ntp.se",
				// Stockholm:
				"sth1.ntp.se",
				"sth2.ntp.se",
				// Sundsvall:
				"svl1.ntp.se",
				"svl2.ntp.se",
				// Internet Systems Consortium
				"clock.isc.org",
				// NTP Pool
				"pool.ntp.org",
				"0.pool.ntp.org",
				"1.pool.ntp.org",
				"2.pool.ntp.org",
				"3.pool.ntp.org",
				"europe.pool.ntp.org",
				"0.europe.pool.ntp.org",
				"1.europe.pool.ntp.org",
				"2.europe.pool.ntp.org",
				"3.europe.pool.ntp.org",
				"asia.pool.ntp.org",
				"0.asia.pool.ntp.org",
				"1.asia.pool.ntp.org",
				"2.asia.pool.ntp.org",
				"3.asia.pool.ntp.org",
				"ru.pool.ntp.org",
				"0.ru.pool.ntp.org",
				"1.ru.pool.ntp.org",
				"2.ru.pool.ntp.org",
				"3.ru.pool.ntp.org",
				"0.gentoo.pool.ntp.org",
				"1.gentoo.pool.ntp.org",
				"2.gentoo.pool.ntp.org",
				"3.gentoo.pool.ntp.org",
				"0.arch.pool.ntp.org",
				"1.arch.pool.ntp.org",
				"2.arch.pool.ntp.org",
				"3.arch.pool.ntp.org",
				"0.fedora.pool.ntp.org",
				"1.fedora.pool.ntp.org",
				"2.fedora.pool.ntp.org",
				"3.fedora.pool.ntp.org",
				"0.opensuse.pool.ntp.org",
				"1.opensuse.pool.ntp.org",
				"2.opensuse.pool.ntp.org",
				"3.opensuse.pool.ntp.org",
				"0.centos.pool.ntp.org",
				"1.centos.pool.ntp.org",
				"2.centos.pool.ntp.org",
				"3.centos.pool.ntp.org",
				"0.debian.pool.ntp.org",
				"1.debian.pool.ntp.org",
				"2.debian.pool.ntp.org",
				"3.debian.pool.ntp.org",
				"0.ubuntu.pool.ntp.org",
				"1.ubuntu.pool.ntp.org",
				"2.ubuntu.pool.ntp.org",
				"3.ubuntu.pool.ntp.org",
				"0.askozia.pool.ntp.org",
				"1.askozia.pool.ntp.org",
				"2.askozia.pool.ntp.org",
				"3.askozia.pool.ntp.org",
				"0.freebsd.pool.ntp.org",
				"1.freebsd.pool.ntp.org",
				"2.freebsd.pool.ntp.org",
				"3.freebsd.pool.ntp.org",
				"0.netbsd.pool.ntp.org",
				"1.netbsd.pool.ntp.org",
				"2.netbsd.pool.ntp.org",
				"3.netbsd.pool.ntp.org",
				"0.openbsd.pool.ntp.org",
				"1.openbsd.pool.ntp.org",
				"2.openbsd.pool.ntp.org",
				"3.openbsd.pool.ntp.org",
				"0.dragonfly.pool.ntp.org",
				"1.dragonfly.pool.ntp.org",
				"2.dragonfly.pool.ntp.org",
				"3.dragonfly.pool.ntp.org",
				"0.pfsense.pool.ntp.org",
				"1.pfsense.pool.ntp.org",
				"2.pfsense.pool.ntp.org",
				"3.pfsense.pool.ntp.org",
				"0.opnsense.pool.ntp.org",
				"1.opnsense.pool.ntp.org",
				"2.opnsense.pool.ntp.org",
				"3.opnsense.pool.ntp.org",
				"0.smartos.pool.ntp.org",
				"1.smartos.pool.ntp.org",
				"2.smartos.pool.ntp.org",
				"3.smartos.pool.ntp.org",
				"0.android.pool.ntp.org",
				"1.android.pool.ntp.org",
				"2.android.pool.ntp.org",
				"3.android.pool.ntp.org",
				"0.amazon.pool.ntp.org",
				"1.amazon.pool.ntp.org",
				"2.amazon.pool.ntp.org",
				"3.amazon.pool.ntp.org",
			};
		}

		/** \brief Проверить занятость подключений к NTP
		 * \return Если с момента последнего запроса прошло недостаточное количество времени, вернет true
		 */
		inline bool busy_client() noexcept {
			std::lock_guard<std::mutex> lock(config_mutex);
			for (size_t i = 0; i < clients.size(); ++i) {
				if (clients[i].busy()) return true;
			}
			return false;
		}

		/** \brief Проверить занятость пула подключений к NTP
		 * \return Если с момента последних запросов прошло недостаточное количество времени, вернет true
		 */
		inline bool busy() noexcept {
			if (next_timestamp == 0) return false;
			const struct timespec ts = get_timespec();
			if ((uint64_t)ts.tv_sec > next_timestamp) return false;
			return true;
		}

		/** \brief Проверить инициализацию смещения времени
		 * \return Вернет true в случае инициализации смещения времени
		 */
		inline bool init() noexcept {
			return is_init;
		}

		/** \brief Сделать замер времени
		 * \return Вернет true в случае успешного замера
		 */
		inline bool make_measurement() noexcept {
			std::lock_guard<std::mutex> lock(make_measurement_mutex);
			if (busy()) return false;

			int64_t delay_measurements = 0;
			size_t max_measurements = 0;
			// создаем необходимое количество клиентов NTP
			{
				std::lock_guard<std::mutex> lock(config_mutex);
				delay_measurements = config.delay_measurements;
				max_measurements = config.max_measurements;
				if (clients.size() != config.hosts.size()) {
					clients.resize(config.hosts.size());
				}
				std::deque<std::string> hosts;
				if (config.is_hosts_shuffle) {
					hosts = config.hosts;
					std::mt19937 gen(get_timespec().tv_nsec);
					std::shuffle(hosts.begin(), hosts.end(), gen);
				}
				for (size_t i = 0; i < clients.size(); ++i) {
					clients[i].set_host(hosts[i]);
				}
			}

			// делаем замер задержек по всему списку NTP серверов
			int index = 0;
			for (size_t i = 0; i < clients.size(); ++i) {
				if (clients[i].make_measurement()) {
					time_measurements.push_front(clients[i].get_offset_us());
					++index;
					std::lock_guard<std::mutex> lock(config_mutex);
					if (index > config.stop_measurements) break;
				}
			}
			// запоминаем время замера
			next_timestamp = get_timespec().tv_sec + delay_measurements;

			// срезаем лишние данные
			if (time_measurements.size() > max_measurements) {
				time_measurements.resize(max_measurements);
			}
			if (time_measurements.empty()) {
				is_init = false;
				return false;
			}

			// применяем медианный фильтр
			std::deque<int64_t> temp(time_measurements);
			std::sort(temp.begin(), temp.end());
			offset_us = temp[temp.size()/2];
			is_init = true;
			return true;
		}

		/** \brief Получить смещение времени в микросекундах
		 * \return Время смещения в микросекундах
		 */
		inline const int64_t get_offset_us() noexcept {
			return offset_us;
		}

		/** \brief Получить метку времени в микросекундах
		 * \return Метка времени в микросекундах
		 */
		inline const uint64_t get_timestamp_us() noexcept {
			const struct timespec ts = get_timespec();
			return (uint64_t)(1000000ll * (int64_t)ts.tv_sec + ((int64_t)ts.tv_nsec / 1000ll) + get_offset_us());
		}

		/** \brief Получить метку времени в миллисекундах
		 * \return Метка времени в миллисекундах
		 */
		inline const uint64_t get_timestamp_ms() noexcept {
			const struct timespec ts = get_timespec();
			return (uint64_t)(1000ll * (int64_t)ts.tv_sec + (((int64_t)ts.tv_nsec / 1000ll) + get_offset_us()) / 1000ll);
		}

		inline const uint64_t get_timestamp() noexcept {
			const struct timespec ts = get_timespec();
			return (uint64_t)((int64_t)ts.tv_sec + (((int64_t)ts.tv_nsec / 1000ll) + get_offset_us()) / 1000000ll);
		}

		inline const double get_ftimestamp() noexcept {
			return (double)get_timestamp_us() / 1000000.0d;
		}

		/** \brief Получить метку времени в микросекундах
		 * Данный метод возвращает время, которое не может уменьшаться
		 * \return Метка времени в микросекундах
		 */
		inline const uint64_t get_steady_timestamp_us() noexcept {
			const uint64_t t = get_timestamp_us();
			if (is_init) {
				if (last_timestamp_us == 0) last_timestamp_us = t;
				last_timestamp_us = std::max(t, (uint64_t)last_timestamp_us);
				return last_timestamp_us;
			}
			return t;
		}

		/** \brief Получить метку времени в миллисекундах
		 * Данный метод возвращает время, которое не может уменьшаться
		 * \return Метка времени в миллисекундах
		 */
		inline const uint64_t get_steady_timestamp_ms() noexcept {
			const uint64_t t = get_timestamp_ms();
			if (is_init) {
				if (last_timestamp_ms == 0) last_timestamp_ms = t;
				last_timestamp_ms = std::max(t, (uint64_t)last_timestamp_ms);
				return last_timestamp_ms;
			}
			return t;
		}

		/** \brief Получить метку времени
		 * Данный метод возвращает время, которое не может уменьшаться
		 * \return Метка времени
		 */
		inline const uint64_t get_steady_timestamp() noexcept {
			const uint64_t t = get_timestamp();
			if (is_init) {
				if (last_timestamp == 0) last_timestamp = t;
				last_timestamp = std::max(t, (uint64_t)last_timestamp);
				return last_timestamp;
			}
			return t;
		}

		/** \brief Получить метку времени в виде числа с плавающей запятой
		 * Данный метод возвращает время, которое не может уменьшаться
		 * \return Метка времени
		 */
		inline const double get_steady_ftimestamp() noexcept {
			return (double)get_steady_timestamp_us() / 1000000.0d;
		}
	}; // NtpClientPool

	class ntp {
	private:

		class NtpMeasurer {
		private:
			std::mutex init_mutex;
		public:
			std::future<void>	client_pool_future;
			NtpClientPool		client_pool;
			std::atomic<bool>	is_init;
			std::atomic<bool>	is_once;
			std::atomic<bool>	is_init_once;
			std::atomic<bool>	is_shutdown;

			NtpMeasurer() {
				is_init = false;
				is_shutdown = false;
				is_once = false;
				is_init_once = false;
			}

			void init(const bool use_async) {
				std::lock_guard<std::mutex> lock(init_mutex);
				if (is_init_once) return;
				is_init_once = true;
				if (use_async) {
					client_pool_future = std::async(std::launch::async,[&]() {
						while (!is_shutdown) {
							is_init = client_pool.make_measurement();
							is_once = true;
							std::this_thread::sleep_for(std::chrono::milliseconds(10));
						}
					});
				} else {
					is_init = client_pool.make_measurement();
					is_once = true;
				}
			}

			~NtpMeasurer() {
				is_shutdown = true;
				if(client_pool_future.valid()) {
					try {
						client_pool_future.wait();
						client_pool_future.get();
					}
					catch(const std::exception &e) {
						//std::cerr << "NtpMeasurer error: ~NtpMeasurer(), what: " << e.what() << std::endl;
					}
					catch(...) {
						//std::cerr << "NtpMeasurer error: ~NtpMeasurer()" << std::endl;
					}
				}
			}
		};

		static inline NtpMeasurer ntp_measurer;
		static inline std::mutex ntp_measurer_init_mutex;

	public:

		/** \brief Инициализировать NTP
		 * \param use_async Использовать асинхронные замеры
		 * \return Вернет true в случае успешной инициализации или есть NTP уже инициализировано
		 */
		inline static bool init(const std::deque<std::string> &hosts = {}, const bool use_async = true) {
			std::lock_guard<std::mutex> lock(ntp_measurer_init_mutex);
			if (ntp_measurer.is_init) return true;
			if (!hosts.empty()) ntp_measurer.client_pool.set_hosts(hosts);
			ntp_measurer.init(use_async);
			const int max_tick = 5000;
			int tick = 0;
			while (!ntp_measurer.is_once && !ntp_measurer.is_shutdown && tick < max_tick) {
				++tick;
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			return ntp_measurer.is_init;
		};

		/** \brief Сделать замер
		 * \return Вернет true в случае успеха
		 */
		inline static bool make_measurement() noexcept {
			return ntp_measurer.client_pool.make_measurement();
		}

		/** \brief Установить время задержки между измерениями
		 * \param value Задержка между измерениями в секундах
		 */
		inline static void set_delay_measurements(const int value) noexcept {
			ntp_measurer.client_pool.set_delay_measurements(value);
		}

		/** \brief Установить количество измерений для остановки измерений
		 * \param value Количество измерений
		 */
		inline static void set_stop_measurements(const int value) noexcept {
			ntp_measurer.client_pool.set_stop_measurements(value);
		}

		/** \brief Установить максимальное количество измерений для медианного фильтра
		 * \param value Количество измерений
		 */
		inline static void set_max_measurements(const int value) noexcept {
			ntp_measurer.client_pool.set_max_measurements(value);
		}

		/** \brief Установить флаг перемешивания хостов NTP для создания случайного списка
		 * \param value Флаг перемешивания хостов NTP
		 */
		inline static void set_hosts_shuffle(const bool value) noexcept {
			ntp_measurer.client_pool.set_hosts_shuffle(value);
		}

		/** \brief Установить список хостов
		 * \param hosts Список хостов
		 */
		inline static void set_hosts(const std::deque<std::string> &hosts) noexcept {
			ntp_measurer.client_pool.set_hosts(hosts);
		}

		/** \brief Получить смещение времени в микросекундах
		 * \return Время смещения в микросекундах
		 */
		inline static const int64_t get_offset_us() noexcept {
			return ntp_measurer.client_pool.get_offset_us();
		}

		/** \brief Получить метку времени в микросекундах
		 * \return Метка времени в микросекундах
		 */
		inline static const uint64_t get_timestamp_us() noexcept {
			return ntp_measurer.client_pool.get_timestamp_us();
		}

		/** \brief Получить метку времени в миллисекундах
		 * \return Метка времени в миллисекундах
		 */
		inline static const uint64_t get_timestamp_ms() noexcept {
			return ntp_measurer.client_pool.get_timestamp_ms();
		}

		/** \brief Получить метку времени в секундах
		 * \return Метка времени в секундах
		 */
		inline static const uint64_t get_timestamp() noexcept {
			return ntp_measurer.client_pool.get_timestamp();
		}

		/** \brief Получить метку времени с плавающей запятой
		 * \return Метка времени с плавающей запятой
		 */
		inline static const double get_ftimestamp() noexcept {
			return ntp_measurer.client_pool.get_ftimestamp();
		}

		/** \brief Получить метку времени в микросекундах
		 * Данный метод возвращает время, которое не может уменьшаться
		 * \return Метка времени в микросекундах
		 */
		inline static const uint64_t get_steady_timestamp_us() noexcept {
			return ntp_measurer.client_pool.get_steady_timestamp_us();
		}

		/** \brief Получить метку времени в миллисекундах
		 * Данный метод возвращает время, которое не может уменьшаться
		 * \return Метка времени в миллисекундах
		 */
		inline static const uint64_t get_steady_timestamp_ms() noexcept {
			return ntp_measurer.client_pool.get_steady_timestamp_ms();
		}

		/** \brief Получить метку времени
		 * Данный метод возвращает время, которое не может уменьшаться
		 * \return Метка времени
		 */
		inline static const uint64_t get_steady_timestamp() noexcept {
			return ntp_measurer.client_pool.get_steady_timestamp();
		}

		/** \brief Получить метку времени в виде числа с плавающей запятой
		 * Данный метод возвращает время, которое не может уменьшаться
		 * \return Метка времени
		 */
		inline static const double get_steady_ftimestamp() noexcept {
			return ntp_measurer.client_pool.get_steady_ftimestamp();
		}
	}; // ntp
}; // ztime

#endif // ZTIME_NTP_HPP_INCLUDED
