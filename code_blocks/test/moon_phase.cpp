#include <iostream>
#include <ztime.hpp>

using namespace std;

int main() {
    ztime::MoonPhase moon_phase;
    moon_phase.init(ztime::get_ftimestamp(18, 5, 2021, 0, 0,0));
    std::cout << "age: " << moon_phase.age << std::endl;
    std::cout << "age_in_degrees: " << moon_phase.age_in_degrees << std::endl;
    std::cout << "phase: " << moon_phase.phase << std::endl;

    std::cout << "get_moon_phase: " << ztime::get_moon_phase(ztime::get_ftimestamp(18, 5, 2021, 0, 0, 0)) << std::endl;
    std::cout << "get_moon_phase: " << ztime::get_moon_phase_v2(ztime::get_ftimestamp(18, 5, 2021, 0, 0, 0)) << std::endl;
    std::cout << "get_moon_age: " << (ztime::get_moon_age(ztime::get_ftimestamp(18, 5, 2021, 0, 0,0))) << std::endl;

    std::cout << "get_moon_phase: " << ztime::get_moon_phase_v2(ztime::get_ftimestamp(10, 6, 2021, 10, 54, 0)) << std::endl;
    std::cout << "get_moon_phase: " << ztime::get_moon_phase(ztime::get_ftimestamp(10, 6, 2021, 10, 54, 0)) << std::endl;

    std::cout << "is_new_moon (false):  " << moon_phase.is_new_moon(ztime::get_ftimestamp(11, 5, 2021, 19, 00, 0)) << std::endl;
    std::cout << "is_new_moon (true):   " << moon_phase.is_new_moon(ztime::get_ftimestamp(11, 5, 2021, 19, 01, 0)) << std::endl;
    std::cout << "is_new_moon (false):  " << moon_phase.is_new_moon(ztime::get_ftimestamp(11, 5, 2021, 19, 02, 0)) << std::endl;

    std::cout << "is_new_moon (false):  " << moon_phase.is_new_moon(ztime::get_ftimestamp(10, 6, 2021, 10, 53, 0)) << std::endl;
    std::cout << "is_new_moon (true):   " << moon_phase.is_new_moon(ztime::get_ftimestamp(10, 6, 2021, 10, 54, 0)) << std::endl;
    std::cout << "is_new_moon (false):  " << moon_phase.is_new_moon(ztime::get_ftimestamp(10, 6, 2021, 10, 55, 0)) << std::endl;

    std::cout << "is_new_moon (false):  " << moon_phase.is_new_moon(ztime::get_ftimestamp(11, 5, 2021, 19, 00, 0)) << std::endl;
    std::cout << "is_new_moon (true):   " << moon_phase.is_new_moon(ztime::get_ftimestamp(11, 5, 2021, 19, 01, 0)) << std::endl;
    std::cout << "is_new_moon (false):  " << moon_phase.is_new_moon(ztime::get_ftimestamp(11, 5, 2021, 19, 02, 0)) << std::endl;

    std::cout << "is_full_moon (false):  " << moon_phase.is_full_moon(ztime::get_ftimestamp(26, 5, 2021, 11, 13, 0)) << std::endl;
    std::cout << "is_full_moon (true):   " << moon_phase.is_full_moon(ztime::get_ftimestamp(26, 5, 2021, 11, 14, 0)) << std::endl;
    std::cout << "is_full_moon (false):  " << moon_phase.is_full_moon(ztime::get_ftimestamp(26, 5, 2021, 11, 15, 0)) << std::endl;

    std::cout << "calc_phase_v3:        " << moon_phase.calc_phase_v3(ztime::get_ftimestamp(18, 5, 2021, 0, 0, 0)) << std::endl;
    std::cout << "calc_phase_v3:        " << moon_phase.calc_phase_v3(ztime::get_ftimestamp(11, 5, 2021, 19, 00, 0)) << std::endl;
    std::cout << "calc_phase_v3:        " << moon_phase.calc_phase_v3(ztime::get_ftimestamp(11, 5, 2021, 19, 01, 0)) << std::endl;
    std::cout << "calc_phase_v3:        " << moon_phase.calc_phase_v3(ztime::get_ftimestamp(11, 5, 2021, 19, 02, 0)) << std::endl;

    std::cout << "get_moon_minute:        " << moon_phase.get_moon_minute(ztime::get_ftimestamp(18, 5, 2021, 0, 0, 0)) << std::endl;
    std::cout << "get_moon_minute:        " << moon_phase.get_moon_minute(ztime::get_ftimestamp(11, 5, 2021, 19, 00, 0)) << std::endl;
    std::cout << "get_moon_minute:        " << moon_phase.get_moon_minute(ztime::get_ftimestamp(11, 5, 2021, 19, 01, 0)) << std::endl;
    std::cout << "get_moon_minute:        " << moon_phase.get_moon_minute(ztime::get_ftimestamp(11, 5, 2021, 19, 01, 40)) << std::endl;
    std::cout << "get_moon_minute:        " << moon_phase.get_moon_minute(ztime::get_ftimestamp(11, 5, 2021, 19, 02, 0)) << std::endl;
    return 0;
}
