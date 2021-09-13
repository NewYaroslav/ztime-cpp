#include <iostream>
#include <ztime.hpp>

using namespace std;

int main() {
    std::cout << "calc: " << (int64_t)(ztime::get_julian_date(17.5, 5, 2021)*100) << std::endl;
    std::cout << "calc (2459351.9623843): " << (int64_t)(ztime::get_julian_date(17, 5, 2021, 11, 5, 50)*10000000) << std::endl;
    std::cout << "calc (2459351.9623843): " << (int64_t)(ztime::get_julian_date(ztime::get_ftimestamp(17, 5, 2021, 11, 5, 50))*10000000) << std::endl;

    std::cout << "calc (2459352.004): " << (int64_t)(ztime::get_julian_date(17, 5, 2021, 12, 5, 50)*1000) << std::endl;
    std::cout << "calc (2459351.962): " << (int64_t)(ztime::get_julian_date(17, 5, 2021, 11, 5, 50)*1000) << std::endl;

    std::cout << "(4.81, 10, 1957) to jd = 2436116.31, calc: " << (int64_t)(ztime::get_julian_date(4.81, 10, 1957)*100) << std::endl;
    if ((int64_t)(ztime::get_julian_date(4.81, 10, 1957)*100) != 243611631) {
        std::cout << "error" << std::endl;
        return 0;
    }

    std::cout << "(17, 5, 2021) to jd = 2459351.50, calc: " << (int64_t)(ztime::get_julian_date(17, 5, 2021)*100) << std::endl;
    if ((int64_t)(ztime::get_julian_date(17, 5, 2021)*100) != 245935150) {
        std::cout << "error" << std::endl;
        return 0;
    }

    std::cout << "(1, 1, 2021) to jd = 2459215.5, calc: " << (int64_t)(ztime::get_julian_date(1, 1, 2021)*100) << std::endl;
    if ((int64_t)(ztime::get_julian_date(1, 1, 2021)*100) != 245921550) {
        std::cout << "error" << std::endl;
        return 0;
    }

    std::cout << "(17, 5, 2021) to jd = 2459352, calc: " << (int64_t)(ztime::get_julian_day_number(17, 5, 2021)) << std::endl;
    if (ztime::get_julian_day_number(17, 5, 2021) != 2459352) {
        std::cout << "error " << ztime::get_julian_day_number(17, 5, 2021) << std::endl;
        return 0;
    }

    std::cout << "2299160.5, calc: " << (int64_t)(ztime::get_julian_day_number(15, 10, 1582)*100) << std::endl;
    std::cout << "2440588, calc: " << (int64_t)(ztime::get_julian_day_number(1, 1, 1970)*100) << std::endl;
    std::cout << "2451796, calc: " << (int64_t)(ztime::get_julian_day_number(9, 9, 2000)*100) << std::endl;
    std::cout << "2459352, calc: " << (int64_t)(ztime::get_julian_day_number(17, 5, 2021)*100) << std::endl;
    std::cout << "2451544, calc: " << (int64_t)(ztime::get_julian_day_number(1, 1, 2000)*100) << std::endl;

    std::cout << "a " << ((14LL - 1) / 12LL) << " a2 " << ((14.0d - (double)1) / 12.0d) << std::endl;

    std::cout << "2440588, calc: " << (int64_t)(ztime::get_julian_date(1, 1, 1970, 0, 00, 00) * 1) << std::endl;
    std::cout << "2459351.5, calc: " << (int64_t)(ztime::get_julian_date(17, 5, 2021, 0, 00, 00) * 1) << std::endl;
    std::cout << "2459352.4524884, calc: " << (int64_t)(ztime::get_julian_date(17, 5, 2021, 22, 51, 35) * 1000000) << std::endl;
    std::cout << "2451545.25, calc: " << (int64_t)(ztime::get_julian_date(1, 1, 2000, 18, 00, 00) * 100) << std::endl;
    std::cout << "2451544.9989583, calc: " << (int64_t)(ztime::get_julian_date(1, 1, 2000, 11, 58, 30) * 1000000) << std::endl;
    return 0;
}
