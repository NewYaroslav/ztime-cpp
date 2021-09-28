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

#include "ztime.hpp"

#include <ctime>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/timeb.h>

#include <thread>
#include <vector>
#include <algorithm>
#include <cctype>

namespace ztime {

    const timestamp_t get_timestamp() noexcept {
        // https://en.cppreference.com/w/c/chrono/timespec_get
        struct timespec ts;
#       if defined(CLOCK_REALTIME)
        clock_gettime(CLOCK_REALTIME, &ts); // Версия для POSIX
#       else
        timespec_get(&ts, TIME_UTC);
#       endif
        return ts.tv_sec;
    }

    const timestamp_t get_timestamp_ms() noexcept {
        struct timespec ts;
#       if defined(CLOCK_REALTIME)
        clock_gettime(CLOCK_REALTIME, &ts); // Версия для POSIX
#       else
        timespec_get(&ts, TIME_UTC);
#       endif
        return MILLISECONDS_IN_SECOND * ts.tv_sec + ts.tv_nsec / 1000000;
    }

    const timestamp_t get_timestamp_us() noexcept {
        struct timespec ts;
#       if defined(CLOCK_REALTIME)
        clock_gettime(CLOCK_REALTIME, &ts); // Версия для POSIX
#       else
        timespec_get(&ts, TIME_UTC);
#       endif
        return MICROSECONDS_IN_SECOND * ts.tv_sec + ts.tv_nsec / 1000;
    }

    const ftimestamp_t get_ftimestamp() noexcept {
        struct timespec ts;
#       if defined(CLOCK_REALTIME)
        clock_gettime(CLOCK_REALTIME, &ts); // Версия для POSIX
#       else
        timespec_get(&ts, TIME_UTC);
#       endif
        return (ftimestamp_t)ts.tv_sec + (ftimestamp_t)ts.tv_nsec / 1000000000.0d;
    }

    const uint32_t get_millisecond() noexcept {
        struct timespec ts;
#       if defined(CLOCK_REALTIME)
        clock_gettime(CLOCK_REALTIME, &ts); // Версия для POSIX
#       else
        timespec_get(&ts, TIME_UTC);
#       endif
        return ts.tv_nsec / 1000000;
    }

    const uint32_t get_microsecond() noexcept {
        struct timespec ts;
#       if defined(CLOCK_REALTIME)
        clock_gettime(CLOCK_REALTIME, &ts); // Версия для POSIX
#       else
        timespec_get(&ts, TIME_UTC);
#       endif
        return ts.tv_nsec / 1000;
    }

    const uint32_t get_nanosecond() noexcept {
        struct timespec ts;
#       if defined(CLOCK_REALTIME)
        clock_gettime(CLOCK_REALTIME, &ts); // Версия для POSIX
#       else
        timespec_get(&ts, TIME_UTC);
#       endif
        return ts.tv_nsec;
    }

    timestamp_t get_timestamp(std::string value) {
        const bool is_digit = std::all_of(value.begin(),value.end(),[](const char &c) {
            return isdigit(c);
        });

        if (is_digit) {
            auto new_end = std::remove_if(value.begin(), value.end(), [](const char& c){
                return !std::isdigit(c);
            });
            value.erase(new_end, value.end());
            uint32_t digit = (uint32_t)value.size() - 1;
            timestamp_t t = 0;
            const timestamp_t factors[] = {
                1,10,100,
                1000,10000,100000,
                1000000,10000000,100000000,
                1000000000,10000000000,100000000000,
                1000000000000,10000000000000,100000000000000,
                1000000000000000,10000000000000000,100000000000000000,
                1000000000000000000};
            uint32_t start = value.size() > 19 ? (uint32_t)value.size() - 19 : 0;
            for(uint32_t d = start; d <= digit; ++d) {
                t += ((timestamp_t)(value[d] - '0') * factors[digit - d]);
            }
            return t;
        } else {
            timestamp_t t = 0;
            if (!convert_str_to_timestamp(value, t)) return 0;
            return t;
        }
    }

    ftimestamp_t get_ftimestamp(const std::string &value) {
        timestamp_t temp = get_timestamp(value);
        timestamp_t t = temp / 1000;
        return (double)(temp - t*1000)/1000.0 + (double)t;
    }

    DateTime::DateTime() :
        year(1970),
        millisecond(0),
        microsecond(0),
        nanosecond(0),
        second(0),
        minute(0),
        hour(0),
        day(1),
        month(1) {
    };

    DateTime::DateTime(
            const uint32_t _day,
            const uint32_t _month,
            const uint32_t _year,
            const uint32_t _hour,
            const uint32_t _minute,
            const uint32_t _second,
            const uint32_t _millisecond) :
        year(_year),
        millisecond(_millisecond),
        second(_second),
        minute(_minute),
        hour(_hour),
        day(_day),
        month(_month) {
    }

    DateTime::DateTime(const timestamp_t timestamp) {
        set_timestamp(timestamp);
    }

    DateTime::DateTime(const ftimestamp_t ftimestamp) {
        set_ftimestamp(ftimestamp);
    }

    DateTime::DateTime(const std::string &str_iso_formatted_utc_datetime) {
        convert_iso(str_iso_formatted_utc_datetime, *this);
    }

    bool DateTime::is_correct() {
        return is_correct_date_time(day, month, year, hour, minute, second, millisecond);
    }

    timestamp_t DateTime::get_timestamp() {
        return ztime::get_timestamp(day, month, year, hour, minute, second);
    }

    ftimestamp_t DateTime::get_ftimestamp() {
        return ztime::get_ftimestamp(day, month, year, hour, minute, second, millisecond);
    }

