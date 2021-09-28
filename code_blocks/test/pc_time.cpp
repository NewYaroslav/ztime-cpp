#include <iostream>
#include <ztime.hpp>

#include <time.h>
#include <stdint.h>
#include <stdio.h>

int main() {
    while (true) {
        std::cout << "get_ftimestamp: " <<  ztime::get_str_date_time_ms(ztime::get_ftimestamp()) << std::endl;
        std::cout << "get_timestamp: " <<  ztime::get_str_date_time_ms(ztime::get_timestamp()) << std::endl;
        std::cout << "get_millisecond: " <<  ztime::get_millisecond() << std::endl;
        std::cout << "get_timestamp_us: " <<  ztime::get_timestamp_us() << std::endl;
        ztime::delay_ms(10);
    }
    return 0;
}
