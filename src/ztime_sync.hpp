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

#ifndef ZTIME_TIME_SYNC_HPP_INCLUDED
#define ZTIME_TIME_SYNC_HPP_INCLUDED

#include <curl/curl.h>
#include <ztime.hpp>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <cmath>
//#include <algorithm>

namespace ztime {

    /** \brief Класс для получения метки времени, синхронизированной с интернетом
     * Для синхронизации используется несколько замеров смещения времени ПК относительно
     * времени сервера. При этом учитывается пинг. Алгоритм считает, что половину времени
     * сигнал шел до сервера, это время учитывается при рассчете смещения времени.
     */
    class TimeSync {
        private:
        std::string sert_file = "curl-ca-bundle.crt";   /**< Файл сертификата */
        std::mutex sert_file_mutex;
        std::atomic<double> offset;                     /**< Сещение времени */
        std::atomic<double> accuracy;
        std::atomic<bool> is_sync;
        std::atomic<int> update_period;
        std::atomic<int> update_offset;
        /// Варианты состояния ошибок
        enum {
            ZTIME_OK = 0,           ///< Ошибки нет
            ZTIME_CURL_ERROR = -1,  ///< CURL не может быть инициализирован
            ZTIME_ERROR = -2,       ///< Ошибка парсера или иная ошибка
        };

        public:

        /** \brief Установить пользовательский сертификат
         * \param user_sert_file файл сертификата
         */
        void set_sert_file(std::string user_sert_file) {
            sert_file_mutex.lock();
            sert_file = user_sert_file;
            sert_file_mutex.unlock();
        }

        private:

        /** \brief Callback-функция для обработки ответа
         * Данная функция нужна для внутреннего использования
         */
        static int ztime_writer(char *data, size_t size, size_t nmemb, void *userdata) {
            int result = 0;
            std::string *buffer = (std::string*)userdata;
            if(buffer != NULL) {
                buffer->append(data, size * nmemb);
                result = size * nmemb;
            }
            return result;
        }

        /** \brief Разобрать сообщение на составляющие
         * \param response сообщение, которое надо распарсить
         * \param elemet_list список полученных элементов
         */
        void parse(std::string response, std::vector<std::string> &elemet_list) {
            if(response.back() != ' ')
                response += " ";
            std::size_t start_pos = 0;
            while(true) {
                std::size_t found_beg = response.find_first_of(" \n\t\r\t.", start_pos);
                if(found_beg != std::string::npos) {
                    std::size_t len = found_beg - start_pos;
                    if(len > 0)
                        elemet_list.push_back(response.substr(start_pos, len));
                    start_pos = found_beg + 1;
                } else break;
            }
        }

