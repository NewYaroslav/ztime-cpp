#include <iostream>
#include <ztime.hpp>

#include <time.h>
#include <stdint.h>
#include <stdio.h>

int main() {
    size_t index = 0;

    while (index < 100) {
        std::cout << "get_ftimestamp:   " << ztime::get_str_date_time_ms(ztime::get_ftimestamp()) << std::endl;
        std::cout << "get_timestamp:    " << ztime::get_str_date_time_ms(ztime::get_timestamp()) << std::endl;
        std::cout << "get_millisecond:  " << ztime::get_millisecond() << std::endl;
        std::cout << "get_timestamp_us: " << ztime::get_timestamp_us() << std::endl;
        std::cout << "get_second_day:   " << ztime::get_second_day(ztime::get_timestamp()) << std::endl;
        ztime::delay_ms(10);
        ++index;
    }

    std::cout << "get_second_day:   " << ztime::get_second_day(23,59,59) << std::endl;
    std::cout << "get_month:        " << ztime::get_month(ztime::get_timestamp(01,01,2022,0,0,0)) << std::endl;
    std::cout << "get_day_month:    " << ztime::get_day_month(ztime::get_timestamp(01,01,2022,0,0,0)) << std::endl;
    std::cout << "get_weekday:      " << ztime::get_weekday(ztime::get_timestamp(02,01,2022,0,0,0)) << std::endl;

    return 0;
}
