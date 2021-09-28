# ztime-cpp
C ++ library for working with date and time

![logo](doc/logo.png)
> boost::posix_time, std, boost ::chrono... надо учиться эффективно использовать чужие велосипеды, иначе легко завязнуть в своих...

Простая библиотека **C++** для работы с меткой времени (*timestamp*) и "понятной датой" (*human readable date*). Ранее библиотека называлась [xtime](https://github.com/NewYaroslav/xtime_cpp). Для совместимости с Visual Studio название было изменено. 
Библиотека поддерживает стандарты **C++11** и **C++14**.

## Описание

Данная библиотека представляет собой сборник алгоритмов, классов, функций и констант для удобной работы с веременем:

* Измерение точного времени при помощи NTP
* Получение реальноого времени компьютера с точностью до микросекунд
* Преобразование даты в метку времени и обратно
* Парсинг строк или формирование строки из даты
* Расчет фаз Луны, поиск следующего новолуния
* Преобразование UINX времени в OLE Automation Date и обратно
* Получение секунды, минуты, часа дня из метки времени, получение последней метки времени месяца и т.п.
* И многие другие возможности...

Форматы времени, с которыми работает библиотека:

* метка времени (*timestamp*)
* юлианский день (*JD или JDN*)
* дата автоматизации (*OADate*)
* "понятная дата" (*human readable date*)

Файлы *ztime.hpp*, *ztime_cpu_time.hpp*, *ztime_ntp.hpp* содежрат подробные комментарии.

Для хранения и преобразования меток времени используется тип данных *uint64*, поэтому у данной библиотеки нет [проблемы 2038 года](https://en.wikipedia.org/wiki/Year_2038_problem)

Примеры использования различных функций и классов можно найти в папке *code_blocks/test*.

## Пример использования

```cpp
#include <iostream>
#include "ztime_ntp.hpp"
#include "ztime.hpp"

int main() {
    ztime::ntp::init();
	
	// Time synchronized with the Internet
    std::cout << "ntp timestamp (us): " << ztime::ntp::get_timestamp_us() << std::endl;
    std::cout << "ntp timestamp  (s): " << ztime::ntp::get_timestamp() << std::endl;
	std::cout << "ntp timestamp  (s): " << ztime::ntp::get_ftimestamp() << std::endl;
    std::cout << "ntp offset    (us): " << ztime::ntp::get_offset_us() << std::endl;
	
	// Computer time
    std::cout << "pc  timestamp (us): " << ztime::get_timestamp_us() << std::endl;
	std::cout << "pc  timestamp  (s): " << ztime::get_timestamp() << std::endl;
	std::cout << "pc  timestamp  (s): " << ztime::get_ftimestamp() << std::endl;
	
	// Date
	std::cout << "date: " << ztime::get_timestamp(17, 5, 2021) << std::endl;
	std::cout << "date: " << ztime::get_timestamp(17, 5, 2021, 11, 5, 50) << std::endl;
	
	DateTime iTime(12, 6, 2013, 15, 23, 01);
	iTime = DateTime("2013-12-06T15:23:01+00:00");
	
	ztime::timestamp_t timestamp = iTime.get_timestamp();
	
	// translating GMT time during CET
	DateTime iTimeCET(convert_gmt_to_cet(timestamp));
	timestamp = iTimeCET.get_timestamp();
	
	std::cout << "date: " << ztime::get_str_date_time(timestamp) << std::endl;
	
	
	// Julian day
	std::cout << "jd: " << ztime::get_julian_date(17.5, 5, 2021) << std::endl;
	std::cout << "jd: " << ztime::get_julian_date(17, 5, 2021, 11, 5, 50) << std::endl;
	std::cout << "jdn: " << ztime::get_julian_day_number(1, 1, 1970) << std::endl;

	// Moon Phase
	ztime::MoonPhase moon_phase;
    moon_phase.init(ztime::get_ftimestamp(18, 5, 2021, 0, 0,0));
	
    std::cout << "age: " << moon_phase.age << std::endl;
    std::cout << "age_in_degrees: " << moon_phase.age_in_degrees << std::endl;
    std::cout << "phase: " << moon_phase.phase << std::endl;

    std::cout << "get_moon_phase: " << ztime::get_moon_phase(ztime::get_ftimestamp(18, 5, 2021, 0, 0, 0)) << std::endl;
    std::cout << "get_moon_phase: " << ztime::get_moon_phase_v2(ztime::get_ftimestamp(18, 5, 2021, 0, 0, 0)) << std::endl;
    std::cout << "get_moon_age: " << ztime::get_moon_age(ztime::get_ftimestamp(18, 5, 2021, 0, 0,0)) << std::endl;
	
    return 0;
}

```

## Как установить?

Просто добавьте файл *ztime.cpp* в свой проект. Подключите заголовочный файл *ztime.hpp*, а также, если нужно, *ztime_cpu_time.hpp*, *ztime_ntp.hpp*.

Подробности использования смотрите в **DESCRIPTION.md**