        int http_sync(
                ztime::ftimestamp_t &start_time,
                ztime::ftimestamp_t &stop_time,
                ztime::ftimestamp_t &server_time) {
            struct curl_slist *http_headers = NULL;
            http_headers = curl_slist_append(http_headers, "Host: time.is");
            http_headers = curl_slist_append(http_headers, "Accept: */*");
            http_headers = curl_slist_append(http_headers, "Accept-Language: ru-RU,ru;q=0.8,en-US;q=0.5,en;q=0.3");
            http_headers = curl_slist_append(http_headers, "Accept-Encoding: identity");
            http_headers = curl_slist_append(http_headers, "Connection: keep-alive");
            http_headers = curl_slist_append(http_headers, "Referer: https://time.is/UTC");

            ztime::timestamp_t t = (ztime::timestamp_t)((ztime::get_ftimestamp()) * 1000.0 + 0.5);
            std::string url = "https://time.is/t/?en.0.117.0.0p.0.a00." + std::to_string(t) + "." + std::to_string(t) + ".";
            std::string response;

            CURL *curl = curl_easy_init();
            if(!curl) return ZTIME_CURL_ERROR;
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
            sert_file_mutex.lock();
            curl_easy_setopt(curl, CURLOPT_CAINFO, sert_file.c_str());
            sert_file_mutex.unlock();
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ztime_writer);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            const int TIME_OUT = 1;
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, TIME_OUT); // выход через N сек
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, http_headers);

            start_time = ztime::get_ftimestamp();
            CURLcode result = curl_easy_perform(curl);
            stop_time = ztime::get_ftimestamp();

            curl_easy_cleanup(curl);
            curl_slist_free_all(http_headers);
            http_headers = NULL;

            if(result == CURLE_OK) {
                std::vector<std::string> elemet_list;
                parse(response, elemet_list);
                if(elemet_list.size() >= 1) {
                    server_time = ztime::get_ftimestamp(elemet_list[0]);
                } else {
                    return ZTIME_CURL_ERROR;
                }
                return ZTIME_OK;
            }
            return ZTIME_CURL_ERROR;
        }

        int sync(int samples = 20, int attempts = 10, int delay = 5) {
            if(samples < 1) samples = 1;
            if(attempts < 1) attempts = 1;
            if(delay < 1) delay = 1;
            ztime::ftimestamp_t sum_accuracy = 0;
            ztime::ftimestamp_t sum_diff_time = 0;
            std::vector<ztime::ftimestamp_t> array_diff_time;
            int sample = 0;
            int num_error = 0;
            while(true) {
                ztime::ftimestamp_t start_time = 0;
                ztime::ftimestamp_t stop_time = 0;
                ztime::ftimestamp_t server_time = 0;
                int err = ZTIME_OK;
                for(int attempt = 0; attempt < attempts; ++attempt) {
                    err = http_sync(start_time, stop_time, server_time);
                    if(err == ZTIME_OK) break;
                    std::chrono::seconds sec(delay);
                    std::this_thread::sleep_for(sec);
                }
                if(server_time == 0 || err != ZTIME_OK) {
                    num_error++;
                    if(num_error >= attempts) return ZTIME_ERROR;
                    continue;
                } else {
                    num_error = 0;
                }

                // находим общую задержку ответа от сервера
                ztime::ftimestamp_t delay_server = stop_time - start_time;
                /* находим задержку отправки сообщения на сервер
                 * (предполагаем что сигнал идет туда и обратно одинаковое время)
                 */
                ztime::ftimestamp_t delay_server_div2 = delay_server/2;
                // находим разницу во времени между сервером и временем ПК
                ztime::ftimestamp_t diff_time = (server_time - start_time + delay_server_div2);

                sum_diff_time += diff_time;
                sum_accuracy += delay_server_div2;
                array_diff_time.push_back(diff_time);
                sample++;
                if(sample >= samples) break;

                std::chrono::seconds sec(delay);
                std::this_thread::sleep_for(sec);
            }

            double aver = sum_accuracy/(double)samples;
            double sum = 0;
            for(int i = 0; i < samples; ++i) {
                ztime::ftimestamp_t temp = array_diff_time[i] - aver;
                sum += temp * temp;
            }
            ztime::ftimestamp_t std_dev = std::sqrt(sum) /(double)(samples - 1);
            accuracy = 3 * std_dev;
            //std::sort(array_diff_time.begin(), array_diff_time.end());
            offset = sum_diff_time/(double)samples;
            //std::cout << "offset " << offset << std::endl;
            //std::cout << "median " << array_diff_time[samples/2] << std::endl;
            //std::cout << "std_dev " << std_dev << std::endl;
            //std::cout << "accuracy " << accuracy << std::endl;
            return ZTIME_OK;
        }

    public:

        /** \brief Проверить флаг синхронизации времени
         * \return Вернет true если время синхронизировано
         */
        bool is_time_sync() {
            return is_sync;
        }

        /** \brief Получить точность синхронизации
         * \return Вернет точность синхронизации
         */
        double get_accuracy() {
            return accuracy;
        }

        /** \brief Получить метку времени компьютера
         * \return метка времен
         */
        ftimestamp_t get_ftimestamp() {
            return ztime::get_ftimestamp() + offset;
        }

        /** \brief Получить метку времени компьютера
         * \return метка времен
         */
        timestamp_t get_timestamp() {
            return (timestamp_t)(ztime::get_ftimestamp() + offset);
        }

        /** \brief Инициализировать класс для получения времени
         * \param samples количество измерений для настройки смещения времени
         * \param attempts количество попыток получить ответ от сервера
         * \param delay_attempts задержка между попытками получить ответ от сервера
         * \param delay_update время между повторными синхронизациями в секундах
         * \param offset_update смещение времени между повторными синхронизациями в секундах
         */
        TimeSync(
                const int samples = 50,
                const int attempts = 10,
                const int delay_attempts = 5,
                const int delay_update = ztime::SECONDS_IN_DAY,
                const int offset_update = ztime::SECONDS_IN_HOUR) {
            curl_global_init(CURL_GLOBAL_ALL);
            offset = 0;
            accuracy = 0;
            is_sync = false;
            update_period = delay_update;
            update_offset = offset_update;
            std::thread sync_thread = std::thread([&, samples, attempts, delay_attempts]() {
                while(true) {
                    is_sync = false;
                    int err = sync(samples, attempts, delay_attempts);
                    if(err == ZTIME_OK) {
                        is_sync = true;
                        timestamp_t real_utc = get_timestamp();
                        int delay_sleep = update_period - (real_utc % update_period) + update_offset;
                        std::chrono::seconds sec(delay_sleep);
                        std::this_thread::sleep_for(sec);
                    } else {
                        std::chrono::seconds sec(delay_attempts);
                        std::this_thread::sleep_for(sec);
                    }
                }
            });
            sync_thread.detach();
        };

        ~TimeSync() {};
    };
}

#endif // ZTIME_HIGH_ACCURACY_HPP_INCLUDED
