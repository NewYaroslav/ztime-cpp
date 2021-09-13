#include <iostream>
#include <ztime.hpp>

using namespace std;

int main() {
    ztime::timestamp_t timestamp = 0;
    ztime::convert_str_to_timestamp("08.08.2021 23:59:59", timestamp);
    std::cout << "convert-1 08.08.2021 23:59:59 == " << ztime::get_str_date_time(timestamp) << std::endl;

    timestamp = 0;
    ztime::convert_str_to_timestamp("23:59:59", timestamp);
    std::cout << "convert-2 23:59:59 == " << ztime::get_str_date_time(timestamp) << std::endl;

    std::cout << "convert-3 23:59:59 == " << ztime::get_str_date_time(ztime::to_timestamp("23:59:59")) << std::endl;
    std::cout << "convert-4 23:59:59 == " << ztime::get_str_time(ztime::to_second_day("23:59:59")) << std::endl;
    std::cout << "convert-5 23:59:00 == " << ztime::get_str_time(ztime::to_second_day("23:59")) << std::endl;
    std::cout << "convert-4 23:00:00 == " << ztime::get_str_time(ztime::to_second_day("23")) << std::endl;
    std::cout << "convert-4b 26:00:00 == " << ztime::to_second_day("26") << std::endl;

    std::cout << "convert-5 08.08.2021 23:59:59 == " << ztime::get_str_date_time(ztime::to_timestamp("08.08.2021 23:59:59")) << std::endl;
    std::cout << "convert-6 08.08.2021 23:59:00 == " << ztime::get_str_date_time(ztime::to_timestamp("08.08.2021 23:59")) << std::endl;
    std::cout << "convert-6 08.08.2021 23:59:00 == " << ztime::get_str_date_time(ztime::to_timestamp("2021.08.08 23:59")) << std::endl;

    return 0;
}
