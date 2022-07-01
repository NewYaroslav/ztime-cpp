#include <iostream>
#include <ztime.hpp>

#include <time.h>
#include <stdint.h>
#include <stdio.h>

int main() {
    std::cout << "timestamp year (2000):    " << ztime::get_timestamp_beg_year(2000) << std::endl;
    std::cout << "timestamp year (2100):    " << ztime::get_timestamp_beg_year(2100) << std::endl;
    std::cout << "timestamp year (2200):    " << ztime::get_timestamp_beg_year(2200) << std::endl;
    std::cout << "date year (2000):         " << ztime::get_str_date_time_ms(ztime::get_timestamp_beg_year(2000)) << std::endl;
    std::cout << "date year (2100):         " << ztime::get_str_date_time_ms(ztime::get_timestamp_beg_year(2100)) << std::endl;
    std::cout << "date year (2200):         " << ztime::get_str_date_time_ms(ztime::get_timestamp_beg_year(2200)) << std::endl;
    return 0;
}