    void DateTime::set_timestamp(const timestamp_t timestamp) {
        timestamp_t _secs;
        long _mon, _year;
        long long _days;
        long i;

        _secs = timestamp;
        const long _TBIAS_DAYS = 25567;
        _days = _TBIAS_DAYS;

        _days += _secs / SECONDS_IN_DAY; _secs = _secs % SECONDS_IN_DAY;
        hour = _secs / SECONDS_IN_HOUR; _secs %= SECONDS_IN_HOUR;
        minute = _secs / SECONDS_IN_MINUTE; second = _secs % SECONDS_IN_MINUTE;
        const long lmos[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
        const long mos[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

        for(_year = _days / DAYS_IN_YEAR;
            _days < (i = (((_year - 1) / 4) + ((((_year) & 03) || ((_year) == 0)) ? mos[0] : lmos[0]) + DAYS_IN_YEAR*_year));) {
            --_year;
        }
        _days -= i;
        const long _TBIAS_YEAR = 1900;
        year = _year + _TBIAS_YEAR;
        _mon = MONTHS_IN_YEAR;
        if(((_year) & 03) || ((_year) == 0)) {
            while(_days < mos[--_mon]) {};
            month = _mon + 1;
            day = _days - mos[_mon] + 1;
        } else {
            while(_days < lmos[--_mon]) {};
            month = _mon + 1;
            day = _days - lmos[_mon] + 1;
        }
    }

    void DateTime::set_ftimestamp(const ftimestamp_t ftimestamp) {
        const timestamp_t sec_timestamp = (timestamp_t)ftimestamp;
        set_timestamp(sec_timestamp);
        millisecond = (long)(((timestamp_t)(ftimestamp * 1000.0 + 0.5)) % 1000);
        microsecond = (long)(((timestamp_t)(ftimestamp * 1000000.0 + 0.5)) % 1000000);
        nanosecond = (long)(((timestamp_t)(ftimestamp * 1000000000.0 + 0.5)) % 1000000000);
    }

    void DateTime::print() {
        printf("%.2d.%.2d.%.4d %.2d:%.2d:%.2d\n",
            (uint32_t)day,
            (uint32_t)month,
            (uint32_t)year,
            (uint32_t)hour,
            (uint32_t)minute,
            (uint32_t)second);
    }

    std::string DateTime::get_str_date_time() {
        char text[24] = {};
        sprintf(text,"%.2d.%.2d.%.4d %.2d:%.2d:%.2d",
            (uint32_t)day,
            (uint32_t)month,
            (uint32_t)year,
            (uint32_t)hour,
            (uint32_t)minute,
            (uint32_t)second);
        return std::string(text);
    }

    std::string DateTime::get_str_date_time_ms() {
        char text[32] = {};
        sprintf(text,"%.2d.%.2d.%.4d %.2d:%.2d:%.2d.%.3d",
            (uint32_t)day,
            (uint32_t)month,
            (uint32_t)year,
            (uint32_t)hour,
            (uint32_t)minute,
            (uint32_t)second,
            (uint32_t)millisecond);
        return std::string(text);
    }

    std::string DateTime::get_str_date() {
        char text[16] = {};
        sprintf(text,"%.2d.%.2d.%.4d", (uint32_t)day, (uint32_t)month, (uint32_t)year);
        return std::string(text);
    }

    std::string DateTime::get_str_time(const bool is_use_seconds) {
        char text[16] = {};
        if(is_use_seconds) {
            sprintf(text,"%.2d:%.2d:%.2d",
                (uint32_t)hour,
                (uint32_t)minute,
                (uint32_t)second);
        } else {
            sprintf(text,"%.2d:%.2d",
                (uint32_t)hour,
                (uint32_t)minute);
        }
        return std::string(text);
    }

    std::string DateTime::get_str_time_ms() {
        char text[16] = {};
        sprintf(text,"%.2d:%.2d:%.2d.%.3d",
            (uint32_t)hour,
            (uint32_t)minute,
            (uint32_t)second,
            (uint32_t)millisecond);
        return std::string(text);
    }

    uint32_t DateTime::get_weekday() {
        return ztime::get_weekday(day, month, year);
    }

    bool DateTime::is_leap_year() {
        return ztime::is_leap_year(year);
    }

    uint32_t DateTime::get_num_days_current_month() {
        return get_num_days_month(month, year);
    }

    void DateTime::set_end_month() {
        set_end_day();
        day = get_num_days_month(month, year);
    }

    oadate_t DateTime::get_oadate() {
        return ztime::get_oadate(
            day,
            month,
            year,
            hour,
            minute,
            second,
            millisecond);
    }

    void DateTime::set_oadate(const oadate_t oadate) {
        set_ftimestamp(ztime::convert_oadate_to_ftimestamp(oadate));
    }

    bool convert_iso(const std::string &str_datetime, DateTime& t) {
        const std::string &word = str_datetime;
        if(word.size() >= 20) {
            // находим дату и время, пример 2020-10-12T14:48:46.618757Z
            t.year = std::atoi(word.substr(0, 4).c_str());
            t.month = std::atoi(word.substr(5, 2).c_str());
            t.day = std::atoi(word.substr(8, 2).c_str());
            t.hour = std::atoi(word.substr(11, 2).c_str());
            t.minute = std::atoi(word.substr(14, 2).c_str());
            t.second = std::atoi(word.substr(17, 2).c_str());

            const char str_end = (word.substr(19, 1))[0];

            if (str_end == '+' || str_end == '-') {
                // 2013-12-06T15:23:01+00:00
                std::string str_offset = word.substr(20, 2);
                const int gh = std::atoi(word.substr(20, 2).c_str());
                const int gm = std::atoi(word.substr(23, 2).c_str());
                const int offset = gh * 3600 + gm * 60;
                timestamp_t timestamp = t.get_timestamp();
                if(str_end == '+') timestamp += offset;
                else if(str_end == '-') timestamp -= offset;
                t.set_timestamp(timestamp);
            } else
            if (str_end == '.') {
                if(word.size() >= 26) {
                    // 2020-10-12T14:48:46.618757Z
                    // 2020-12-09T17:14:16.117642
                    int ms = std::atoi(word.substr(20, 6).c_str());
                    t.millisecond = ms / 1000;
                } else
                if(word.size() >= 23) {
                    // 2017-10-19T15:45:44.941Z
                    t.millisecond = std::atoi(word.substr(20, 3).c_str());
                }
            } else
            if(str_end == 'Z' && word.size() >= 20) {
                // 2020-10-12T14:48:46Z
                t.millisecond = 0;
            } else {
                return false;
            }
            return true;
        }
        return false;
    }

    ztime::ftimestamp_t convert_iso_to_ftimestamp(const std::string &str_datetime) {
        DateTime t;
        const size_t str_size = str_datetime.size();
        if(str_size >= 20) {
            // находим дату и время, пример 2020-10-12T14:48:46.618757Z
            t.year = std::stoi(str_datetime.substr(0, 4));
            t.month = std::stoi(str_datetime.substr(5, 2));
            t.day = std::stoi(str_datetime.substr(8, 2));
            t.hour = std::stoi(str_datetime.substr(11, 2));
            t.minute = std::stoi(str_datetime.substr(14, 2));
            t.second = std::stoi(str_datetime.substr(17, 2));

            const char str_end = (str_datetime.substr(19, 1))[0];

            if (str_end == '+' || str_end == '-') {
                // 2013-12-06T15:23:01+00:00
                const int gh = std::stoi(str_datetime.substr(20, 2));
                const int gm = std::stoi(str_datetime.substr(23, 2));
                const int offset = gh * 3600 + gm * 60;
                timestamp_t timestamp = t.get_timestamp();
                if(str_end == '+') timestamp += offset;
                else if(str_end == '-') timestamp -= offset;
                t.set_timestamp(timestamp);
            } else
            if (str_end == '.') {
                if(str_size >= 26) {
                    // 2020-10-12T14:48:46.618757Z
                    // 2020-12-09T17:14:16.117642
                    const int ms = std::stoi(str_datetime.substr(20, 6));
                    t.millisecond = ms / 1000;
                } else
                if(str_size >= 23) {
                    // 2017-10-19T15:45:44.941Z
                    t.millisecond = std::stoi(str_datetime.substr(20, 3));
                }
            } else
            if(str_end == 'Z' && str_size >= 20) {
                // 2020-10-12T14:48:46Z
                t.millisecond = 0;
            } else {
                return 0;
            }
            return t.get_ftimestamp();
        }
        return 0;
    }

    uint32_t get_month(std::string month) {
        if(month.size() == 0) return 0;
        std::transform(month.begin(), month.end(), month.begin(), tolower);
        month[0] = toupper(month[0]);
        for(uint32_t i = 0; i < MONTHS_IN_YEAR; ++i) {
            std::string name_long = month_name_long[i];
            std::string name_short = month_name_short[i];
            if(month == name_long) return i + 1;
            if(month == name_short) return i + 1;
        }
        return 0;
    }

    bool convert_str_to_timestamp(std::string str, timestamp_t& t) {
        uint32_t day = 0, month = 0, year = 0, hour = 0, minute = 0, second = 0;
        str += "_";
        std::vector<std::string> output_list;
        std::size_t start_pos = 0;

        while(true) {
            std::size_t found_beg = str.find_first_of("/\\_:-., ", start_pos);
            if(found_beg != std::string::npos) {
                std::size_t len = found_beg - start_pos;
                if(len > 0)
                    output_list.push_back(str.substr(start_pos, len));
                start_pos = found_beg + 1;
            } else break;
        }

        if(output_list.size() >= 3) {
            if(output_list[0].size() >= 4) {
                // если год в самом начале
                year = std::atoi(output_list[0].c_str());
                month = std::atoi(output_list[1].c_str());
                day = std::atoi(output_list[2].c_str());
                if(output_list.size() == 6) {
                    hour = std::atoi(output_list[3].c_str());
                    minute = std::atoi(output_list[4].c_str());
                    second = std::atoi(output_list[5].c_str());
                }
            } else
            if(output_list[2].size() >= 4) {
                // если год в конце
                day = std::atoi(output_list[0].c_str());
                month = std::atoi(output_list[1].c_str());
                year = std::atoi(output_list[2].c_str());
                if(output_list.size() == 6) {
                    hour = std::atoi(output_list[3].c_str());
                    minute = std::atoi(output_list[4].c_str());
                    second = std::atoi(output_list[5].c_str());
                }
            } else
            if(output_list[2].size() == 2 && output_list.size() == 6) {
                hour = std::atoi(output_list[0].c_str());
                minute = std::atoi(output_list[1].c_str());
                second = std::atoi(output_list[2].c_str());
                if(output_list[5].size() >= 4 && output_list[4].size() == 2) {
                    day = std::atoi(output_list[3].c_str());
                    month = std::atoi(output_list[4].c_str());
                    year = std::atoi(output_list[5].c_str());
                } else
                if(output_list[5].size() == 2 && output_list[4].size() >= 3) {
                    day = std::atoi(output_list[3].c_str());
                    month = get_month(output_list[4]);
                    year = std::atoi(output_list[5].c_str()) + 2000;
                } else
                if(output_list[5].size() == 4 && output_list[4].size() >= 3) {
                    day = std::atoi(output_list[3].c_str());
                    month = get_month(output_list[4]);
                    year = std::atoi(output_list[5].c_str());
                } else
                if(output_list[5].size() == 2 && output_list[4].size() == 2) {
                    day = std::atoi(output_list[3].c_str());
                    month = std::atoi(output_list[4].c_str());
                    year = std::atoi(output_list[5].c_str()) + 2000;
                } else {
                    return false;
                }
            }
        } else {
            return false;
        }
        if(day >= 32 || day <= 0 || minute >= 60 ||
            second >= 60 || hour >= 24 ||
            year < 1970 || month > 12 || month <= 0) {
            return false;
        }

        t = get_timestamp(day, month, year, hour, minute, second);
        return true;
    }

    ztime::timestamp_t to_timestamp(std::string str_datetime) {
        int day = 0, month = 0, year = 0, hour = 0, minute = 0, second = 0;
        str_datetime += "_";
        std::vector<std::string> arguments;
        std::size_t start_pos = 0;

        while(true) {
            std::size_t found_beg = str_datetime.find_first_of("/\\_:-., ", start_pos);
            if(found_beg != std::string::npos) {
                std::size_t len = found_beg - start_pos;
                if(len > 0)
                    arguments.push_back(str_datetime.substr(start_pos, len));
                start_pos = found_beg + 1;
            } else break;
        }

        if(arguments.size() >= 3) {
            // если аргументов больше или 3

            if(arguments[0].size() >= 4) {
                // если год в самом начале

                year = std::stoi(arguments[0]);
                month = std::stoi(arguments[1]);
                day = std::stoi(arguments[2]);

                if(arguments.size() == 6) {
                    hour = std::stoi(arguments[3]);
                    minute = std::stoi(arguments[4]);
                    second = std::stoi(arguments[5]);
                } else
                if(arguments.size() == 5) {
                    hour = std::stoi(arguments[3]);
                    minute = std::stoi(arguments[4]);
                }
            } else
            if(arguments[2].size() >= 4) {
                // если год в конце

                day = std::stoi(arguments[0]);
                month = std::stoi(arguments[1]);
                year = std::stoi(arguments[2]);

                if(arguments.size() == 6) {
                    hour = std::stoi(arguments[3]);
                    minute = std::stoi(arguments[4]);
                    second = std::stoi(arguments[5]);
                } else
                if(arguments.size() == 5) {
                    hour = std::stoi(arguments[3]);
                    minute = std::stoi(arguments[4]);
                }
            } else
            if (arguments.size() >= 3 &&
                arguments[0].size() == arguments[1].size() &&
                arguments[1].size() == arguments[2].size() &&
                arguments[2].size() == 2) {
                // если сначала идет время, а потом дата

                hour = std::stoi(arguments[0]);
                minute = std::stoi(arguments[1]);
                second = std::stoi(arguments[2]);

                if (arguments.size() >= 6) {
                    if (arguments[5].size() >= 4 && arguments[4].size() == 2) {
                        day = std::stoi(arguments[3]);
                        month = std::stoi(arguments[4]);
                        year = std::stoi(arguments[5]);
                    } else
                    if (arguments[5].size() == 2 && arguments[4].size() >= 3) {
                        day = std::stoi(arguments[3]);
                        month = get_month(arguments[4]);
                        year = std::stoi(arguments[5]) + 2000;
                    } else
                    if (arguments[5].size() == 4 && arguments[4].size() >= 3) {
                        day = std::stoi(arguments[3]);
                        month = get_month(arguments[4]);
                        year = std::stoi(arguments[5]);
                    } else
                    if (arguments[5].size() == 2 && arguments[4].size() == 2) {
                        day = std::stoi(arguments[3]);
                        month = std::stoi(arguments[4]);
                        year = std::stoi(arguments[5]) + 2000;
                    } else {
                        return 0;
                    }
                }
            }
        } else {
            return 0;
        }
        if (day >= 32 || day <= 0 || minute >= 60 ||
            second >= 60 || hour >= 24 ||
            year < 1970 || month > 12 || month <= 0) {
            return 0;
        }
        return get_timestamp(day, month, year, hour, minute, second);
    }

    int to_second_day(std::string str_time) {
        int hour = 0, minute = 0, second = 0;
        str_time += "_";
        std::vector<std::string> arguments;
        std::size_t start_pos = 0;

        while(true) {
            std::size_t found_beg = str_time.find_first_of("/\\_:-., ", start_pos);
            if(found_beg != std::string::npos) {
                std::size_t len = found_beg - start_pos;
                if(len > 0)
                    arguments.push_back(str_time.substr(start_pos, len));
                start_pos = found_beg + 1;
            } else break;
        }

        if(arguments.size() == 1) {
            hour = std::stoi(arguments[0]);
        } else
        if(arguments.size() == 2) {
            hour = std::stoi(arguments[0]);
            minute = std::stoi(arguments[1]);
        } else
        if(arguments.size() == 3) {
            hour = std::stoi(arguments[0]);
            minute = std::stoi(arguments[1]);
            second = std::stoi(arguments[2]);
        }
        if (hour >= 24 || hour < 0 ||
            minute >= 60 || minute < 0 ||
            second >= 60 || second < 0) {
            return -1;
        }
        return (int)(hour * ztime::SECONDS_IN_HOUR + minute * ztime::SECONDS_IN_MINUTE + second);
    }

    DateTime convert_timestamp_to_datetime(const timestamp_t timestamp) {
        DateTime outTime;
        timestamp_t _secs;
        long _mon, _year;
        long long _days;
        long i;

        _secs = timestamp;
        const long _TBIAS_DAYS = 25567;
        _days = _TBIAS_DAYS;

        _days += _secs / SECONDS_IN_DAY; _secs = _secs % SECONDS_IN_DAY;
        outTime.hour = _secs / SECONDS_IN_HOUR; _secs %= SECONDS_IN_HOUR;
        outTime.minute = _secs / SECONDS_IN_MINUTE;
        outTime.second = _secs % SECONDS_IN_MINUTE;
        const long	lmos[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
        const long	mos[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

        for (_year = _days / DAYS_IN_YEAR; _days < (i = (((_year - 1) / 4) + ((((_year) & 03) || ((_year) == 0)) ? mos[0] : lmos[0]) + DAYS_IN_YEAR*_year)); ) { --_year; }
        _days -= i;
        const long _TBIAS_YEAR = 1900;
        outTime.year = _year + _TBIAS_YEAR;

        if(((_year) & 03) || ((_year) == 0)) {
            // mos
            for(_mon = MONTHS_IN_YEAR; _days < mos[--_mon];);
            outTime.month = _mon + 1;
            outTime.day = _days - mos[_mon] + 1;
        } else {
            for(_mon = MONTHS_IN_YEAR; _days < lmos[--_mon];);
            outTime.month = _mon + 1;
            outTime.day = _days - lmos[_mon] + 1;
        }
        return outTime;
    }

    uint32_t get_weekday(const uint32_t day, const uint32_t month, const uint32_t year) {
        uint32_t a, y, m, R;
        a = ( 14 - month ) / 12;
        y = year - a;
        m = month + 12 * a - 2;
        R = 7000 + ( day + y + y / 4 - y / 100 + y / 400 + (31 * m) / 12 );
        return R % 7;
    }

    void print_date_time(const timestamp_t &timestamp) {
        DateTime t(timestamp);
        t.print();
    }

    uint32_t get_num_days_month(const uint32_t month, const uint32_t year) {
        if(month > MONTHS_IN_YEAR)
            return 0;
        const uint32_t num_days[13] = {0,31,30,31,30,31,30,31,31,30,31,30,31};
        if(month == FEB) {
            if(is_leap_year(year)) return 29;
            else return 28;
        } else {
            return num_days[month];
        }
    }

    uint32_t get_num_days_month(const timestamp_t timestamp) {
        const uint32_t num_days[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
        const uint32_t month = get_month(timestamp);
        if(month == FEB) {
            if(is_leap_year(get_year(timestamp))) return 29;
            else return 28;
        } else {
            return num_days[month];
        }
    }

    timestamp_t convert_gmt_to_cet(const timestamp_t gmt) {
        const timestamp_t ONE_HOUR = SECONDS_IN_HOUR;
        const uint8_t OLD_START_SUMMER_HOUR = 2;
        const uint8_t OLD_STOP_SUMMER_HOUR = 3;
        const uint8_t NEW_SUMMER_HOUR = 1;
        const uint8_t MONTH_MARSH = MAR;
        const uint8_t MONTH_OCTOBER = OCT;
        DateTime iTime(gmt);
        uint32_t maxDays = iTime.get_num_days_current_month();
        if(iTime.year < 2002) {
            // До 2002 года в Европе переход на летнее время осуществлялся в последнее воскресенье марта в 2:00 переводом часов на 1 час вперёд
            // а обратный переход осуществлялся в последнее воскресенье октября в 3:00 переводом на 1 час назад
            if(iTime.month > MONTH_MARSH && iTime.month < MONTH_OCTOBER) { // летнее время
                return gmt + ONE_HOUR * 2;
            } else
            if(iTime.month == MONTH_MARSH) {
                for(uint32_t d = maxDays; d >= iTime.day; d--) {
                    uint32_t _wday = get_weekday(d, MONTH_MARSH, iTime.year);
                    if(_wday == SUN) {
                        if(d == iTime.day) { // если сейчас воскресенье
                            if((iTime.hour + 1) >= OLD_START_SUMMER_HOUR) return gmt + ONE_HOUR * 2; // летнее время
                            return gmt + ONE_HOUR; // зимнее время
                        }
                        return gmt + ONE_HOUR; // зимнее время
                    }
                }
                return gmt + ONE_HOUR * 2; // летнее время
            } else
            if(iTime.month == MONTH_OCTOBER) {
                for(uint32_t d = maxDays; d >= iTime.day; d--) {
                    uint32_t _wday = get_weekday(d, MONTH_OCTOBER, iTime.year);
                    if(_wday == SUN) {
                        if(d == iTime.day) { // если сейчас воскресенье
                            if(iTime.hour + 2 >= OLD_STOP_SUMMER_HOUR) return gmt + ONE_HOUR; // зимнее время
                            return gmt + ONE_HOUR; // зимнее время
                        }
                        return gmt + ONE_HOUR * 2; // летнее время
                    }
                }
                return gmt + ONE_HOUR; // зимнее время
            }
            return gmt + ONE_HOUR; // зимнее время
        } else {
            // Начиная с 2002 года, согласно директиве ЕС(2000/84/EC) в Европе переход на летнее время осуществляется в 01:00 по Гринвичу.
            if(iTime.month > MONTH_MARSH && iTime.month < MONTH_OCTOBER) { // летнее время
                    return gmt + ONE_HOUR * 2;
            } else
            if(iTime.month == MONTH_MARSH) {
                for(uint32_t d = maxDays; d >= iTime.day; d--) {
                    uint32_t _wday = get_weekday(d, MONTH_MARSH, iTime.year);
                    if(_wday == SUN) {
                        if(d == iTime.day) { // если сейчас воскресенье
                            if(iTime.hour >= NEW_SUMMER_HOUR)
                                return gmt + ONE_HOUR * 2; // летнее время
                            return gmt + ONE_HOUR; // зимнее время
                        }
                        return gmt + ONE_HOUR; // зимнее время
                    }
                }
                return gmt + ONE_HOUR * 2; // летнее время
            } else
            if(iTime.month == MONTH_OCTOBER) {
                for(uint32_t d = maxDays; d >= iTime.day; d--) {
                    uint32_t _wday = get_weekday(d, MONTH_OCTOBER, iTime.year);
                    if(_wday == SUN) {
                        if(d == iTime.day) { // если сейчас воскресенье
                            if(iTime.hour >= NEW_SUMMER_HOUR)
                                return gmt + ONE_HOUR; // зимнее время
                            return gmt + ONE_HOUR * 2; // летнее время
                        }
                        return gmt + ONE_HOUR * 2; // летнее время
                    }
                }
                return gmt + ONE_HOUR; // зимнее время
            }
            return gmt + ONE_HOUR; // зимнее время
        }
        return gmt + ONE_HOUR; // зимнее время
    }

    timestamp_t convert_gmt_to_eet(const timestamp_t gmt) {
        return convert_gmt_to_cet(gmt) + SECONDS_IN_HOUR;
    }

    timestamp_t convert_gmt_to_msk(const timestamp_t gmt) {
        const timestamp_t gmt2 = gmt + 2 * SECONDS_IN_HOUR;
        const timestamp_t gmt3 = gmt + 3 * SECONDS_IN_HOUR;
        const timestamp_t gmt4 = gmt + 4 * SECONDS_IN_HOUR;

        const timestamp_t OCT_26_2014 = 1414281600;
        const timestamp_t MAR_27_2011 = 1301184000;
        const timestamp_t JAN_19_1992 = 695782800;
        const timestamp_t YEAR_1991 = 662688000;
        const timestamp_t OFFSET_HOUR1 = 23*SECONDS_IN_HOUR + 1;
        const timestamp_t OFFSET_HOUR2 = 22*SECONDS_IN_HOUR + 1;

        //int year4 = get_year(gmt4);
        //int month4 = get_month_year(gmt4);
        //int dayweek4 = get_weekday(gmt4);
        //int year3 = get_year(gmt4);
        //int month3 = get_month_year(gmt4);
        //int dayweek3 = get_weekday(gmt4);

        if(gmt4 >= OCT_26_2014) return gmt3;
        else
        if(gmt3 >= MAR_27_2011) return gmt4;
        else
        if(gmt2 >= JAN_19_1992) {
            timestamp_t last_timestamp_sunday2 = get_last_timestamp_sunday_month(gmt2);
            uint32_t month2 = get_month(gmt2);
            //uint32_t month3 = get_month_year(gmt3);
            uint32_t month4 = get_month(gmt4);
            if(month2 == MAR) {
                if(gmt2 < last_timestamp_sunday2) return gmt3;
                else return gmt4;
            } else
            if(month2 < MAR) return gmt3;
            else
            if(month4 == SEPT) {
                timestamp_t last_timestamp_sunday4 = get_last_timestamp_sunday_month(gmt4);
                if(gmt4 < (last_timestamp_sunday4 - OFFSET_HOUR1)) return gmt3;
                else return gmt4;
            } else
            if(month4 >= SEPT) return gmt3;
            else return gmt4;
        } else
        if(gmt3 >= YEAR_1991) {
            //timestamp_t last_timestamp_sunday2 = get_last_timestamp_sunday_month(gmt2);
            uint32_t month3 = get_month(gmt3);
            if(month3 == SEPT) {
                timestamp_t last_timestamp_sunday3 = get_last_timestamp_sunday_month(gmt3);
                if(gmt3 < (last_timestamp_sunday3 - OFFSET_HOUR2)) return gmt3;
                else return gmt4;
            } else
            if(month3 >= SEPT) return gmt2;
            else return gmt3;
        } else {
            timestamp_t last_timestamp_sunday3 = get_last_timestamp_sunday_month(gmt3);
            uint32_t month2 = get_month(gmt2);
            uint32_t month3 = get_month(gmt3);
            uint32_t month4 = get_month(gmt4);
            if(month3 == MAR) {
                if(gmt3 < last_timestamp_sunday3) return gmt3;
                else return gmt4;
            } else
            if(month2 < MAR) return gmt3;
            else
            if(month4 == SEPT) {
                timestamp_t last_timestamp_sunday4 = get_last_timestamp_sunday_month(gmt4);
                if(gmt4 < (last_timestamp_sunday4 - OFFSET_HOUR1)) return gmt3;
                else return gmt4;
            } else
            if(month4 >= SEPT) return gmt3;
            else return gmt4;
        }
    }

    timestamp_t convert_cet_to_gmt(const timestamp_t cet) {
        const timestamp_t ONE_HOUR = SECONDS_IN_HOUR;
        const uint32_t OLD_START_SUMMER_HOUR = 2;
        const uint32_t OLD_STOP_SUMMER_HOUR = 3;
        const uint32_t NEW_SUMMER_HOUR = 1;
        const uint32_t MONTH_MARSH = MAR;
        const uint32_t MONTH_OCTOBER = OCT;
        DateTime iTime(cet);
        uint32_t maxDays = iTime.get_num_days_current_month();

        if(iTime.year < 2002) {
            // До 2002 года в Европе переход на летнее время осуществлялся в последнее воскресенье марта в 2:00 переводом часов на 1 час вперёд
            // а обратный переход осуществлялся в последнее воскресенье октября в 3:00 переводом на 1 час назад
            if(iTime.month > MONTH_MARSH && iTime.month < MONTH_OCTOBER) { // летнее время
                return cet - ONE_HOUR * 2;
            } else
            if(iTime.month == MONTH_MARSH) {
                for(uint32_t d = maxDays; d >= iTime.day; d--) {
                    uint32_t _wday = get_weekday(d, MONTH_MARSH, iTime.year);
                    if(_wday == SUN) {
                        if(d == iTime.day) { // если сейчас воскресенье
                            if(iTime.hour >= OLD_START_SUMMER_HOUR) return cet - ONE_HOUR * 2; // летнее время
                            return cet - ONE_HOUR; // зимнее время
                        }
                        return cet - ONE_HOUR; // зимнее время
                    }
                }
                return cet - ONE_HOUR * 2; // летнее время
            } else
            if(iTime.month == MONTH_OCTOBER) {
                for(uint32_t d = maxDays; d >= iTime.day; d--) {
                    uint32_t _wday = get_weekday(d, MONTH_OCTOBER, iTime.year);
                    if(_wday == SUN) {
                        if(d == iTime.day) { // если сейчас воскресенье
                            if(iTime.hour >= OLD_STOP_SUMMER_HOUR) return cet - ONE_HOUR; // зимнее время
                            return cet - ONE_HOUR; // зимнее время
                        }
                        return cet - ONE_HOUR * 2; // летнее время
                    }
                }
                return cet - ONE_HOUR; // зимнее время
            }
            return cet - ONE_HOUR; // зимнее время
        } else {
            // Начиная с 2002 года, согласно директиве ЕС(2000/84/EC) в Европе переход на летнее время осуществляется в 01:00 по Гринвичу.
            if(iTime.month > MONTH_MARSH && iTime.month < MONTH_OCTOBER) { // летнее время
                return cet - ONE_HOUR * 2;
            } else
            if(iTime.month == MONTH_MARSH) {
                for(uint32_t d = maxDays; d >= iTime.day; d--) {
                    uint32_t _wday = get_weekday(d, MONTH_MARSH, iTime.year);
                    if(_wday == SUN) {
                        if(d == iTime.day) { // если сейчас воскресенье
                            if(iTime.hour >= (NEW_SUMMER_HOUR + 2)) return cet - ONE_HOUR * 2; // летнее время
                            return cet - ONE_HOUR; // зимнее время
                        }
                        return cet - ONE_HOUR; // зимнее время
                    }
                }
                return cet - ONE_HOUR * 2; // летнее время
            } else
            if(iTime.month == MONTH_OCTOBER) {
                for(uint32_t d = maxDays; d >= iTime.day; d--) {
                    uint32_t _wday = get_weekday(d, MONTH_OCTOBER, iTime.year);
                    if(_wday == SUN) {
                        if(d == iTime.day) { // если сейчас воскресенье
                            if(iTime.hour >= (NEW_SUMMER_HOUR + 1)) return cet - ONE_HOUR; // зимнее время
                            return cet - ONE_HOUR * 2; // летнее время
                        }
                        return cet - ONE_HOUR * 2; // летнее время
                    }
                }
                return cet - ONE_HOUR; // зимнее время
            }
            return cet - ONE_HOUR; // зимнее время
        }
        return cet - ONE_HOUR; // зимнее время
    }

    timestamp_t convert_eet_to_gmt(const timestamp_t eet) {
        return convert_cet_to_gmt(eet - SECONDS_IN_HOUR);
    }

    std::string get_str_date_time(const timestamp_t timestamp) {
        DateTime iTime(timestamp);
        return iTime.get_str_date_time();
    }

    std::string get_str_date_time_ms(const ftimestamp_t timestamp) {
        DateTime iTime(timestamp);
        return iTime.get_str_date_time_ms();
    }

    std::string get_str_date(const timestamp_t timestamp) {
        DateTime iTime(timestamp);
        return iTime.get_str_date();
    }

    std::string get_str_time(const timestamp_t timestamp, const bool is_use_seconds) {
        DateTime iTime(timestamp);
        return iTime.get_str_time(is_use_seconds);
    }

    std::string get_str_time_ms(const ftimestamp_t timestamp) {
        DateTime iTime(timestamp);
        return iTime.get_str_time_ms();
    }

    std::string get_str_iso_8601(const ztime::ftimestamp_t timestamp, const int64_t offset) {
        // YYYY-MM-DDThh:mm:ss.sssZ
        DateTime t(timestamp);
        std::string text_zone;
        if(offset == 0) {
            text_zone += "Z";
        } else {
            char text[32] = {};
            const uint64_t temp = std::abs(offset);
            sprintf(text,"%.2d:%.2d",
                ztime::get_hour_day(temp),
                ztime::get_minute_hour(temp));
            if(offset > 0) text_zone += "+";
            else text_zone += "-";
            text_zone += std::string(text);
        }
        char text[32] = {};
        sprintf(text,"%.4d-%.2d-%.2dT%.2d:%.2d:%.2d.%.3d",
            (uint32_t)t.year,
            (uint32_t)t.month,
            (uint32_t)t.day,
            (uint32_t)t.hour,
            (uint32_t)t.minute,
            (uint32_t)t.second,
            (uint32_t)t.millisecond);
        return (std::string(text) + text_zone);
    }

    std::string to_string(const std::string &mode, const ztime::ftimestamp_t timestamp) {
        if(mode.size() == 0) return std::string();
        DateTime t(timestamp);
        std::string text;
        uint32_t tick = 0;
        char str_temp[15];
        char previous_character = mode[0];
        bool is_cmd = mode[0] == '%' ? true : false;
        const size_t max_size = mode.size() + 1;
        for(size_t i = 0; i < max_size; ++i) {
            const char current_character = i >= mode.size() ? '\0' : mode[i];
            if(previous_character != current_character) {
                switch(previous_character) {
                    case '%':
                        if(tick <= 1) {
                            is_cmd = true;
                        } else if(tick == 2) {
                            tick = 0;
                            is_cmd = false;
                        }
                        break;
                    case 'Y':
                        if(is_cmd) {
                            if(tick == 4) {
                                std::fill(str_temp, str_temp + sizeof(str_temp), '\0');
                                sprintf(str_temp,"%.4d", (uint32_t)t.year);
                                text += std::string(str_temp);
                            } else
                            if(tick == 2) {
                                std::fill(str_temp, str_temp + sizeof(str_temp), '\0');
                                sprintf(str_temp,"%.2d", (uint32_t)(t.year % 100));
                                text += std::string(str_temp);
                            } else
                            if(tick == 1) {
                                std::fill(str_temp, str_temp + sizeof(str_temp), '\0');
                                sprintf(str_temp,"%d", (uint32_t)t.year);
                                text += std::string(str_temp);
                            }
                            is_cmd = false;
                        }
                        break;
                    case 'M':
                        if(is_cmd) {
                            if(tick == 2) {
                                std::fill(str_temp, str_temp + sizeof(str_temp), '\0');
                                sprintf(str_temp,"%.2d", (uint32_t)t.month);
                                text += std::string(str_temp);
                            } else
                            if(tick == 1) {
                                text += month_name_short[t.month];
                            }
                            is_cmd = false;
                        }
                        break;
                    case 'D':
                        if(is_cmd) {
                            if(tick == 2) {
                                std::fill(str_temp, str_temp + sizeof(str_temp), '\0');
                                sprintf(str_temp,"%.2d", (uint32_t)t.day);
                                text += std::string(str_temp);
                            }
                            is_cmd = false;
                        }
                        break;
                    case 'W':
                        if(is_cmd) {
                            if(tick == 1) {
                                std::fill(str_temp, str_temp + sizeof(str_temp), '\0');
                                sprintf(str_temp,"%.1d", (uint32_t)t.get_weekday());
                                text += std::string(str_temp);
                            }
                            is_cmd = false;
                        }
                        break;
                    case 'w':
                        if(is_cmd) {
                            if(tick == 1) {
                                text += weekday_name_short[t.get_weekday()];
                            }
                            is_cmd = false;
                        }
                        break;
                    case 'H':
                    case 'h':
                        if(is_cmd) {
                            if(tick == 2) {
                                std::fill(str_temp, str_temp + sizeof(str_temp), '\0');
                                sprintf(str_temp,"%.2d", (uint32_t)t.hour);
                                text += std::string(str_temp);
                            }
                            is_cmd = false;
                        }
                        break;
                    case 'm':
                        if(is_cmd) {
                            if(tick == 2) {
                                std::fill(str_temp, str_temp + sizeof(str_temp), '\0');
                                sprintf(str_temp,"%.2d", (uint32_t)t.minute);
                                text += std::string(str_temp);
                            }
                            is_cmd = false;
                        }
                        break;
                    case 'S':
                    case 's':
                        if(is_cmd) {
                            if(tick == 2) {
                                std::fill(str_temp, str_temp + sizeof(str_temp), '\0');
                                sprintf(str_temp,"%.2d", (uint32_t)t.second);
                                text += std::string(str_temp);
                            } else
                            if(tick == 3) {
                                std::fill(str_temp, str_temp + sizeof(str_temp), '\0');
                                sprintf(str_temp,"%.3d", (uint32_t)t.millisecond);
                                text += std::string(str_temp);
                            }
                            is_cmd = false;
                        }
                        break;
                }
                tick = 1;
                previous_character = current_character;
            } else {
                ++tick;
            }
            if(!is_cmd && i < mode.size() && !(current_character == '%' && tick == 1)) text += current_character;
        }
        return text;
    }

    bool is_end_month(const timestamp_t timestamp) {
        DateTime iTime(timestamp);
        return iTime.day == iTime.get_num_days_current_month();
    }

    bool is_correct_date(const uint32_t day, const uint32_t month, const uint32_t year) {
        if(day < 1 || day > MAX_DAY_MONTH) return false;
        if(month > MONTHS_IN_YEAR || month < 1) return false;
        if(year < FIRST_YEAR_UNIX) return false;
        if(day > get_num_days_month(month, year)) return false;
        return true;
    }

    bool is_correct_time(
            const uint32_t hour,
            const uint32_t minute,
            const uint32_t second,
            const uint32_t millisecond) {
        if(hour > 23) return false;
        if(minute > 59) return false;
        if(second > 59) return false;
        if(millisecond > 999) return false;
        return true;
    }

    bool is_correct_date_time(
            const uint32_t day,
            const uint32_t month,
            const uint32_t year,
            const uint32_t hour,
            const uint32_t minute,
            const uint32_t second,
            const uint32_t millisecond) {
        if(!is_correct_date(day, month, year)) return false;
        if(!is_correct_time(hour, minute, second, millisecond)) return false;
        return true;
    }

    uint32_t get_day_month(const timestamp_t timestamp) {
        uint32_t day_year = get_day_year(timestamp);
        const uint8_t JAN_AND_FEB_DAY = 60;
        // таблица для обычного года, не високосного
        const uint8_t TABLE_DAY_OF_YEAR[] = {
            0,
            1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,    // 31 январь
            1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,             // 28 февраль
            1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,    // 31 март
            1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,       // 30 апрель
            1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
            1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
            1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
            1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
            1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
            1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
            1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
            1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
        };
        if(is_leap_year(get_year(timestamp))) {
            if(day_year == JAN_AND_FEB_DAY) return TABLE_DAY_OF_YEAR[day_year - 1] + 1;
            else if(day_year > JAN_AND_FEB_DAY) return TABLE_DAY_OF_YEAR[day_year - 1];
        }
        return TABLE_DAY_OF_YEAR[day_year];
    }

    uint32_t get_month(const timestamp_t timestamp) {
        uint32_t day_year = get_day_year(timestamp);
        const unsigned char JAN_AND_FEB_DAY_LEAP_YEAR = 60;
        const unsigned char TABLE_DAY_OF_YEAR[] = {
            0,
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  // 31 январь
            2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,        // 28 февраль
            3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,  // 31 март
            4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,    // 30 апрель
            5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
            6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
            7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
            8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
            9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
            10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
            11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
            12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
        };
        return (is_leap_year(get_year(timestamp)) && day_year >= JAN_AND_FEB_DAY_LEAP_YEAR) ? TABLE_DAY_OF_YEAR[day_year - 1] : TABLE_DAY_OF_YEAR[day_year];
    }

    void delay_ms(const uint64_t milliseconds) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    void delay(const uint64_t seconds) {
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
    }
}
