/*
* ztime_cpp - Library for work with time.
*
* Copyright (c) 2018 Elektro Yar. Email: git.electroyar@gmail.com
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#ifndef ZTIME_HPP_INCLUDED
#define ZTIME_HPP_INCLUDED

#include <string>
#include <array>
#include <chrono>
#include <functional>
#include <algorithm>
#include <cmath>

#if __cplusplus <= 201103L
#	define ZTIME_CONSTEXPR
#else
#	define ZTIME_CONSTEXPR constexpr
#endif

namespace ztime {
	/* для того, чтобы можно было работать и после 19 января 2038 года,
	 * используем 64 бит, а не 32 бит
	 */
	typedef uint64_t timestamp_t;			///< Целочисленный тип метки врмени
	/* для работы с миллисекундами */
	typedef double ftimestamp_t;			///< Тип метки времени с плавающей точкой
	typedef double oadate_t;				///< Тип даты автоматизации (OADate) с плавающей точкой

	const timestamp_t TIMESTAMP_MAX = 0xFFFFFFFFFFFFFFFF;	///< Максимально возможное значение для типа timestamp_t
	const oadate_t OADATE_MAX = 9223372036854775807;		///< Максимально возможное значение даты автоматизации (OADate)
	const double AVERAGE_DAYS_IN_YEAR = 365.25;				///< Среднее количество дней за год

	/// Различные периоды
	enum {
		NANOSECONDS_IN_SECOND = 1000000000,	///< Количество наносекунд в одной секунде
		MICROSECONDS_IN_SECOND = 1000000,	///< Количество микросекунд в одной секунде
		MILLISECONDS_IN_SECOND = 1000,		///< Количество миллисекунд в одной секунде
		MILLISECONDS_IN_MINUTE = 60000,		///< Количество миллисекунд в одной минуте
		MILLISECONDS_IN_HALF_HOUR = 1800000,///< Количество миллисекунд в получасе
		MILLISECONDS_IN_HOUR = 3600000,		///< Количество миллисекунд в часе
		MILLISECONDS_IN_DAY = 86400000,		///< Количество миллисекунд в одном дне
		SECONDS_IN_MINUTE = 60,				///< Количество секунд в одной минуте
		SECONDS_IN_HALF_HOUR = 1800,		///< Количество секунд в получасе
		SECONDS_IN_HOUR = 3600,				///< Количество секунд в часе
		SECONDS_IN_DAY = 86400,				///< Количество секунд в дне
		SECONDS_IN_YEAR = 31536000,			///< Количество секунд за год
		SECONDS_IN_LEAP_YEAR = 31622400,	///< Количество секунд за високосный год
		AVERAGE_SECONDS_IN_YEAR = 31557600, ///< Среднее количество секунд за год
		SECONDS_IN_4_YEAR = 126230400,		///< Количество секунд за 4 года
		MINUTES_IN_HOUR = 60,				///< Количество минут в одном часе
		MINUTES_IN_DAY = 1440,				///< Количество минут в одном дне
		MINUTES_IN_WEEK = 10080,			///< Количество минут в одной неделе
		MINUTES_IN_MONTH = 40320,			///< Количество минут в одном месяце
		HOURS_IN_DAY = 24,					///< Количество часов в одном дне
		MONTHS_IN_YEAR = 12,				///< Количество месяцев в году
		DAYS_IN_WEEK = 7,					///< Количество дней в неделе
		DAYS_IN_LEAP_YEAR = 366,			///< Количество дней в високосом году
		DAYS_IN_YEAR = 365,					///< Количество дней в году
		DAYS_IN_4_YEAR = 1461,				///< Количество дней за 4 года
		FIRST_YEAR_UNIX = 1970,				///< Год начала UNIX времени
		MAX_DAY_MONTH = 31,					///< Максимальное количество дней в месяце
		OADATE_UNIX_EPOCH = 25569,			///< Дата автоматизации OLE с момента эпохи UNIX
	};

	/// Скоращенные имена дней неделии
	enum {
		SUN = 0,	///< Воскресенье
		MON,		///< Понедельник
		TUS,		///< Вторник
		WED,		///< Среда
		THU,		///< Четверг
		FRI,		///< Пятница
		SAT,		///< Суббота
	};

	/// Скоращенные имена месяцев
	enum {
		JAN = 1,	///< Январь
		FEB,		///< Февраль
		MAR,		///< Март
		APR,		///< Апрель
		MAY,		///< Май
		JUNE,		///< Июнь
		JULY,		///< Июль
		AUG,		///< Август
		SEPT,		///< Сентябрь
		OCT,		///< Октябрь
		NOV,		///< Ноябрь
		DEC,		///< Декабрь
	};

	/// Фазы Луны
	enum {
		WAXING_CRESCENT_MOON,
		FIRST_QUARTER_MOON,
		WAXING_GIBBOUS_MOON,
		FULL_MOON,
		WANING_GIBBOUS_MOON,
		LAST_QUARTER_MOON,
		WANING_CRESCENT_MOON,
		NEW_MOON,
	};

	template<class T>
	struct Period {
		T start;
		T stop;

		Period() : start(0), stop(0) {};

		Period(T &a, T &b) : start(a), stop(b) {};

		Period(const T &a, const T &b) : start(a), stop(b) {};
	};

	typedef Period<uint64_t> period_t;	///< Целочисленный тип периода времени
	typedef Period<double> fperiod_t;	///< Тип периода времени с плавающей точкой

	const std::array<std::string, MONTHS_IN_YEAR> month_name_long = {
		"January","February","March",
		"April","May","June",
		"July","August","September",
		"October","November","December",
	}; /**< Длинные имена месяцев */

	const std::array<std::string, MONTHS_IN_YEAR> month_name_short = {
		"Jan","Feb","Mar",
		"Apr","May","June",
		"July","Aug","Sept",
		"Oct","Nov","Dec",
	}; /**< Сокращенные имена месяцев */

	const std::array<std::string, DAYS_IN_WEEK> weekday_name_short = {
		"SUN",
		"MON",
		"TUS",
		"WED",
		"THU",
		"FRI",
		"SAT",
	}; /**< Сокращенные имена дней недели */

	/** \brief Получить миллисекунду секунды
	 * \return Миллисекунда секунды
	 */
	const uint32_t get_millisecond() noexcept;

	/** \brief Получить микросекунду секунды
	 * \return Микросекунда секунды
	 */
	const uint32_t get_microsecond() noexcept;

	/** \brief Получить наносекунду секунды
	 * \return Наносекунда секунды
	 */
	const uint32_t get_nanosecond() noexcept;

	/** \brief Получить метку времени компьютера
	 * \return Метка времени
	 */
	const timestamp_t get_timestamp() noexcept;

	/** \brief Получить метку времени компьютера в миллисекундах
	 * \return Метка времени в миллисекундах
	 */
	const timestamp_t get_timestamp_ms() noexcept;

	/** \brief Получить метку времени компьютера в микросекундах
	 * \return Метка времени в микросекундах
	 */
	const timestamp_t get_timestamp_us() noexcept;

	/** \brief Получить метку времени
	 * \param value Cтроковое представление метки времени
	 * \return Метка времени
	 */
	timestamp_t get_timestamp(std::string value);

	/** \brief Получить метку времени
	 * \param value строковое представление числа
	 * \return Метка времени
	 */
	ftimestamp_t get_ftimestamp(const std::string &value);

	/** \brief Получить метку времени с плавающей запятой
	 * \return Метка времени с плавающей запятой
	 */
	const ftimestamp_t get_ftimestamp() noexcept;

	/** \brief Получить метку времени из даты и стандартного времени
	 * \param day		День
	 * \param month		Месяц
	 * \param year		Год
	 * \param hour		Час
	 * \param minute	Минуты
	 * \param second	Секунды
	 * \return Метка времени
	 */
	ZTIME_CONSTEXPR inline timestamp_t get_timestamp(
			const uint32_t day,
			const uint32_t month,
			const uint32_t year,
			const uint32_t hour = 0,
			const uint32_t minute = 0,
			const uint32_t second = 0) noexcept {
		long _mon = month - 1;
		const long _TBIAS_YEAR = 1900;
		const long	lmos[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
		const long	mos[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
		long _year = year - _TBIAS_YEAR;
		// для предотвращения проблемы 2038 года переменная должна быть больше 32 бит
		long long _days = (((_year - 1) / 4) + ((((_year) & 03) || ((_year) == 0)) ? mos[_mon] : lmos[_mon])) - 1;
		_days += DAYS_IN_YEAR * _year;
		_days += day;
		const long _TBIAS_DAYS = 25567;
		_days -= _TBIAS_DAYS;
		timestamp_t _secs = SECONDS_IN_HOUR * hour;
		_secs += SECONDS_IN_MINUTE * minute;
		_secs += second;
		_secs += _days * SECONDS_IN_DAY;
		return _secs;
	}

	/** \brief Получить метку времени с миллисекундами из даты и стандартного времени
	 * \param day день
	 * \param month месяц
	 * \param year год
	 * \param hour час
	 * \param minutes минуты
	 * \param seconds секунды
	 * \param milliseconds миллисекунды
	 * \return метка времени
	 */
	ZTIME_CONSTEXPR inline ftimestamp_t get_ftimestamp(
			const uint32_t day,
			const uint32_t month,
			const uint32_t year,
			const uint32_t hour = 0,
			const uint32_t minute = 0,
			const uint32_t second = 0,
			const uint32_t millisecond = 0) noexcept {
		timestamp_t t = get_timestamp(day, month, year, hour, minute, second);
		return (double)t + (double)millisecond/1000.0;
	}

	/* Дата автоматизации OLE реализована в виде числа с плавающей запятой,
	 * неотъемлемым компонентом которого является число дней до
	 * или после полуночи 30 декабря 1899 года,
	 * а дробный компонент представляет время этого дня, деленное на 24.
	 * Например, полночь 31 декабря 1899 представлен 1,0; 6 утра,
	 * 1 января 1900 года представлено 2,25;
	 * полночь 29 декабря 1899 года представлена -1,0;
	 * и 6 часов утра 29 декабря 1899 года - -1,25.
	 *
	 * Базовая дата автоматизации OLE - полночь 30 декабря 1899 года.
	 * Минимальная дата автоматизации OLE - полночь 1 января 0100 года.
	 * Максимальная дата автоматизации OLE такая же,
	 * как DateTime.MaxValue, последний момент 31 декабря 9999 года.
	 */

	/** \brief Получить дату автоматизации OLE из метки времени
	 * \param timestamp Метка времени
	 * \return Дата автоматизации OLE
	 */
	constexpr inline oadate_t convert_timestamp_to_oadate(const timestamp_t timestamp) noexcept {
		return (const oadate_t)OADATE_UNIX_EPOCH + (const oadate_t)timestamp / (const oadate_t)SECONDS_IN_DAY;
	}

	/** \brief Получить дату автоматизации OLE из метки времени c плавающей точкой
	 * \param timestamp Метка времени c плавающей точкой
	 * \return Дата автоматизации OLE
	 */
	constexpr inline oadate_t convert_ftimestamp_to_oadate(const ftimestamp_t timestamp) noexcept {
		return (const oadate_t)OADATE_UNIX_EPOCH + (const oadate_t)timestamp / (const oadate_t)SECONDS_IN_DAY;
	}

	/** \brief Преобразовать дату автоматизации OLE в метку времени
	 * \param oadate Дата автоматизации OLE
	 * \return Метка времени
	 */
	ZTIME_CONSTEXPR inline timestamp_t convert_oadate_to_timestamp(const oadate_t oadate) noexcept {
		if(oadate < (const oadate_t)OADATE_UNIX_EPOCH) return 0;
		return (timestamp_t)((oadate - (oadate_t)OADATE_UNIX_EPOCH) * (oadate_t)SECONDS_IN_DAY);
	}

	/** \brief Преобразовать дату автоматизации OLE в метку времени	 плавающей точкой
	 * \param oadate Дата автоматизации OLE
	 * \return Метка времени с плавающей точкой
	 */
	ZTIME_CONSTEXPR inline ftimestamp_t convert_oadate_to_ftimestamp(const oadate_t oadate) noexcept {
		if(oadate < (const oadate_t)OADATE_UNIX_EPOCH) return 0;
		return (ftimestamp_t)((oadate - (oadate_t)OADATE_UNIX_EPOCH) * (oadate_t)SECONDS_IN_DAY);
	}

	/** \brief Получить дату автоматизации OLE
	 * \return Дата автоматизации OLE
	 */
	inline oadate_t get_oadate() noexcept {
		return convert_ftimestamp_to_oadate(get_ftimestamp());
	}

	/** \brief Получить дату автоматизации OLE
	 * \param day			День
	 * \param month			Месяц
	 * \param year			Год
	 * \param hour			Час
	 * \param minute		Минуты
	 * \param second		Секунды
	 * \param millisecond	Миллисекунды
	 * \return Дата автоматизации OLE
	 */
	ZTIME_CONSTEXPR inline oadate_t get_oadate(
			const uint32_t day,
			const uint32_t month,
			const uint32_t year,
			const uint32_t hour = 0,
			const uint32_t minute = 0,
			const uint32_t second = 0,
			const uint32_t millisecond = 0) noexcept {
		return convert_ftimestamp_to_oadate(get_ftimestamp(
			day,
			month,
			year,
			hour,
			minute,
			second,
			millisecond));
	}

	#define get_ole_automation_date get_oadate

	/** \brief Получить юлианскую дату (JD) с дробной частью по метке времени
	 * \param timestamp Метка времени с дробной частью
	 * \return Число дней по юлианскому календарю с дробной частью (JD)
	 */
	constexpr inline double get_julian_date(const ftimestamp_t timestamp) {
		return 2440587.5 + (timestamp / 86400.0d);
	}

	/** \brief Получить юлианскую дату (JD) с дробной частью по дате григорианского календаря
	 *
	 * Исходник алгоритма: http://www.krutov.org/algorithms/julianday/
	 * \param day	День, может быть с дробной частью
	 * \param month Месяц
	 * \param year	Год
	 * \return Число дней по юлианскому календарю с дробной частью (JD)
	 */
	ZTIME_CONSTEXPR inline double get_julian_date(
			double day,
			int64_t month,
			int64_t year) {
		if (month == 1 || month == 2) {
			year -= 1;
			month += 12;
		}
		const double a = std::floor((double)year / 100.0d);
		const double b = 2.0d - a + std::floor((double)a / 4.0d);
		const double jdn = std::floor(365.25d * ((double)year + 4716.0d)) + std::floor(30.6000001d * ((double)month + 1.0d)) + (double)day + b - 1524.5d;
		return jdn;
	}

	/** \brief Получить юлианскую дату (JD) с дробной частью по дате григорианского календаря
	 *
	 * Исходник алгоритма: http://www.krutov.org/algorithms/julianday/
	 * \param day			День
	 * \param month			Месяц
	 * \param year			Год
	 * \param hour			Час
	 * \param minute		Минуты
	 * \param second		Секунды
	 * \param millisecond	Миллисекунды
	 * \return Число дней по юлианскому календарю с дробной частью (JD)
	 */
	ZTIME_CONSTEXPR inline double get_julian_date(
			const uint32_t day,
			const uint32_t month,
			const uint32_t year,
			const uint32_t hour,
			const uint32_t minute,
			const uint32_t second = 0,
			const uint32_t millisecond = 0) {
		const double offset = ((double)hour / 24.0d) + ((double)minute / 1440.0d) + (((double)second + ((double)millisecond / 1000.0d)) / 84600.0d);
		return get_julian_date((double)day + offset, month, year);
	}

	#define get_julian_day						get_julian_date
	#define convert_gregorian_to_julian_date	get_julian_date
	#define convert_gregorian_to_julian_day		get_julian_date
	#define convert_gregorian_to_julian			get_julian_date
	#define convert_gregorian_to_jd				get_julian_date
	#define to_julian_date						get_julian_date
	#define to_jd								get_julian_date

	/** \brief Получить модифицированный юлианский день (MJD) с дробной частью по дате григорианского календаря
	 *
	 * Исходник алгоритма: http://www.krutov.org/algorithms/julianday/
	 * \param day	День, может быть с дробной частью
	 * \param month Месяц
	 * \param year	Год
	 * \return Число дней по юлианскому календарю (JDN)
	 */
	ZTIME_CONSTEXPR inline double get_modified_julian_day(
			double day,
			int64_t month,
			int64_t year) {
		return get_julian_date(day, month, year) - 2400000.5d;
	}

	/** \brief Получить модифицированный юлианский день (MJD) с дробной частью по дате григорианского календаря
	 * \param timestamp Метка времени с дробной частью
	 * \return Число дней по юлианскому календарю (JDN)
	 */
	constexpr inline double get_modified_julian_day(const ftimestamp_t timestamp) {
		return get_julian_date(timestamp) - 2400000.5d;
	}

	#define convert_gregorian_to_modified_julian_day	get_modified_julian_day
	#define convert_gregorian_to_mjd					get_modified_julian_day
	#define to_modified_julian_day						get_modified_julian_day
	#define to_mjd										get_modified_julian_day

	/** \brief Получить число дней по юлианскому календарю (JDN) по дате григорианского календаря
	 * \param day	День
	 * \param month Месяц
	 * \param year	Год
	 * \return Число дней по юлианскому календарю (JDN)
	 */
	ZTIME_CONSTEXPR inline uint64_t get_julian_day_number(
			uint32_t day,
			uint32_t month,
			uint32_t year) {
		const uint64_t a = ((14LL - month) / 12LL);
		const uint64_t y = year + 4800LL - a;
		const uint64_t m = month + 12LL * a - 3LL;
		const uint64_t jdn = day + ((153LL * m + 2LL) / 5LL) + (365LL * y) + (y / 4LL) - (y / 100LL) + (y / 400LL) - 32045LL;
		return jdn;
	}

	#define convert_gregorian_to_julian_day_number		get_julian_day_number
	#define convert_gregorian_to_jdn					get_julian_day_number
	#define to_julian_day_number						get_julian_day_number
	#define to_jdn										get_julian_day_number

	/* проверкить JD и JDN можно тут:
	 * http://cosmos-online.myhomeinet.ru/jd.php
	 * https://www.aavso.org/jd-calculator
	 * http://www.michurin.net/online-tools/julian-day.html
	 * https://quasar.as.utexas.edu/BillInfo/JulianDateCalc.html
	 */

	/** \brief Получить фазу Луны
	 *
	 * Алгоритм: https://web.archive.org/web/20090218203728/http://home.att.net/~srschmitt/lunarphasecalc.html
	 * \param timestamp Метка времени с плавающей точкой
	 * \return Вернет значение от 0 до 1
	 */
	ZTIME_CONSTEXPR inline double get_moon_phase(const ftimestamp_t timestamp) {
		double temp = (get_julian_date(timestamp) - 2451550.1d) / 29.530588853d;
		temp = temp - std::floor(temp);
		if (temp < 0) temp += 1.0d;
		return temp;
	}

	/** \brief Получить возраст Луны в днях
	 *
	 * Алгоритм: https://web.archive.org/web/20090218203728/http://home.att.net/~srschmitt/lunarphasecalc.html
	 * \param timestamp Метка времени с плавающей точкой
	 * \return Вернет значение от 0 до 29.53
	 */
	ZTIME_CONSTEXPR inline double get_moon_age(const ftimestamp_t timestamp) {
		return get_moon_phase(timestamp) * 29.53d;
	}

	/** \brief Получить время и дату в виде строки
	 * Формат строки: DD.MM.YYYY HH:MM:SS
	 * Если не указать метку времени, данная функция вернет строку с UTC/GMT временем компьютера
	 * \param timestamp метка времен
	 * \return строка, содержащая время и дату
	 */
	std::string get_str_date_time(const timestamp_t timestamp = get_timestamp());

	/** \brief Получить время и дату в виде строки
	 * Формат строки: DD.MM.YYYY HH:MM:SS.sss
	 * Если не указать метку времени, данная функция вернет строку с UTC/GMT временем компьютера
	 * \param timestamp метка времен
	 * \return строка, содержащая время и дату
	 */
	std::string get_str_date_time_ms(const ftimestamp_t timestamp = get_ftimestamp());

	/** \brief Получить дату в виде строки
	 * Формат строки: DD.MM.YYYY
	 * Если не указать метку времени, данная функция вернет строку с UTC/GMT временем компьютера
	 * \param timestamp метка времен
	 * \return строка, содержащая дату
	 */
	std::string get_str_date(const timestamp_t timestamp = get_timestamp());

	/** \brief Получить время в виде строки
	 * Формат строки: HH:MM:SS или HH:MM
	 * Если не указать метку времени, данная функция вернет строку с UTC/GMT временем компьютера
	 * \param timestamp метка времен
	 * \param is_use_seconds Флаг, который добавляет секунды
	 * \return строка, содержащая время
	 */
	std::string get_str_time(const timestamp_t timestamp = get_timestamp(), const bool is_use_seconds = true);

	/** \brief Получить время с миллисекундами в виде строки
	 * Формат строки: HH:MM:SS.sss
	 * Если не указать метку времени, данная функция вернет строку с UTC/GMT временем компьютера
	 * \param timestamp метка времен
	 * \return строка, содержащая время
	 */
	std::string get_str_time_ms(const ftimestamp_t timestamp = get_ftimestamp());

	/** \brief Получить строку в формате ISO 8601
	 * Функция возвращает строку с временем и датой,
	 * сформированную по стандарту ISO 8601,
	 * в котором строка имеет следующий формат: YYYY-MM-DDThh:mm:ss.sssZ.
	 * Если установлен часовой пояс, то буква "Z" заменяется значением +hh:mm или -hh.mm
	 * \param timestamp Метка времен
	 * \param offset Смещение в секундах, определяет часовой пояс.
	 * \return Строка, содержащая время
	 */
	std::string get_str_iso_8601(const ztime::ftimestamp_t timestamp, const int64_t offset = 0);

	/** \brief Преобразовать метку времени в строку с пользовательскими настройками преобразования
	 * В качестве параметров функция принимает следующие аргументы:
	 * %YYYY - год
	 * %YY - последние два числа года
	 * %Y - год, без ограничений в 4 символа
	 * %MM - месяц
	 * %DD - день месяца
	 * %hh - час дня
	 * %mm - минуты часа
	 * %ss - секунды
	 * %sss - миллисекунды
	 * %W - день недели (число)
	 * %w - день недели (скоращенное имя)
	 * \param mode Строка с параметрами пользователя, например "%YYYY-%MM-%DD"
	 * \param timestamp Метка времени
	 * \return Вернет строку в формате, заданным пользователем
	 */
	std::string to_string(const std::string &mode, const ztime::ftimestamp_t timestamp = get_ftimestamp());

	/** \brief Класс для хранения времени
	 */
	class DateTime {
	public:
		uint32_t year;			/**< год */
		uint32_t millisecond;	/**< миллисекунды */
		uint32_t microsecond;	/**< микросекунды */
		uint32_t nanosecond;	/**< наносекунды */
		uint8_t second;			/**< секунды */
		uint8_t minute;			/**< минуты */
		uint8_t hour;			/**< час */
		uint8_t day;			/**< день */
		uint8_t month;			/**< месяц */

		bool is_correct();

		/** \brief Установить начало дня
		 * Данная функция устанавливает час, минуту и секунду дня в 0
		 */
		inline void set_beg_day() noexcept {
			nanosecond = 0;
			microsecond = 0;
			millisecond = 0;
			second = 0;
			minute = 0;
			hour = 0;
		}

		/** \brief Установить конец дня
		 */
		inline void set_end_day() noexcept {
			nanosecond = 999999999;
			microsecond = 999999;
			millisecond = 999;
			second = 59;
			minute = 59;
			hour = 23;
		}

		/** \brief Установить начало месяца
		 */
		inline void set_beg_month() noexcept {
			set_beg_day();
			day = 1;
		}

		/** \brief Установить конец месяца
		 */
		void set_end_month();

		/** \brief Конструктор класса без начальной инициализации времени
		 */
		DateTime();

		/** \brief Инициализация с указанием времени и даты
		 * \param day день
		 * \param month месяц
		 * \param year год
		 * \param hour час
		 * \param minute минуты
		 * \param second секунды
		 * \param millisecond миллисекунды
		 */
		DateTime(
			const uint32_t day,
			const uint32_t month,
			const uint32_t year,
			const uint32_t hour = 0,
			const uint32_t minute = 0,
			const uint32_t second = 0,
			const uint32_t millisecond = 0);

		/** \brief Инициализация с указанием unix-времени
		 * \param timestamp метка времени
		 */
		DateTime(const timestamp_t timestamp);

		/** \brief Инициализация с указанием unix-времени
		 * \param timestamp метка времени
		 */
		DateTime(const ftimestamp_t ftimestamp);

		/** \brief Инициализация с указанием unix-времени в формате ISO
		 * Пример формата ISO: 2013-12-06T15:23:01+00:00
		 * \param str_iso_formatted_utc_datetime время UTC в формате ISO
		 */
		DateTime(const std::string &str_iso_formatted_utc_datetime);

		/** \brief Получить метку времени
		 * \return timestamp
		 */
		timestamp_t get_timestamp();

		/** \brief Получить метку времени с миллисекундами
		 * \return timestamp
		 */
		ftimestamp_t get_ftimestamp();

		/** \brief Установить время
		 * \param timestamp Метка времени
		 */
		void set_timestamp(const timestamp_t timestamp);

		/** \brief Установить время с плавающей запятой
		 * \param timestamp Метка времени
		 */
		void set_ftimestamp(const ftimestamp_t ftimestamp);

		/** \brief Вывести время и дату на экран
		 */
		void print();

		/** \brief Получить дату и время в виде строки
		 * Формат строки: DD.MM.YYYY HH:MM:SS
		 * \return строка, содержащая дату и время
		 */
		std::string get_str_date_time();

		/** \brief Получить дату и время в виде строки
		 * Формат строки: DD.MM.YYYY HH:MM:SS
		 * \return строка, содержащая дату и время
		 */
		std::string get_str_date_time_ms();

		/** \brief Получить дату в виде строки
		 * Формат строки: DD.MM.YYYY
		 * \return строка, содержащая дату
		 */
		std::string get_str_date();

		/** \brief Получить время в виде строки
		 * Формат строки: HH:MM:SS
		 * \param is_use_seconds Флаг, включает использование секунд
		 * \return строка, содержащая время
		 */
		std::string get_str_time(const bool is_use_seconds = true);

		/** \brief Получить время в виде строки
		 * Формат строки: HH:MM:SS.sss
		 * \return строка, содержащая время
		 */
		std::string get_str_time_ms();

		/** \brief Получить день недели
		 * \return день недели (SUN = 0, MON = 1, ... SAT = 6)
		 */
		uint32_t get_weekday();

		/** \brief Проверка високосного года
		 * \return вернет true, если год високосный
		 */
		bool is_leap_year();

		/** \brief Получить количество дней в текущем месяце
		 * \return количество дней
		 */
		uint32_t get_num_days_current_month();

		/** \brief Получить дату автоматизации OLE
		 * \return дата автоматизации OLE
		 */
		oadate_t get_oadate();

		/** \brief Установить дату автоматизации OLE
		 * \param oadate Дата автоматизации OLE
		 */
		void set_oadate(const oadate_t oadate);
	};

	/** \brief Таймер для измерений задержек
	 */
	class Timer {
	private:
		/* steady_clock представляет собой монотонные часы.
		 * Точки времени на этих часах не могут уменьшаться по мере того,
		 * как физическое время движется вперед,
		 * и время между тактами этих часов остается постоянным.
		 * Эти часы не связаны со временем настенных часов
		 * (например, это может быть время с момента последней перезагрузки)
		 * и больше всего подходят для измерения интервалов.
		 */
		using clock_t = std::chrono::steady_clock;
		using second_t = std::chrono::duration<double, std::ratio<1>>;

		std::chrono::time_point<clock_t> start_time;
		double sum = 0;
		uint64_t counter = 0;
	public:
		Timer() : start_time(clock_t::now()) {}

		/** \brief Сбросить значение таймера
		 *
		 * Данный метод нужно применять только вместе с методом elapsed()
		 * При использовании метода
		 * get_average_measurements() сбрасывать таймер не нужно!
		 */
		inline void reset() {
			start_time = clock_t::now();
		}

		/** \brief Получить замер времени
		 * \return Время в секундах с момента инициализации класса или после reset()
		 */
		inline double get_elapsed() const {
			return std::chrono::duration_cast<second_t>(clock_t::now() - start_time).count();
		}

		/** \brief Сбросить все замеры
		 *
		 * Данный метод обнуляет сумму замеров и их количество
		 */
		inline void reset_measurement() {
			sum = 0;
			counter = 0;
		}

		/** \brief Начать замер
		 *
		 * Данный метод использовать вместе с методами stop_measurement()
		 * и get_average_measurements()
		 */
		inline void start_measurement() {
			reset();
		}

		/** \brief Остановить замер
		 *
		 * Данный метод использовать вместе с методами start_measurement()
		 * и get_average_measurements()
		 */
		inline void stop_measurement() {
			sum += get_elapsed();
			++counter;
		}

		/** \brief Получить результаты замеров
		 *
		 * Данный метод использовать вместе с методами start_measurement()
		 * и stop_measurement(). Метод возвращает средний результат замеров
		 * \return Среднее время замеров в секундах
		 */
		inline double get_average_measurements() const {
			return sum / (double)counter;
		}
	};

	/** \brief Конвертировать строку в формате ISO в данные класса DateTime
	 * \param str_datetime	Cтрока в формате ISO, например 2013-12-06T15:23:01+00:00
	 * \param t				Класс времени и даты DateTime, который будет заполнен.
	 * \return Вернет true если преобразование завершилось успешно
	 */
	bool convert_iso(
		const std::string &str_datetime,
		DateTime& t);

	/** \brief Конвертировать строку в формате ISO в метку времени с дробной частью
	 * \param str_datetime	Строка в формате ISO, например 2013-12-06T15:23:01+00:00
	 * \return Вернет метку времени, если преобразование завершилось успешно, или 0 в случае провала
	 */
	ztime::ftimestamp_t convert_iso_to_ftimestamp(const std::string &str_datetime);

	/** \brief Преобразовать строку с датой в timestamp
	 *
	 * Данная функция поддерживает форматы времени:
	 * HH:MM:SS DD MM YY Пример: 20:25:00, 29 Aug 19
	 * HH:MM:SS DD MM YY Пример: 20:25:00, 29 Aug 2019
	 * HH:MM:SS DD.MM.YYYY Пример: 00:59:59 30.08.2019
	 * HH:MM:SS DD-MM-YYYY Пример: 00:59:59 30-08-2019
	 * YYYY-MM-DD hh:mm:ss Пример: 2013-02-25 18:25:10
	 * YYYY.MM.DD hh:mm:ss Пример: 2013.02.25 18:25:10
	 * YYYY.MM.DD Пример: 2013.02.25
	 * DD.MM.YYYY Пример: 21.09.2018
	 * \param str_datetime	Время в формате строки
	 * \return Вернет метку времени, если преобразование завершилось успешно, или 0 в случае провала
	 */
	ztime::timestamp_t to_timestamp(std::string str_datetime);

	/** \brief Преобразовать строку с временем дня в секунду дня
	 *
	 * Данная функция поддерживает форматы времени:
	 * HH:MM:SS Пример: 23:25:59
	 * HH:MM	Пример: 23:25
	 * HH		Пример: 23
	 * \param str_time	Время в формате строки
	 * \return Вернет секунду дня, если преобразование завершилось успешно, или -1 в случае провала
	 */
	int to_second_day(std::string str_time);

	/** \brief Получить день недели
	 * \param day день
	 * \param month месяц
	 * \param year год
	 * \return день недели (SUN = 0, MON = 1, ... SAT = 6)
	 */
	uint32_t get_weekday(
		const uint32_t day,
		const uint32_t month,
		const uint32_t year);

	/** \brief Получить день недели
	 * \param timestamp метка времени
	 * \return день недели (SUN = 0, MON = 1, ... SAT = 6)
	 */
	constexpr inline uint32_t get_weekday(const timestamp_t timestamp) noexcept {
		return ((timestamp / SECONDS_IN_DAY + THU) % DAYS_IN_WEEK);
	}

	/** \brief Получить номер месяца по названию
	 * \param month		Имя месяца
	 * \return номер месяца
	 */
	uint32_t get_month(std::string month);

	/** \brief Получить день месяца
	 * \param timestamp		Метка времени
	 * \return День месяца
	 */
	uint32_t get_day_month(const timestamp_t timestamp = get_timestamp());

	/** \brief Преобразует строку в timestamp
	 *
	 * Данная функция поддерживает форматы времени:
	 * HH:MM:SS DD MM YY Пример: 20:25:00, 29 Aug 19
	 * HH:MM:SS DD MM YY Пример: 20:25:00, 29 Aug 2019
	 * HH:MM:SS DD.MM.YYYY Пример: 00:59:59 30.08.2019
	 * HH:MM:SS DD-MM-YYYY Пример: 00:59:59 30-08-2019
	 * YYYY-MM-DD hh:mm:ss Пример: 2013-02-25 18:25:10
	 * YYYY.MM.DD hh:mm:ss Пример: 2013.02.25 18:25:10
	 * YYYY.MM.DD Пример: 2013.02.25
	 * DD.MM.YYYY Пример: 21.09.2018
	 * \param str   Время в формате строки, например
	 * \param t     Метка времени
	 * \return Вернет true если преобразование завершилось успешно
	 */
	bool convert_str_to_timestamp(std::string str, timestamp_t& t);

	/** \brief Преобразовать unix-время в класс DateTime
	 * \param timestamp метка времени
	 * \return класс DateTime
	 */
	DateTime convert_timestamp_to_datetime(const timestamp_t timestamp = get_timestamp());

	/** \brief Напечатать дату и время
	 * \param timestamp метка времени
	 */
	void print_date_time(const timestamp_t timestamp = get_timestamp());

	/** \brief Получить количество дней в месяце
	 * \param month месяц
	 * \param year год
	 * \return количество дней в месяце
	 */
	uint32_t get_num_days_month(const uint32_t month, const uint32_t year);

	/** \brief Получить количество дней в месяце
	 * \param timestamp метка времени
	 * \return количество дней в месяце
	 */
	uint32_t get_num_days_month(const timestamp_t timestamp = get_timestamp());

	/** \brief Переводит время GMT во время CET
	 * До 2002 года в Европе переход на летнее время осуществлялся в последнее воскресенье марта в 2:00 переводом часов на 1 час вперёд
	 * а обратный переход осуществлялся в последнее воскресенье октября в 3:00 переводом на 1 час назад
	 * Начиная с 2002 года, согласно директиве ЕС(2000/84/EC) в Европе переход на летнее время осуществляется в 01:00 по Гринвичу.
	 * \param gmt метка времени, время GMT
	 * \return время CET
	 */
	timestamp_t convert_gmt_to_cet(const timestamp_t gmt);

	/** \brief Переводит время GMT во время EET
	 * До 2002 года в Европе переход на летнее время осуществлялся в последнее воскресенье марта в 2:00 переводом часов на 1 час вперёд
	 * а обратный переход осуществлялся в последнее воскресенье октября в 3:00 переводом на 1 час назад
	 * Начиная с 2002 года, согласно директиве ЕС(2000/84/EC) в Европе переход на летнее время осуществляется в 01:00 по Гринвичу.
	 * \param gmt метка времени, время GMT
	 * \return время EET
	 */
	timestamp_t convert_gmt_to_eet(const timestamp_t gmt);

	/** \brief Переводит время GMT во время MSK
	 * \param gmt метка времени, время GMT
	 * \return время MSK
	 */
	timestamp_t convert_gmt_to_msk(const timestamp_t gmt);

	/** \brief Переводит время CET во время GMT
	 * До 2002 года в Европе переход на летнее время осуществлялся в последнее воскресенье марта в 2:00 переводом часов на 1 час вперёд
	 * а обратный переход осуществлялся в последнее воскресенье октября в 3:00 переводом на 1 час назад
	 * Начиная с 2002 года, согласно директиве ЕС(2000/84/EC) в Европе переход на летнее время осуществляется в 01:00 по Гринвичу.
	 * \param cet метка времени c измененым часовым поясом, время CET
	 * \return время GMT
	 */
	timestamp_t convert_cet_to_gmt(const timestamp_t cet);

	/** \brief Переводит время EET во время GMT
	 * До 2002 года в Европе переход на летнее время осуществлялся в последнее воскресенье марта в 2:00 переводом часов на 1 час вперёд
	 * а обратный переход осуществлялся в последнее воскресенье октября в 3:00 переводом на 1 час назад
	 * Начиная с 2002 года, согласно директиве ЕС(2000/84/EC) в Европе переход на летнее время осуществляется в 01:00 по Гринвичу.
	 * \param eet метка времени c измененым часовым поясом, время EET
	 * \return время GMT
	 */
	timestamp_t convert_eet_to_gmt(const timestamp_t eet);

	/** \brief Переводит время MSK во время GMT
	 * \param msk метка времени c измененым часовым поясом, время MSK
	 * \return время GMT
	 */
	timestamp_t convert_msk_to_gmt(const timestamp_t msk);

	/** \brief Проверить начало получаса
	 * \param timestamp метка времени
	 * \return вернет true, если начало получаса
	 */
	inline bool is_beg_half_hour(const timestamp_t timestamp = get_timestamp()) {
		return timestamp % SECONDS_IN_HALF_HOUR == 0;
	}

	/** \brief Проверить начало часа
	 * \param timestamp метка времени
	 * \return вернет true, если начало часа
	 */
	inline bool is_beg_hour(const timestamp_t timestamp = get_timestamp()) {
		return timestamp % SECONDS_IN_HOUR == 0;
	}

	/** \brief Проверить начало дня
	 * \param timestamp метка времени
	 * \return вернет true, если начало дня
	 */
	inline bool is_beg_day(const timestamp_t timestamp = get_timestamp()) {
		return (timestamp % SECONDS_IN_DAY == 0);
	}

	/** \brief Проверить начало недели
	 * Если день воскресенье, функция вернет true
	 * \warning Данная функция проверят только день! Она не проверяет секунды, минуты, часы
	 * \param timestamp метка времени
	 * \return вернет true, если начало недели
	 */
	inline bool is_beg_week(const timestamp_t timestamp = get_timestamp()){
		return get_weekday(timestamp) == SUN;
	}

	/** \brief Проверить начало месяца
	 * Если первый день месяца, функция вернет true
	 * \warning Данная функция проверят только день! Она не проверяет секунды, минуты, часы
	 * \param timestamp метка времени
	 * \return вернет true, если начало месяца
	 */
	inline bool is_beg_month(timestamp_t timestamp = get_timestamp()) {
		return get_day_month(timestamp) == 1;
	}

	/** \brief Проверить конец месяца
	 * Если последний день месяца, функция вернет true
	 * \warning Данная функция проверят только день! Она не проверяет секунды, минуты, часы
	 * \param timestamp метка времени
	 * \return вернет true, если конец месяца
	 */
	bool is_end_month(const timestamp_t timestamp = get_timestamp());

	/** \brief Проверить корректность даты
	 * \param day день
	 * \param month месяц
	 * \param year год
	 * \return вернет true, если заданная дата возможна
	 */
	bool is_correct_date(const uint32_t day, const uint32_t month, const uint32_t year);

	/** \brief Проверить корректность времени
	 * \param hour час
	 * \param minutes минуты
	 * \param seconds секунды
	 * \param milliseconds миллисекунды
	 * \return вернет true, если заданное время корректно
	 */
	bool is_correct_time(
		const uint32_t hour,
		const uint32_t minutes = 0,
		const uint32_t seconds = 0,
		const uint32_t milliseconds = 0);

	/** \brief Проверить корректность даты и времени
	 * \param day день
	 * \param month месяц
	 * \param year год
	 * \param hour час
	 * \param minutes минуты
	 * \param seconds секунды
	 * \param milliseconds миллисекунды
	 * \return вернет true, если заданное время корректно
	 */
	bool is_correct_date_time(
		const uint32_t day,
		const uint32_t month,
		const uint32_t year,
		const uint32_t hour = 0,
		const uint32_t minute = 0,
		const uint32_t second = 0,
		const uint32_t millisecond = 0);

	/** \brief Получить метку времени в начале года
	 *
	 * Данная функция обнуляет дни, месяцы, часы, минуты и секунды
	 * \param timestamp		Метка времени
	 * \return Метка времени начала года
	 */
	ZTIME_CONSTEXPR inline timestamp_t get_first_timestamp_year(const timestamp_t timestamp) noexcept {
		timestamp_t t = timestamp % SECONDS_IN_4_YEAR;
		if(t < SECONDS_IN_YEAR) return timestamp - t;
		else if(t < (2*SECONDS_IN_YEAR)) return timestamp + SECONDS_IN_YEAR - t;
		else if(t < (2*SECONDS_IN_YEAR + SECONDS_IN_LEAP_YEAR)) return timestamp + (2*SECONDS_IN_YEAR) - t;
		return timestamp + (2*SECONDS_IN_YEAR + SECONDS_IN_LEAP_YEAR) - t;
	}

	/** \brief Получить метку времени в конце года
	 *
	 * Данная функция находит последнюю метку времени текущего года
	 * \param timestamp		Метка времени
	 * \return Метка времени конца года
	 */
	ZTIME_CONSTEXPR inline timestamp_t get_last_timestamp_year(const timestamp_t timestamp = get_timestamp()) noexcept {
		timestamp_t t = timestamp % SECONDS_IN_4_YEAR;
		if(t < SECONDS_IN_YEAR) return timestamp + SECONDS_IN_YEAR - t - 1;
		else if(t < (2*SECONDS_IN_YEAR)) return timestamp + (2*SECONDS_IN_YEAR) - t - 1;
		else if(t < (2*SECONDS_IN_YEAR + SECONDS_IN_LEAP_YEAR)) return timestamp + (2*SECONDS_IN_YEAR + SECONDS_IN_LEAP_YEAR) - t - 1;
		return timestamp + (3*SECONDS_IN_YEAR + SECONDS_IN_LEAP_YEAR) - t - 1;
	}

	/** \brief Получить метку времени в начале дня
	 *
	 * Данная функция вернет метку времени в начале дня.
	 * Функция обнуляет часы, минуты и секунды
	 * \param timestamp		Метка времени
	 * \return Метка времени в начале дня
	 */
	constexpr inline timestamp_t get_first_timestamp_day(const timestamp_t timestamp = get_timestamp()) noexcept {
		return timestamp - (timestamp % SECONDS_IN_DAY);
	}

	/** \brief Получить метку времени в конце дня
	 * Данная функция устанавливает час 23, минута 59 и секунда 59
	 * \param timestamp		Метка времени
	 * \return Метка времени в конце дня
	 */
	constexpr inline timestamp_t get_last_timestamp_day(const timestamp_t timestamp = get_timestamp()) noexcept {
		return timestamp - (timestamp % SECONDS_IN_DAY) + SECONDS_IN_DAY - 1;
	}

	/** \brief Получить метку времени в начале часа
	 * Данная функция обнуляет минуты и секунды
	 * \param timestamp		Метка времени
	 * \return Метка времени в начале часа
	 */
	constexpr inline timestamp_t get_first_timestamp_hour(const timestamp_t timestamp = get_timestamp()) noexcept {
		return timestamp - (timestamp % SECONDS_IN_HOUR);
	}

	/** \brief Получить метку времени в конце часа
	 * Данная функция обнуляет минуты и секунды
	 * \param timestamp		Метка времени
	 * \return Метка времени в конце часа
	 */
	constexpr inline timestamp_t get_last_timestamp_hour(const timestamp_t timestamp = get_timestamp()) noexcept {
		return timestamp - (timestamp % SECONDS_IN_HOUR) + SECONDS_IN_HOUR - 1;
	}

	/** \brief Получить метку времени в начале минуты
	 * Данная функция обнуляет секунды
	 * \param timestamp		Метка времени
	 * \return Метка времени в начале минуты
	 */
	constexpr inline timestamp_t get_first_timestamp_minute(const timestamp_t timestamp = get_timestamp()) noexcept {
		return timestamp - (timestamp % SECONDS_IN_MINUTE);
	}

	/** \brief Получить метку времени в конце минуты
	 * Данная функция обнуляет секунды
	 * \param timestamp		Метка времени
	 * \return Метка времени в конце минуты
	 */
	constexpr inline timestamp_t get_last_timestamp_minute(const timestamp_t timestamp = get_timestamp()) noexcept {
		return timestamp - (timestamp % SECONDS_IN_MINUTE) + SECONDS_IN_MINUTE - 1;
	}

	/** \brief Получить метку времени в начале периода
	 *
	 * \param period Период
	 * \param timestamp Метка времени
	 * \return Метка времени в начале периода
	 */
	constexpr inline timestamp_t get_first_timestamp_period(const uint32_t period, const timestamp_t timestamp  = get_timestamp()) noexcept {
		return timestamp - (timestamp % period);
	}

	/** \brief Получить метку времени в конце периода
	 *
	 * \param period Период
	 * \param timestamp		Метка времени
	 * \return Метка времени в конце периода
	 */
	constexpr inline timestamp_t get_last_timestamp_period(const uint32_t period, const timestamp_t timestamp	 = get_timestamp()) noexcept {
		return timestamp - (timestamp % period) + period - 1;
	}

	/** \brief Проверить выходной день (суббота и воскресение)
	 * \param timestamp метка времени
	 * \return вернет true если выходной день
	 */
	ZTIME_CONSTEXPR inline bool is_day_off(const timestamp_t timestamp = get_timestamp()) noexcept {
		uint32_t wday = get_weekday(timestamp);
		if(wday == ztime::SUN || wday == ztime::SAT) return true;
		return false;
	}

	/** \brief Проверить день с начала отсчета Unix-времени на выходной день
	 * \param day день с начала отсчета Unix-времени
	 * \return вернет true если выходной день
	 */
	ZTIME_CONSTEXPR inline bool is_day_off_for_day(const uint32_t day) noexcept {
		uint32_t wday = (day + THU) % DAYS_IN_WEEK;
		if(wday == ztime::SUN || wday == ztime::SAT) return true;
		return false;
	}

	/** \brief Проверка високосного года
	 * \param year	Год
	 * \return вернет true, если год високосный
	 */
	constexpr inline bool is_leap_year(const uint32_t year) noexcept {
		return ((year & 3) == 0 && ((year % 25) != 0 || (year & 15) == 0));
	}

	/** \brief Проверить корректность дня месяца
	 * \param day	День
	 * \param month Месяц
	 * \param year	Год
	 * \return Вернет true, если день месяца правильный
	 */
	ZTIME_CONSTEXPR inline bool is_day_of_month(
			const uint32_t day,
			const uint32_t month,
			const uint32_t year) {
		if ((month < 1) || (12 < month)) return false;
		uint32_t days_of_month = 0;
		switch(month) {
		case 4:
		case 6:
		case 9:
		case 11:
			days_of_month = 30;
			break;
		case 2:
			days_of_month = 28;
			if (is_leap_year(year)) days_of_month = 29;
			break;
		default:
			days_of_month = 30;
			break;
		};
		return (0 < day) and (day <= days_of_month);
	};

	/** \brief Получить минуту
	 *
	 * Данная функция вернет минуту с начала UNIX
	 * \param timestamp Метка времени
	 * \return Минута с начала UNIX
	 */
	constexpr inline uint64_t get_minute(const timestamp_t timestamp = get_timestamp()) noexcept {
		return (uint64_t)(timestamp / SECONDS_IN_MINUTE);
	}

	/** \brief Получить минуту дня
	 *
	 * Данная функция вернет от 0 до 1439 (минуту дня)
	 * \param timestamp метка времени
	 * \return минута дня
	 */
	constexpr inline uint32_t get_minute_day(const timestamp_t timestamp = get_timestamp()) noexcept {
		return (uint32_t)((timestamp / SECONDS_IN_MINUTE) % MINUTES_IN_DAY);
	}

	/** \brief Получить минуту часа
	 *
	 * Данная функция вернет от 0 до 59
	 * \param timestamp метка времени
	 * \return Минута часа
	 */
	constexpr inline uint32_t get_minute_hour(const timestamp_t timestamp = get_timestamp()) noexcept {
		return (uint32_t)((timestamp / SECONDS_IN_MINUTE) % MINUTES_IN_HOUR);
	}

	/** \brief Получить час дня
	 * Данная функция вернет от 0 до 23 (час дня)
	 * \param timestamp метка времени
	 * \return час дня
	 */
	constexpr inline uint32_t get_hour_day(const timestamp_t timestamp = get_timestamp()) noexcept {
		return (uint32_t)((timestamp / SECONDS_IN_HOUR) % HOURS_IN_DAY);
	}

	/** \brief Получить секунду минуты
	 * Данная функция вернет от 0 до 59 (секунда минуты)
	 * \param timestamp метка времени
	 * \return секунда минуты
	 */
	constexpr inline uint32_t get_second_minute(const timestamp_t timestamp = get_timestamp()) noexcept {
		return (uint32_t)(timestamp % SECONDS_IN_MINUTE);
	}

	/** \brief Получить секунду часа
	 * Данная функция вернет от 0 до 3599 (секунда часа)
	 * \param timestamp метка времени
	 * \return секунда часа
	 */
	constexpr inline uint32_t get_second_hour(const timestamp_t timestamp = get_timestamp()) noexcept {
		return (uint32_t)(timestamp % SECONDS_IN_HOUR);
	}

	/** \brief Получить секунду дня
	 * Данная функция вернет от 0 до 86399 (секунда дня)
	 * \param timestamp метка времени
	 * \return Секунда дня
	 */
	constexpr inline uint32_t get_second_day(const timestamp_t timestamp = get_timestamp()) noexcept {
		return (uint32_t)(timestamp % SECONDS_IN_DAY);
	}

	/** \brief Получить секунду дня
	 * Данная функция вернет от 0 до 86399 (секунда дня)
	 * \param hour		Час дня
	 * \param minute	Минута часа
	 * \param second	Секунд
	 * \return Секунда дня
	 */
	constexpr inline uint32_t get_second_day(const uint32_t hour, const uint32_t minute, const uint32_t second) noexcept {
		return hour * SECONDS_IN_HOUR + minute * SECONDS_IN_MINUTE + second;
	}

	/** \brief Получить день
	 * Данная функция вернет день, начиная с начала UNIX времени
	 * \param timestamp метка времени
	 * \return день с начала UNIX времени
	 */
	constexpr inline uint32_t get_day(const timestamp_t timestamp = get_timestamp()) noexcept {
		return (uint32_t)(timestamp / SECONDS_IN_DAY);
	}

	/** \brief Получить метку времени начала года
	 * \param year год
	 * \return метка времени начала года
	 */
	ZTIME_CONSTEXPR inline timestamp_t get_timestamp_beg_year(const uint32_t year) noexcept {
		uint32_t diff = (year - FIRST_YEAR_UNIX);
		timestamp_t t = (diff / 4) * SECONDS_IN_4_YEAR;
		uint32_t temp = diff % 4;
		if(temp == 0) return t;
		else if(temp == 1) return t + SECONDS_IN_YEAR;
		else if(temp == 2) return t + (2*SECONDS_IN_YEAR);
		return t + (2*SECONDS_IN_YEAR + SECONDS_IN_LEAP_YEAR);
	}

	 /** \brief Получить метку времени дня UINX-времени
	 * \param unix_day день с начала UNIX-времени
	 * \return метка времени начала дня с момента начала UNIX-времени
	 */
	ZTIME_CONSTEXPR inline timestamp_t get_timestamp_day(const uint32_t unix_day) noexcept {
		return (timestamp_t)unix_day * SECONDS_IN_DAY;
	}

	/** \brief Получить год
	 * Данная функция вернет год указанной метки времени
	 * \param timestamp метка времени
	 * \return год UNIX времени
	 */
	ZTIME_CONSTEXPR inline uint32_t get_year(const timestamp_t timestamp = get_timestamp()) noexcept {
		uint32_t year = FIRST_YEAR_UNIX + 4 * (timestamp / SECONDS_IN_4_YEAR);
		timestamp_t t = timestamp % SECONDS_IN_4_YEAR;
		if(t < SECONDS_IN_YEAR) return year;
		else if(t < (2*SECONDS_IN_YEAR)) return year + 1;
		else if(t < (2*SECONDS_IN_YEAR + SECONDS_IN_LEAP_YEAR)) return year + 2;
		return year + 3;
	}

	/** \brief Получить день года
	 * \param timestamp метка времени
	 * \return день года
	 */
	inline uint32_t get_day_year(const timestamp_t timestamp = get_timestamp()) noexcept {
		uint32_t year = get_year(timestamp);
		return get_day(timestamp) - get_day(get_timestamp_beg_year(year)) + 1;
	}

	/** \brief Получить месяц года
	 * \param timestamp метка времени
	 * \return месяц года
	 */
	uint32_t get_month(const timestamp_t timestamp);

	/** \brief Получить метку времени в начале текущего месяца
	 * \param timestamp Метка времени
	 * \return Метка времени в начале текущего месяца
	 */
	inline timestamp_t get_first_timestamp_month(const timestamp_t timestamp = get_timestamp()) noexcept {
		uint32_t day = get_day_month(timestamp);
		timestamp_t timestamp_new = get_first_timestamp_day(timestamp);
		timestamp_new -= (day - 1) * SECONDS_IN_DAY;
		return timestamp_new;
	}

	/** \brief Получить последнюю метку времени текущего месяца
	 * \param timestamp Метка времени
	 * \return Последняя метка времени текущего месяца
	 */
	inline timestamp_t get_last_timestamp_month(const timestamp_t timestamp = get_timestamp()) noexcept {
		uint32_t days_month = get_num_days_month(timestamp);
		uint32_t day = get_day_month(timestamp);
		timestamp_t timestamp_new = get_last_timestamp_day(timestamp);
		uint32_t offset = days_month - day;
		timestamp_new += offset * SECONDS_IN_DAY;
		return timestamp_new;
	}

	/** \brief Получить последнюю метку времени последнего воскресения текущего месяца
	 * \param timestamp Метка времениm
	 * \return Последняя метка времени текущего последнего воскресения текущего месяца
	 */
	inline timestamp_t get_last_timestamp_sunday_month(const timestamp_t timestamp = get_timestamp()) noexcept {
		timestamp_t last_timestamp = get_last_timestamp_month(timestamp);
		uint32_t weekday = get_weekday(timestamp);
		last_timestamp -= weekday * SECONDS_IN_DAY;
		return last_timestamp;
	}

	/** \brief Получить количество дней в текущем году
	 * \param timestamp метка времени
	 * \return дней в текущем году
	 */
	inline uint32_t get_day_in_year(const timestamp_t timestamp = get_timestamp()) noexcept {
		if(is_leap_year(get_year(timestamp))) return DAYS_IN_LEAP_YEAR;
		return DAYS_IN_YEAR;
	}

	/** \brief Получить метку времени начала дня от начала unix-времени
	 * \param day день с начала unix-времени
	 * \return метка времени
	 */
	inline timestamp_t get_first_timestamp_for_day(const uint32_t day) noexcept {
		return day * SECONDS_IN_DAY;
	}

	/** \brief Получить метку времени начала предыдущего дня
	 * \param timestamp Метка времени текущего дня
	 * \return метка времени начала предыдущего дня
	 */
	inline timestamp_t get_first_timestamp_previous_day(const timestamp_t timestamp = get_timestamp()) noexcept {
		return get_first_timestamp_day(timestamp) - SECONDS_IN_DAY;
	}

	/** \brief Получить метку времени начала дня начала недели
	 *
	 * Данная функция найдет метку времемени начала дня начала недели.
	 * Это соответствует началу воскресенья.
	 * \param timestamp Метка времени
	 * \return вернет метку времени начала недели.
	 */
	inline timestamp_t get_week_start_first_timestamp(const timestamp_t timestamp = get_timestamp()) noexcept {
		return get_first_timestamp_day(timestamp) -
			(timestamp_t)get_weekday(timestamp) * SECONDS_IN_DAY;
	}

	/** \brief Получить метку времени начала дня конца недели
	 *
	 * Данная функция найдет метку времемени начала дня конца недели.
	 * Это соответствует началу дня субботы.
	 * \param timestamp Метка времени
	 * \return вернет метку времени начала недели.
	 */
	inline timestamp_t get_week_end_first_timestamp(const timestamp_t timestamp = get_timestamp()) noexcept {
		return get_first_timestamp_day(timestamp) +
			(timestamp_t)(SAT - get_weekday(timestamp)) * SECONDS_IN_DAY;
	}

	/** \brief Получить метку времени начала дня через указанное количество дней
	 *
	 * Данная функция полезна, если нужно найти метку времени начала дня через указанное количество дней.
	 * \param timestamp Текущая метка времени
	 * \param days Количество дней спустя
	 * \return вернет метку времени начала дня через указанное количество дней
	 */
	inline timestamp_t get_first_timestamp_next_day(
			const timestamp_t timestamp,
			const uint32_t days) noexcept {
		return get_first_timestamp_day(timestamp) +
			((timestamp_t)days * SECONDS_IN_DAY);
	}

	/** \brief Преобразовать метку времени в секундах в метку времени в миллисекундах
	 * \param timestamp Метка времени в секундах
	 * \return Метка времени в миллисекундах
	 */
	template<class T1, class T2>
	inline T1 to_timestamp_ms(const T2 timestamp) noexcept {
		return (T1)timestamp * (T1)MILLISECONDS_IN_SECOND;
	}

	/** \brief Преобразовать метку времени в миллисекундах в метку времени в секундах
	 * \param timestamp_ms	Метка времени в миллисекундах
	 * \return Метка времени в секундах
	 */
	template<class T1, class T2>
	inline T1 to_timestamp(const T2 timestamp_ms) noexcept {
		return (T1)timestamp_ms / (T1)MILLISECONDS_IN_SECOND;
	}

	/** \brief Задержка на указанное количество миллисекунд
	 * \param milliseconds количество миллисекунд
	 */
	void delay_ms(const uint64_t milliseconds);

	/** \brief Задержка на указанное количество секунд
	 * \param seconds количество секунд
	 */
	void delay(const uint64_t seconds);

	/** \brief Проверить пересечение периодов
	 * \param a Первый период
	 * \param b Второй период
	 * \return Вернет true, если есть пересечение периодов
	 */
	template<class PERIOD_TYPE>
	constexpr inline bool intersection_periods(
			const PERIOD_TYPE &a,
			const PERIOD_TYPE &b) noexcept {
		return ((a.start <= b.start && b.start <= a.stop) ||
			(a.start <= b.stop && b.stop <= a.stop));
	}

	/** \brief Проверить вложенность периодов
	 * \param test_period	Тестируемый период
	 * \param main_period	Основной период, который может включать в себя тестируемый период
	 * \return Вернет true, если тестируемый период умещается в основной перпиод
	 */
	template<class PERIOD_TYPE>
	ZTIME_CONSTEXPR inline bool internal_period(
			const PERIOD_TYPE &test_period,
			const PERIOD_TYPE &main_period) noexcept {
		return (test_period.start >= main_period.start &&
			test_period.stop <= main_period.stop);
	}

	/** \brief Отсортировать контейнер с периодами
	 *
	 * Контейнер должен содержать периоды типа period_t или fperiod_t
	 * \param periods Неотсортированный контейнер с периодами
	 */
	template<class PERIOD_CONTAINER_TYPE>
	ZTIME_CONSTEXPR inline void sort_periods(PERIOD_CONTAINER_TYPE &periods) noexcept {
		typedef typename PERIOD_CONTAINER_TYPE::value_type period_type;
		if (periods.size() <= 1) return;
		if (!std::is_sorted(
			periods.begin(),
			periods.end(),
			[](const period_type & a, const period_type & b) {
				return a.start < b.start;
			})) {
			std::sort(periods.begin(), periods.end(),
			[](const period_type & a, const period_type & b) {
				return a.start < b.start;
			});
		}
	}

	/** \brief Найти период в контейнере
	 *
	 * Данная функция вернет указатель на период в заранее отсортированном конейтенере с периодами, в котором находится указанная метка времени
	 * \param periods	Отсортированный контейнер с периодами
	 * \param timestamp Метка времени
	 * \return Указатель на элемент отсортированного контейнера с периодами или на конец контейнера, если период не найден
	 */
	template<class PERIOD_CONTAINER_TYPE, class TIMESTAMP_TYPE>
	ZTIME_CONSTEXPR inline typename PERIOD_CONTAINER_TYPE::iterator find_period(PERIOD_CONTAINER_TYPE &periods, const TIMESTAMP_TYPE timestamp) noexcept {
		typedef typename PERIOD_CONTAINER_TYPE::value_type period_type;
		if (periods.empty()) return periods.end();
		auto it = std::lower_bound(
				periods.begin(),
				periods.end(),
				timestamp,
				[](const period_type & l, const TIMESTAMP_TYPE timestamp) {
					return	l.start < timestamp;
				});
		if ((it == periods.begin() || it != periods.end()) && it->start == timestamp) return it;
		if (it != periods.begin()) {
			auto prev_it = std::prev(it, 1);
			if (prev_it->start <= timestamp && prev_it->stop >= timestamp) return prev_it;
		}
		return periods.end();
	}

	/** \brief Объединить пересекающиеся периоды
	 * \param periods	Отсортированный контейнер с периодами
	 * \return Вернет true, если есть объединенные периоды
	 */
	template<class PERIOD_CONTAINER_TYPE>
	inline bool merge_periods(PERIOD_CONTAINER_TYPE &periods) noexcept {
		typedef typename PERIOD_CONTAINER_TYPE::value_type period_type;
		size_t i = 0;
		bool changed = false;
		while(i < periods.size()) {
			size_t j = i + 1;
			while(j < periods.size()) {
				if (intersection_periods(periods[i], periods[j])) {
					/* периоды пересекаются, склеиваем их */
					period_type new_period(
						std::min(periods[i].start, periods[j].start),
						std::max(periods[i].stop, periods[j].stop));
					periods[i] = new_period;
					periods.erase(periods.begin() + j);
					changed = true;
				} else {
					++j;
				}
			} // while(j < all_periods.size())
			++i;
		} // while(i < all_periods.size())
		return changed;
	}

	/** \brief Объединить пересекающиеся периоды
	 * \param periods			Отсортированный контейнер с периодами
	 * \param insert_periods	Массив периодов, которые были добавлены
	 * \param remove_periods	Массив периодов, которые были удалены
	 * \return Вернет true, если есть объединенные периоды
	 */
	template<class PERIOD_CONTAINER_TYPE_1,
		class PERIOD_CONTAINER_TYPE_2,
		class PERIOD_CONTAINER_TYPE_3>
	inline bool merge_periods(
			PERIOD_CONTAINER_TYPE_1 &periods,
			PERIOD_CONTAINER_TYPE_2 &insert_periods,
			PERIOD_CONTAINER_TYPE_3 &remove_periods) noexcept {
		typedef typename PERIOD_CONTAINER_TYPE_1::value_type period_type;
		size_t i = 0;
		bool changed = false;
		while(i < periods.size()) {
			size_t j = i + 1;
			while(j < periods.size()) {
				if (intersection_periods(periods[i], periods[j])) {
					/* периоды пересекаются, склеиваем их */
					period_type new_period(
						std::min(periods[i].start, periods[j].start),
						std::max(periods[i].stop, periods[j].stop));
					remove_periods.insert(remove_periods.end(), periods[i]);
					remove_periods.insert(remove_periods.end(), periods[j]);
					insert_periods.insert(insert_periods.end(), new_period);
					periods[i] = new_period;
					periods.erase(periods.begin() + j);
					changed = true;
				} else {
					++j;
				}
			} // while(j < all_periods.size())
			++i;
		} // while(i < all_periods.size())
		return changed;
	}

	/** \brief Параметры фазы Луны
	 *
	 * Оригинальный алгоритм №1: https://github.com/solarissmoke/php-moon-phase/blob/master/Solaris/MoonPhase.php
	 * Оригинальный алгоритм №2: https://www.fourmilab.ch/moontoolw/
	 */
	class MoonPhase {
	private:
		// Astronomical constants. 1980 January 0.0
		const double epoch = 2444238.5;

		// Constants defining the Sun's apparent orbit
		const double elonge = 278.833540;		// Ecliptic longitude of the Sun at epoch 1980.0
		const double elongp = 282.596403;		// Ecliptic longitude of the Sun at perigee
		const double eccent = 0.016718;			// Eccentricity of Earth's orbit
		const double sunsmax = 1.495985e8;		// Semi-major axis of Earth's orbit, km
		const double sunangsiz = 0.533128;		// Sun's angular size, degrees, at semi-major axis distance

		// Elements of the Moon's orbit, epoch 1980.0
		const double mmlong = 64.975464;		// Moon's mean longitude at the epoch
		const double mmlongp = 349.383063;		// Mean longitude of the perigee at the epoch
		const double mlnode = 151.950429;		// Mean longitude of the node at the epoch
		const double minc = 5.145396;			// Inclination of the Moon's orbit
		const double mecc = 0.054900;			// Eccentricity of the Moon's orbit
		const double mangsiz = 0.5181;			// Moon's angular size at distance a from Earth
		const double msmax = 384401;			// Semi-major axis of Moon's orbit in km
		const double mparallax = 0.9507;		// Parallax at distance a from Earth
		const double synmonth = 29.53058868;	// Synodic month (new Moon to new Moon)

		inline double deg2rad (double degrees) noexcept {
			static const double pi_on_180 = 3.14159265358979323846d / 180.0d;
			return degrees * pi_on_180;
		}

		inline double rad2deg(double angle) noexcept {
			static const double coeff = 180.0d / 3.14159265358979323846d;
			return angle * coeff;
		}

		inline double fixangle(const double a) noexcept {
			return (a - 360.0d * std::floor(a / 360.0d));
		}

		inline double kepler(double m, const double ecc) noexcept {
			// 1E-6
			const double epsilon = 0.000001;
			double e = m = deg2rad(m);
			double delta;
			do {
				delta = e - ecc * std::sin(e) - m;
				e -= delta / (1 - ecc * std::cos(e));
			} while (std::abs(delta) > epsilon);
			return e;
		}
	public:

		double phase = 0;								// Phase (0 to 1)
		double illumination = 0;						// Illuminated fraction (0 to 1)
		double age = 0;									// Age of moon (days)
		double distance = 0;							// Distance (kilometres)
		double diameter = 0;							// Angular diameter (degrees)
		double age_in_degrees = 0;						//Age of the Moon in degrees
		double sundistance = 0;							// Distance to Sun (kilometres)
		double sundiameter = 0;							// Sun's angular diameter (degrees)

		std::array<double, 8> quarters = {0,0,0,0,0,0,0,0};

		void init(const ztime::ftimestamp_t timestamp) noexcept {
			const double date = get_julian_date(timestamp);

			// Calculation of the Sun's position
			const double Day = date - epoch;					// Date within epoch
			const double N = fixangle((360 / 365.2422) * Day);	// Mean anomaly of the Sun
			const double M = fixangle(N + elonge - elongp);		// Convert from perigee co-ordinates to epoch 1980.0
			double Ec = kepler(M, eccent);						// Solve equation of Kepler
			Ec = std::sqrt((1 + eccent) / (1 - eccent)) * std::tan(Ec / 2);
			Ec = 2 * rad2deg(std::atan(Ec));					// True anomaly
			const double Lambdasun = fixangle(Ec + elongp);		// Sun's geocentric ecliptic longitude

			const double F = ((1 + eccent * std::cos(deg2rad(Ec))) / (1 - eccent * eccent));	// Orbital distance factor
			const double SunDist = sunsmax / F;		// Distance to Sun in km
			const double SunAng = F * sunangsiz;	// Sun's angular size in degrees

			// Calculation of the Moon's position
			const double ml = fixangle(13.1763966 * Day + mmlong);			// Moon's mean longitude
			const double MM = fixangle(ml - 0.1114041 * Day - mmlongp);		// Moon's mean anomaly
			//const double MN = fixangle(mlnode - 0.0529539 * Day);			// Moon's ascending node mean longitude
			const double Ev = 1.2739 * std::sin(deg2rad(2 * (ml - Lambdasun) - MM));// Evection
			const double Ae = 0.1858 * std::sin(deg2rad(M));						// Annual equation
			const double A3 = 0.37 * std::sin(deg2rad(M));							// Correction term
			const double MmP = MM + Ev - Ae - A3;									// Corrected anomaly
			const double mEc = 6.2886 * std::sin(deg2rad(MmP));						// Correction for the equation of the centre
			const double A4 = 0.214 * std::sin(deg2rad(2 * MmP));					// Another correction term
			const double lP = ml + Ev + mEc - Ae + A4;								// Corrected longitude
			const double V = 0.6583 * std::sin(deg2rad(2 * (lP - Lambdasun)));		// Variation
			const double lPP = lP + V;												// True longitude
			//const double NP = MN - 0.16 * std::sin(deg2rad(M));						// Corrected longitude of the node
			//const double y = std::sin(deg2rad(lPP - NP)) * std::cos(deg2rad(minc));	// Y inclination coordinate
			//const double x = std::cos(deg2rad(lPP - NP));							// X inclination coordinate

			//const double Lambdamoon = rad2deg(std::atan2(y, x)) + NP;				// Ecliptic longitude
			//const double BetaM = rad2deg(std::asin(std::sin(deg2rad(lPP - NP)) * std::sin(deg2rad(minc)))); // Ecliptic latitude

			// Calculation of the phase of the Moon
			const double MoonAge = lPP - Lambdasun;						// Age of the Moon in degrees
			const double MoonPhase = (1 - std::cos(deg2rad(MoonAge))) / 2;	// Phase of the Moon

			// Distance of moon from the centre of the Earth
			const double MoonDist = (msmax * (1 - mecc * mecc)) / (1 + mecc * std::cos(deg2rad(MmP + mEc)));

			const double MoonDFrac = MoonDist / msmax;
			const double MoonAng = mangsiz / MoonDFrac;		// Moon's angular diameter
			// $MoonPar = $mparallax / $MoonDFrac;			// Moon's parallax

			// Store results
			phase = fixangle(MoonAge) / 360;				// Phase (0 to 1)
			illumination = MoonPhase;						// Illuminated fraction (0 to 1)
			age = synmonth * phase;							// Age of moon (days)
			distance = MoonDist;							// Distance (kilometres)
			diameter = MoonAng;								// Angular diameter (degrees)
			age_in_degrees = MoonAge;						//Age of the Moon in degrees
			sundistance = SunDist;							// Distance to Sun (kilometres)
			sundiameter = SunAng;							// Sun's angular diameter (degrees)
		}

		void init_only_phase(const ztime::ftimestamp_t timestamp) noexcept {
			const double date = get_julian_date(timestamp);

			// Calculation of the Sun's position
			const double Day = date - epoch;					// Date within epoch
			const double N = fixangle((360 / 365.2422) * Day);	// Mean anomaly of the Sun
			const double M = fixangle(N + elonge - elongp);		// Convert from perigee co-ordinates to epoch 1980.0
			double Ec = kepler(M, eccent);						// Solve equation of Kepler
			Ec = std::sqrt((1 + eccent) / (1 - eccent)) * std::tan(Ec / 2);
			Ec = 2 * rad2deg(std::atan(Ec));					// True anomaly
			const double Lambdasun = fixangle(Ec + elongp);		// Sun's geocentric ecliptic longitude

			//const double F = ((1 + eccent * std::cos(deg2rad(Ec))) / (1 - eccent * eccent));	  // Orbital distance factor
			//const double SunDist = sunsmax / F;		// Distance to Sun in km
			//const double SunAng = F * sunangsiz;	// Sun's angular size in degrees

			// Calculation of the Moon's position
			const double ml = fixangle(13.1763966 * Day + mmlong);			// Moon's mean longitude
			const double MM = fixangle(ml - 0.1114041 * Day - mmlongp);		// Moon's mean anomaly
			//const double MN = fixangle(mlnode - 0.0529539 * Day);			// Moon's ascending node mean longitude
			const double Ev = 1.2739 * std::sin(deg2rad(2 * (ml - Lambdasun) - MM));// Evection
			const double Ae = 0.1858 * std::sin(deg2rad(M));						// Annual equation
			const double A3 = 0.37 * std::sin(deg2rad(M));							// Correction term
			const double MmP = MM + Ev - Ae - A3;									// Corrected anomaly
			const double mEc = 6.2886 * std::sin(deg2rad(MmP));						// Correction for the equation of the centre
			const double A4 = 0.214 * std::sin(deg2rad(2 * MmP));					// Another correction term
			const double lP = ml + Ev + mEc - Ae + A4;								// Corrected longitude
			const double V = 0.6583 * std::sin(deg2rad(2 * (lP - Lambdasun)));		// Variation
			const double lPP = lP + V;												// True longitude
			//const double NP = MN - 0.16 * std::sin(deg2rad(M));						// Corrected longitude of the node
			//const double y = std::sin(deg2rad(lPP - NP)) * std::cos(deg2rad(minc));	// Y inclination coordinate
			//const double x = std::cos(deg2rad(lPP - NP));							// X inclination coordinate

			//const double Lambdamoon = rad2deg(std::atan2(y, x)) + NP;				// Ecliptic longitude
			//const double BetaM = rad2deg(std::asin(std::sin(deg2rad(lPP - NP)) * std::sin(deg2rad(minc)))); // Ecliptic latitude

			// Calculation of the phase of the Moon
			const double MoonAge = lPP - Lambdasun;						// Age of the Moon in degrees
			// Store results
			phase = fixangle(MoonAge) / 360;				// Phase (0 to 1)
		}

		MoonPhase() {};

		MoonPhase(const ztime::ftimestamp_t timestamp) {
			init(timestamp);
		};

		double meanphase(const ztime::ftimestamp_t date, double k) noexcept {
			// Time in Julian centuries from 1900 January 0.5
			const double jt = (date - 2415020.0) / 36525;
			const double t2 = jt * jt;
			const double t3 = t2 * jt;

			const double  nt1 = 2415020.75933 + synmonth * k
				+ 0.0001178 * t2
				- 0.000000155 * t3
				+ 0.00033 * std::sin(deg2rad(166.56 + 132.87 * jt - 0.009173 * t2));
			return nt1;
		}

		double truephase(double k, double phase) noexcept {
			bool apcor = false;
			k += phase;				// Add phase to new moon time
			const double t = k / 1236.85;	// Time in Julian centuries from 1900 January 0.5
			const double t2 = t * t;		// Square for frequent use
			const double t3 = t2 * t;		// Cube for frequent use
			double pt = 2415020.75933 // Mean time of phase
				+ synmonth * k
				+ 0.0001178 * t2
				- 0.000000155 * t3
				+ 0.00033 * std::sin(deg2rad(166.56 + 132.87 * t - 0.009173 * t2));

			const double m = 359.2242 + 29.10535608 * k - 0.0000333 * t2 - 0.00000347 * t3;			// Sun's mean anomaly
			const double mprime = 306.0253 + 385.81691806 * k + 0.0107306 * t2 + 0.00001236 * t3;	// Moon's mean anomaly
			const double f = 21.2964 + 390.67050646 * k - 0.0016528 * t2 - 0.00000239 * t3;			// Moon's argument of latitude

			if (phase < 0.01 || std::abs(phase - 0.5) < 0.01) {
				// Corrections for New and Full Moon
				pt += (0.1734 - 0.000393 * t) * std::sin(deg2rad(m))
					+ 0.0021 * std::sin(deg2rad(2 * m))
					- 0.4068 * std::sin(deg2rad(mprime))
					+ 0.0161 * std::sin(deg2rad(2 * mprime))
					- 0.0004 * std::sin(deg2rad(3 * mprime))
					+ 0.0104 * std::sin(deg2rad(2 * f))
					- 0.0051 * std::sin(deg2rad(m + mprime))
					- 0.0074 * std::sin(deg2rad(m - mprime))
					+ 0.0004 * std::sin(deg2rad(2 * f + m))
					- 0.0004 * std::sin(deg2rad(2 * f - m))
					- 0.0006 * std::sin(deg2rad(2 * f + mprime))
					+ 0.0010 * std::sin(deg2rad(2 * f - mprime))
					+ 0.0005 * std::sin(deg2rad(m + 2 * mprime));

				apcor = true;
			} else
			if (std::abs(phase - 0.25) < 0.01 || std::abs(phase - 0.75) < 0.01) {
				pt += (0.1721 - 0.0004 * t) * std::sin(deg2rad(m))
					+ 0.0021 * std::sin(deg2rad(2 * m))
					- 0.6280 * std::sin(deg2rad(mprime))
					+ 0.0089 * std::sin(deg2rad(2 * mprime))
					- 0.0004 * std::sin(deg2rad(3 * mprime))
					+ 0.0079 * std::sin(deg2rad(2 * f))
					- 0.0119 * std::sin(deg2rad(m + mprime))
					- 0.0047 * std::sin(deg2rad(m - mprime))
					+ 0.0003 * std::sin(deg2rad(2 * f + m))
					- 0.0004 * std::sin(deg2rad(2 * f - m))
					- 0.0006 * std::sin(deg2rad(2 * f + mprime))
					+ 0.0021 * std::sin(deg2rad(2 * f - mprime))
					+ 0.0003 * std::sin(deg2rad(m + 2 * mprime))
					+ 0.0004 * std::sin(deg2rad(m - 2 * mprime))
					- 0.0003 * std::sin(deg2rad(2 * m + mprime));

				// First and last quarter corrections
				if (phase < 0.5) {
					pt += 0.0028 - 0.0004 * std::cos(deg2rad(m)) + 0.0003 * std::cos(deg2rad(mprime));
				}
				else {
					pt += -0.0028 + 0.0004 * std::cos(deg2rad(m)) - 0.0003 * std::cos(deg2rad(mprime));
				}
				apcor = true;
			}
			return apcor ? pt : 0.0d;
		}


		void phasehunt(const ztime::timestamp_t timestamp) noexcept {
			const double sdate = get_julian_date(timestamp);
			double adate = sdate - 45;
			const double ats = timestamp - 86400 * 45;
			const double yy = get_year(ats);
			const double mm = get_month(ats);

			double k1 = std::floor((yy + ((mm - 1) * (1 / 12)) - 1900) * 12.3685);
			double k2;
			double nt1;
			adate = nt1 = meanphase(adate, k1);

			while (true) {
				adate += synmonth;
				k2 = k1 + 1;
				double nt2 = meanphase(adate, k2);

				// If nt2 is close to sdate, then mean phase isn't good enough, we have to be more accurate
				if (std::abs(nt2 - sdate) < 0.75) {
					nt2 = truephase(k2, 0.0);
				}

				if (nt1 <= sdate && nt2 > sdate){
					break;
				}

				nt1 = nt2;
				k1 = k2;
			}

			// Results in Julian dates
			double dates[8] = {
				truephase(k1, 0.0),
				truephase(k1, 0.25),
				truephase(k1, 0.5),
				truephase(k1, 0.75),
				truephase(k2, 0.0),
				truephase(k2, 0.25),
				truephase(k2, 0.5),
				truephase(k2, 0.75)
			};
			// Convert to UNIX time
			for (size_t i = 0; i < 8; ++i) {
				quarters[i] = (dates[i] - 2440587.5) * 86400;
			}
		}

		bool is_new_moon(const ztime::ftimestamp_t timestamp) noexcept {
			if (quarters[0] == 0 || timestamp > quarters[4] || timestamp < quarters[0]) {
				phasehunt(timestamp);
			}
			const uint64_t m = get_minute(timestamp);
			if (ztime::get_minute(quarters[0]) == m || get_minute(quarters[4]) == m) return true;
			return false;
		}

		/** \brief Проверить, является в данную минуту Луна Полной
		 * \param timestamp Метка времени
		 * \return Вернет true, если Луна полная
		 */
		bool is_full_moon(const ztime::ftimestamp_t timestamp) noexcept {
			if (quarters[2] == 0 || timestamp > quarters[6] || timestamp < quarters[2]) {
				phasehunt(timestamp);
			}
			const uint64_t m = get_minute(timestamp);
			if (ztime::get_minute(quarters[2]) == m || get_minute(quarters[6]) == m) return true;
			return false;
		}

		/** \brief Посчитать Фазу Луны, основываясь на датах новолуния
		 * \param timestamp Метка времени
		 * \return Вернет Фазу Луны
		 */
		const double calc_phase_v3(const ztime::ftimestamp_t timestamp) noexcept {
			if (quarters[0] == 0 || timestamp > quarters[4] || timestamp < quarters[0]) {
				phasehunt(timestamp);
			}
			return (timestamp - quarters[0]) / (quarters[4] - quarters[0]);
		}

		const uint32_t MAX_MOON_MINUTE = 42523; /**< Максимальное количество Лунных минут */

		/** \brief Получить Лунную минуту
		 * \param timestamp Метка времени
		 * \return Лунная минута (от 0 до 42523)
		 */
		const uint32_t get_moon_minute(const ztime::ftimestamp_t timestamp) noexcept {
			const uint32_t temp = calc_phase_v3(timestamp) * synmonth * 24 * 60;
			return temp > MAX_MOON_MINUTE ? MAX_MOON_MINUTE : temp;
		}
	};

	inline double get_moon_phase_v2(const ftimestamp_t timestamp) noexcept {
		MoonPhase moon_phase;
		moon_phase.init_only_phase(timestamp);
		return moon_phase.phase;
	}
};

#endif // ZTIME_HPP_INCLUDED
