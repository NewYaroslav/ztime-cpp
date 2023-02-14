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
#pragma once
#ifndef ZTIME_DEFINITIONS_HPP_INCLUDED
#define ZTIME_DEFINITIONS_HPP_INCLUDED

namespace ztime {

    /*
        To be able to work after January 19, 2038, we use 64 bits instead of 32 bits
        Для того, чтобы можно было работать и после 19 января 2038 года, используем 64 бит, а не 32 бит
     */
    typedef uint64_t    timestamp_t;            ///< Integer timestamp type
    typedef double      ftimestamp_t;           ///< Floating point timestamp type
    typedef double      oadate_t;               ///< Automation date type (OADate) floating point

    //
    const timestamp_t   MAX_TIMESTAMP   = std::numeric_limits<timestamp_t>::max();
    // Maximum possible automation date value (OADate)
    const oadate_t      MAX_OADATE      = std::numeric_limits<oadate_t>::max();
    // Average number of days per year
    const double AVG_DAYS_PER_YEAR      = 365.25;

    enum TimeConstants {
        NS_PER_SEC = 1000000000,
        US_PER_SEC = 1000000,
        MS_PER_SEC = 1000,
        MS_PER_MIN = 60000,
        MS_PER_HALF_HOUR = 1800000,
        MS_PER_HOUR = 3600000,
        MS_PER_DAY = 86400000,
        SEC_PER_MIN = 60,
        SEC_PER_HALF_HOUR = 1800,
        SEC_PER_HOUR = 3600,
        SEC_PER_DAY = 86400,
        SEC_PER_YEAR = 31536000,
        SEC_PER_LEAP_YEAR = 31622400,
        AVG_SEC_PER_YEAR = 31557600,
        SEC_PER_4_YEARS = 126230400,
        MIN_PER_HOUR = 60,
        MIN_PER_DAY = 1440,
        MIN_PER_WEEK = 10080,
        MIN_PER_MONTH = 40320,
        HOURS_PER_DAY = 24,
        DAYS_PER_WEEK = 7,
        DAYS_PER_LEAP_YEAR = 366,   ///< Number of days in a leap year (Количество дней в високосном году)
        DAYS_PER_YEAR = 365,
        DAYS_PER_4_YEARS = 1461,    ///< Number of days in 4 years (Количество дней за 4 года)
        MONTHS_PER_YEAR = 12,
        MAX_DAYS_PER_MONTH = 31,    ///< Maximum number of days in a month (Максимальное количество дней в месяце)
        UNIX_EPOCH = 1970,          ///< Starting year of UNIX time (Год начала UNIX времени)
        OLE_EPOCH = 25569,          ///< OLE automation date since UNIX epoch (Дата автоматизации OLE с момента эпохи UNIX)
    };

    enum Weekday {
        SUN = 0,    ///< Sunday
        MON,        ///< Monday
        TUE,        ///< Tuesday
        WED,        ///< Wednesday
        THU,        ///< Thursday
        FRI,        ///< Friday
        SAT         ///< Saturday
    };

    enum Month {
        JAN = 1,    ///< January
        FEB,        ///< February
        MAR,        ///< March
        APR,        ///< April
        MAY,        ///< May
        JUN,        ///< June
        JUL,        ///< July
        AUG,        ///< August
        SEP,        ///< September
        OCT,        ///< October
        NOV,        ///< November
        DEC         ///< December
    };

    const char* const MonthNameLong[] = {
        "January", "February", "March",
        "April", "May", "June",
        "July", "August", "September",
        "October", "November", "December"
    };

    const char* const MonthNameShort[] = {
        "Jan", "Feb", "Mar",
        "Apr", "May", "Jun",
        "Jul", "Aug", "Sep",
        "Oct", "Nov", "Dec"
    };

    const char* const WeekdayNameShort[] = {
        "SUN", "MON", "TUE",
        "WED", "THU", "FRI",
        "SAT"
    };

//------------------------------------------------------------------------------
    __attribute__((deprecated)) const char* const month_name_long[] = {
        "January", "February", "March",
        "April", "May", "June",
        "July", "August", "September",
        "October", "November", "December"
    };

    __attribute__((deprecated)) const char* const month_name_short[] = {
        "Jan", "Feb", "Mar",
        "Apr", "May", "Jun",
        "Jul", "Aug", "Sep",
        "Oct", "Nov", "Dec"
    };

    __attribute__((deprecated)) const char* const weekday_name_short[] = {
        "SUN", "MON", "TUE",
        "WED", "THU", "FRI",
        "SAT"
    };
//------------------------------------------------------------------------------

    /// Moon phases (Фазы Луны)
    enum MoonPhases {
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

    typedef Period<uint64_t> period_t;  ///< Целочисленный тип периода времени
    typedef Period<double> fperiod_t;   ///< Тип периода времени с плавающей точкой

}

#endif // ZTIME_DEFINITIONS_HPP_INCLUDE
