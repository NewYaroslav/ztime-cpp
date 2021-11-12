#include <iostream>
#include <ztime.hpp>

using namespace std;

int main() {
    ztime::timestamp_t timestamp = 0;

    std::cout << "test #1" << std::endl;
    ztime::convert_str_to_timestamp("08.08.2021 23:59:59", timestamp);
    std::cout << "convert 08.08.2021 23:59:59 == " << ztime::get_str_date_time(timestamp) << std::endl;
    std::cout << "convert 08.08.2021 23:59:59 == " << ztime::get_str_date_time(ztime::to_timestamp("08.08.2021 23:59:59")) << std::endl;

    std::cout << "test #2" << std::endl;
    timestamp = 0;
    ztime::convert_str_to_timestamp("23:59:59", timestamp);
    std::cout << "convert 23:59:59 == " << ztime::get_str_date_time(timestamp) << std::endl;
    std::cout << "convert 23:59:59 == " << ztime::get_str_date_time(ztime::to_second_day("23:59:59")) << std::endl;

    std::cout << "test #3" << std::endl;
    timestamp = 0;
    ztime::convert_str_to_timestamp(" 28 Oct 21", timestamp);
    std::cout << "convert 28 Oct 21 == " << ztime::get_str_date_time(timestamp) << std::endl;
    std::cout << "convert 28 Oct 21 == " << ztime::get_str_date_time(ztime::to_timestamp(" 28 Oct 21")) << std::endl;

    std::cout << "test #4" << std::endl;
    timestamp = 0;
    ztime::convert_str_to_timestamp("06:45:11", timestamp);
    std::cout << "convert 06:45:11 == " << ztime::get_str_date_time(timestamp) << std::endl;
    std::cout << "convert 06:45:11 == " << ztime::get_str_date_time(ztime::to_second_day("06:45:11")) << std::endl;

    std::cout << "convert-5 23:59:59 == " << ztime::get_str_date_time(ztime::to_timestamp("23:59:59")) << std::endl;
    std::cout << "convert-6 23:59:59 == " << ztime::get_str_time(ztime::to_second_day("23:59:59")) << std::endl;
    std::cout << "convert-7 23:59:00 == " << ztime::get_str_time(ztime::to_second_day("23:59")) << std::endl;
    std::cout << "convert-8 23:00:00 == " << ztime::get_str_time(ztime::to_second_day("23")) << std::endl;
    std::cout << "convert-8b 26:00:00 == " << ztime::to_second_day("26") << std::endl;

    std::cout << "convert-9 08.08.2021 23:59:59 == " << ztime::get_str_date_time(ztime::to_timestamp("08.08.2021 23:59:59")) << std::endl;
    std::cout << "convert-10 08.08.2021 23:59:00 == " << ztime::get_str_date_time(ztime::to_timestamp("08.08.2021 23:59")) << std::endl;
    std::cout << "convert-11 08.08.2021 23:59:00 == " << ztime::get_str_date_time(ztime::to_timestamp("2021.08.08 23:59")) << std::endl;

    return 0;
}
