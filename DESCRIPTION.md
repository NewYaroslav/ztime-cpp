## Типы данных

* **timestamp_t** 	- Тип длиной 64 бита для хранения метки времени
* **ftimestamp_t** 	- Тип с плавающей точкой длиной 64 бита для хранения метки времени с дробной частью секунд
* **oadate_t** 		- Тип даты автоматизации (OADate) с плавающей точкой
* **period_t** 		- Тип периода, две переменные 64 бита для хранения меток времени
* **fperiod_t**		- Тип периода, две переменные с плавающей точкой длиной 64 бита для хранения меток времени

## Перечисления и константы

Все перечисления и константы представлены в файле *ztime_definitions.hpp*

```cpp
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
```

## Классы

### Timer

Многофункциональный класс. Подходит для следующих задач:

* Измерение прошедшего времени, например задержки на выполнение участка кода.
* Измерение среднего прошедшего времени
* Асинхронный вызов callback-функции с заданным периодом
* Асинхронный вызов callback-функции с изменяемым периодом
* Асинхронный вызов callback-функции через заданное время в случае отсутствия обнуления счетчика

У таймера есть три режима:

```cpp
enum class TimerMode {
	STRICT_INTERVAL,            /**< First timer mode, where the timer calls the callback at fixed intervals by resetting its internal counter before the callback is called. */
	UNSTABLE_INTERVAL,          /**< Second timer mode, where the timer resets its counter after the callback is called, making the period between callbacks unstable. */
	ONE_SHOT_AFTER_INTERVAL,    /**< Third timer mode, where the timer calls the callback only once after a set amount of time, if its counter is not reset during that time. */
};
```

Пример использования режима STRICT_INTERVAL и UNSTABLE_INTERVAL:

```cpp
ztime::Timer timer1(1000, ztime::Timer::TimerMode::STRICT_INTERVAL, [&](){
	std::cout << "event 1" << std::endl;
});

ztime::Timer timer2(1000, ztime::Timer::TimerMode::UNSTABLE_INTERVAL, [&](){
	std::cout << "event 2" << std::endl;
});
```

Разница между режимами STRICT_INTERVAL и UNSTABLE_INTERVAL такая, что таймер с режимом UNSTABLE_INTERVAL перед обнулением счетчика ожидает завершение задачи в callback-функции, а режим STRICT_INTERVAL обнуляет счетчик перед вызовом callback-функции.

Пример использования режима ONE_SHOT_AFTER_INTERVAL:

```cpp
ztime::Timer timer(1000, ztime::Timer::TimerMode::ONE_SHOT_AFTER_INTERVAL, [&](){
	std::cout << "event" << std::endl;
});

// Откладываем вызов callback
timer.reset_event();
```

### MoonPhase

Данный класс используется для расчета фаз Луны и поиска даты следующего новолуния

#### Методы класса MoonPhase

* void init(const ztime::ftimestamp_t timestamp)				- Инициализация переменных класса, касаемых возраста, фазы луны и т.д.
* void phasehunt(const ztime::timestamp_t timestamp) 			- Расчитать даты различных фаз Луны
* bool is_new_moon(const ztime::ftimestamp_t timestamp) 		- Проверить новолуние
* bool is_full_moon(const ztime::ftimestamp_t timestamp) 		- Проверить полнолуние
* double calc_phase_v3(const ztime::ftimestamp_t timestamp) 	- Посчитать Фазу Луны, основываясь на датах новолуния
* uint32_t get_moon_minute(const ztime::ftimestamp_t timestamp)	- Получить Лунную минуту (от 0 до 42523)

### DateTime

Данный класс используется для хранения времени в "понятной дате"

#### Методы класса DateTime

* set_beg_day() - установить начало дня
* set_end_day() - установить конец дня
* set_beg_month() - установить начало месяца (00:00)
* set_end_month() - установить конец месяца
* timestamp_t get_timestamp() - получить метку времени
* ftimestamp_t get_ftimestamp() - получить метку времени с плавающей запятой
* set_timestamp(const timestamp_t &timestamp) - установить метку времени
* set_ftimestamp(const ftimestamp_t &ftimestamp) - установить метку времени с плавающей запятой
* print() - напечатать время на экране
* std::string get_str_date_time() - получить время и дату в виде строки
* std::string get_str_date_time_ms() - получить время, миллисекунды и дату в виде строки
* std::string get_str_date() - получить дату в виде строки
* std::string get_str_time() - получить время в виде строки
* uint32_t get_weekday() - получить день недели
* is_leap_year() - получить флаг високосного года (если год високосный, вернет true)
* uint32_t get_num_days_current_month() - получить кол-во дней в текущшем месяце
* set_oadate() - Установить дату автоматизации OLE
* oadate_t get_oadate() - Получить дату автоматизации OLE

### NtpClient

Данный класс позволяет сделать замер смещения времени и используется в составле классов NtpClientPool и ntp

#### Методы класса NtpClient

* set_host 			- Установить хост
* make_measurement 	- Сделать замер смещения времени
* busy 				- Проверить занятость подключения к NTP
* init 				- Проверить инициализацию смещения времени
* get_offset_us	 	- Получить смещение времени в микросекундах
* get_timestamp_us 	- Получить метку времени в микросекундах
* get_timestamp_ms 	- Получить метку времени в миллисекундах
* get_timestamp		- Получить метку времени в секундах
* get_ftimestamp	- Получить метку времени в секундах с плавающей запятой

### NtpClientPool

Данный класс позволяет сделать замер смещения времени и используется в составле классов NtpClientPool и ntp

#### Методы класса NtpClientPool

* set_delay_measurements	- Установить время между измерениями пула NTP
* set_stop_measurements		- Установить количество удачных измерений пула NTP для остановки
* set_max_measurements		- Установить максимальное количество последних измерений для медианного фильтра
* set_hosts_shuffle			- Установить флаг случайного перемешивания хостов пула NTP
* set_hosts					- Установить массив хостов серверов NTP
* add_host					- Добавить хост сервера NTP
* set_default_hosts			- Установить список хостов NTP по умолчанию
* busy_client				- Проверить занятость подключений клинетов пула к NTP
* busy						- Проверить занятость пула подключений к NTP
* init						- Проверить инициализацию смещения времени
* make_measurement			- Сделать замер времени
* get_offset_us	 			- Получить смещение времени в микросекундах
* get_timestamp_us 			- Получить метку времени в микросекундах
* get_timestamp_ms 			- Получить метку времени в миллисекундах
* get_timestamp				- Получить метку времени в секундах
* get_ftimestamp			- Получить метку времени в секундах с плавающей запятой

### ntp

Данный класс позволяет сделать замер смещения времени сразу по пулу серверов NTP, получать актуальные данные смещения с заданной переодичностью и вызывать методы для получения времени в любом месте программы

#### Методы класса ntp

* init								- Инициализировать NTP
* make_measurement					- Сделать замер
* set_delay_measurements			- Установить время между измерениями пула NTP
* set_stop_measurements				- Установить количество удачных измерений пула NTP для остановки
* set_max_measurements				- Установить максимальное количество последних измерений для медианного фильтра
* set_hosts_shuffle					- Установить флаг случайного перемешивания хостов пула NTP
* set_hosts							- Установить массив хостов серверов NTP
* get_offset_us	 					- Получить смещение времени в микросекундах
* get_timestamp_us 					- Получить метку времени в микросекундах
* get_timestamp_ms 					- Получить метку времени в миллисекундах
* get_timestamp						- Получить метку времени в секундах
* get_ftimestamp					- Получить метку времени в секундах с плавающей запятой


## Функции

### Получение времени машины

* uint32_t get_millisecond() 		- Получить миллисекунду секунды
* uint32_t get_microsecond() 		- Получить микросекунду секунды
* uint32_t get_nanosecond() 		- Получить наносекунду секунды
* timestamp_t get_timestamp() 		- Получить метку времени
* timestamp_t get_timestamp_ms()	- Получить метку времени в миллисекундах
* timestamp_t get_timestamp_us()	- Получить метку времени в микросекундах
* ftimestamp_t get_ftimestamp() 	- Получить метку времени с плавающей запятой
* oadate_t get_oadate() 			- Получить дату автоматизации OLE

### Преобразование времени в строку или вывод на экран

* std::string to_string(const std::string &mode, const ztime::ftimestamp_t timestamp) - Преобразовать время и дату в строку с пользовательскими настройками.

В качестве параметров функция принимает следующие аргументы:

**%YYYY** - год

**%YY** - последние два числа года

**%Y** - год, без ограничений в 4 символа

**%MM** - месяц (число)

**%M** - месяц (скоращенное имя)

**%DD** - день месяца

**%hh** - час дня

**%mm** - минуты часа

**%ss** - секунды

**%sss** - миллисекунды

**%W** - день недели (число)

**%w** - день недели (скоращенное имя)


Пример:

```cpp
cout << "to_string " << ztime::to_string("%YYYY-%MM-%DD",ztime::get_timestamp(31,12,2016,22,55,56)) << endl;
cout << "to_string " << ztime::to_string("%DD-%MM-%YYYY %hh:%mm:%ss",ztime::get_timestamp(31,12,2016,22,55,56)) << endl;
cout << "to_string " << ztime::to_string("%hh:%mm",ztime::get_timestamp(31,12,2016,22,55,56)) << endl;
cout << "to_string " << ztime::to_string("%hh:%mm.%sss",ztime::get_ftimestamp(31,12,2016,22,55,56,567)) << endl;
```

* std::string get_str_iso_8601(const ztime::ftimestamp_t timestamp, const int64_t offset) - Получить строку в формате ISO 8601

* std::string get_str_date_time(const timestamp_t timestamp) - Получить время и дату в виде строки из метки времени 

(Формат строки: DD.MM.YYYY HH:MM:SS)

* std::string get_str_date_time_ms(const ftimestamp_t timestamp) - Получить время и дату в виде строки из метки времени
 
(Формат строки: DD.MM.YYYY HH:MM:SS.fff)

* std::string get_str_date(const timestamp_t timestamp) - Получить время и дату в виде строки из метки времени 

(Формат строки: DD.MM.YYYY)

* std::string get_str_time(const timestamp_t timestamp) - Получить время и дату в виде строки из метки времени
 
(Формат строки: HH:MM:SS)

* std::string get_str_time_ms(const ftimestamp timestamp) - Получить время и дату в виде строки из метки времени
 
(Формат строки: HH:MM:SS.fff)

* std::string get_str_date_time() - Получить время и дату компьютера в виде строки 

(Формат строки: DD.MM.YYYY HH:MM:SS)

* std::string get_str_date_time_ms() - Получить время и дату компьютера в виде строки 

(Формат строки: DD.MM.YYYY HH:MM:SS.fff)

* print_date_time(const timestamp_t timestamp) - Напечатать дату и время метки времени 

(Формат строки: DD.MM.YYYY HH:MM:SS)

### Преобразование строки в метку времени или секунду дня

* ztime::timestamp_t to_timestamp(std::string str_datetime)							- (Рекомендовано) Преобразует строку даты и времени в timestamp
* int to_second_day(std::string str_time)											- (Рекомендовано) Преобразует строку времени в секунду дня
* ztime::ftimestamp_t convert_iso_to_ftimestamp(const std::string &str_datetime) 	- Конвертировать строку в формате ISO в метку времени
* bool convert_iso(const std::string &str_datetime, DateTime& t) 					- Конвертировать строку в формате ISO в данные класса DateTime
* bool convert_str_to_timestamp(std::string str, timestamp_t& t) 					- Преобразует строку в timestamp

Функции *convert_str_to_timestamp, to_timestamp, to_second_day* подерживают следующий список разделителей чисел (/\_:-.,<пробел>) 

Функции *convert_str_to_timestamp, to_timestamp* способны распрасить время и дату и поддерживает следующие форматы времени:
	- DD MM YY Пример: 29 Aug 19
	- HH:MM:SS DD MM YY Пример: 20:25:00, 29 Aug 19
	- HH:MM:SS DD MM YY Пример: 20:25:00, 29 Aug 2019
	- HH:MM:SS DD.MM.YYYY Пример: 00:59:59 30.08.2019
	- HH:MM:SS DD-MM-YYYY Пример: 00:59:59 30-08-2019
	- YYYY-MM-DD hh:mm:ss Пример: 2013-02-25 18:25:10
	- YYYY.MM.DD hh:mm:ss Пример: 2013.02.25 18:25:10
	- YYYY.MM.DD Пример: 2013.02.25
	- DD.MM.YYYY Пример: 21.09.2018
	
Функция *to_timestamp* также поддерживает:

	- YYYY-MM-DD hh:mm	Пример: 2013-02-25 18:25
	- YYYY.MM.DD hh:mm	Пример: 2013.02.25 18:25
	- DD-MM-YYYY hh:mm	Пример: 25-02-2013 18:25	
	- DD.MM.YYYY hh:mm	Пример: 25.02.2013 18:25
	
Функция *to_second_day* оддерживает:

	- HH:MM:SS	Пример: 20:25:00
	- HH:MM		Пример: 20:25
	- HH		Пример: 20
	
### Различные преобразования

* oadate_t convert_timestamp_to_oadate(const timestamp_t &timestamp) 	- Получить дату автоматизации OLE из метки времени
* oadate_t convert_ftimestamp_to_oadate(const ftimestamp_t &timestamp) 	- Получить дату автоматизации OLE из метки времени c плавающей точкой
* timestamp_t convert_oadate_to_timestamp(const oadate_t &oadate) 		- Преобразовать дату автоматизации OLE в метку времени
* ftimestamp_t convert_oadate_to_ftimestamp(const oadate_t &oadate) 	- Преобразовать дату автоматизации OLE в метку времени  плавающей точкой
* double convert_gregorian_to_julian_date(const ftimestamp_t &timestamp)			- Преобразовать метку времени в Юлианский день
* double convert_gregorian_to_modified_julian_day(const ftimestamp_t &timestamp)	- Преобразовать метку времени в модифицированный юлианский день

### Перевод времени из одной вреемнной зоны в другую

* timestamp_t convert_gmt_to_cet(const timestamp_t &gmt) - Переводит время GMT во время CET
* timestamp_t convert_gmt_to_eet(const timestamp_t &gmt) - Переводит время GMT во время EET
* timestamp_t convert_gmt_to_msk(const timestamp_t &gmt) - *Пока не поддерживается*
* timestamp_t convert_cet_to_gmt(const timestamp_t &cet) - Переводит время CET во время GMT
* timestamp_t convert_eet_to_gmt(const timestamp_t &eet) - Переводит время EET во время GMT
* timestamp_t convert_msk_to_gmt(const timestamp_t &msk) - *Пока не поддерживается*

### Проверки различных условий

* bool is_beg_half_hour(const timestamp_t timestamp) 	- Проверить начало получаса
* bool is_beg_hour(const timestamp_t timestamp) 		- Проверить начало часа
* bool is_beg_day(const timestamp_t timestamp) 			- Проверить начало дня
* bool is_beg_week(const timestamp_t timestamp) 		- Проверить начало недели
* bool is_beg_month(timestamp_t timestamp)				- Проверить начало месяца (Данная функция проверят только день! Она не проверяет секунды, минуты, часы)
* bool is_end_month(const timestamp_t timestamp) 													- Проверить конец месяца (Данная функция проверят только день! Она не проверяет секунды, минуты, часы)
* bool is_correct_date(const uint32_t day, const uint32_t month, const uint32_t year) 				- Проверить корректность даты
* bool is_correct_time(const uint32_t hour, const uint32_t minute = 0, const uint32_t second = 0) 	- Проверить корректность времени
* bool is_correct_date_time(const uint32_t day, const uint32_t month, const uint32_t year, const uint32_t hour = 0,  const uint32_t minute = 0, const uint32_t second = 0) 	- Проверить корректность даты и времени
* bool is_day_off(const timestamp_t timestamp) 											- Проверить выходной день (суббота и воскресение)
* bool is_day_off_for_day(const uint32_t day) 											- Проверить день с начала отсчета Unix-времени на выходной день (суббота и воскресение)
* bool is_leap_year(const uint32_t year) 												- Проверка високосного года
* bool is_day_of_month(const uint32_t day, const uint32_t month, const uint32_t year) 	- Проверка корректность дня месяца

### Различные преобразования и вычисления

* sec_to_ms		- Преобразовать секунды в миллисекунды
* fsec_to_ms	- Преобразовать секунды с плавающей точкой в миллисекунды
* ms_to_sec		- Преобразовать миллисекунды в секунды
* ms_to_fsec	- Преобразовать миллисекунды в секунды с плавающей точкой 

* timestamp_t get_first_timestamp_year(const timestamp_t timestamp = get_timestamp()) 		- Получить метку времени в начале года
* timestamp_t get_last_timestamp_year(const timestamp_t timestamp = get_timestamp()) 		- Получить метку времени в конце года
* timestamp_t get_first_timestamp_month(const timestamp_t timestamp = get_timestamp()) 		- Получить метку времени в начале текущего месяца
* timestamp_t get_last_timestamp_month(const timestamp_t timestamp = get_timestamp()) 		- Получить последнюю метку времени текущего месяца
* timestamp_t get_first_timestamp_day(const timestamp_t timestamp = get_timestamp()) 		- Получить метку времени в начале дня
* timestamp_t get_last_timestamp_day(const timestamp_t timestamp = get_timestamp()) 		- Получить метку времени в конце дня
* timestamp_t get_first_timestamp_hour(const timestamp_t timestamp = get_timestamp()) 		- Получить метку времени в начале часа
* timestamp_t get_first_timestamp_minute(const timestamp_t timestamp = get_timestamp()) 	- Получить метку времени в начале минуты
* timestamp_t get_first_timestamp_previous_day(const timestamp_t timestamp = get_timestamp()) - Получить метку времени начала предыдущего дня
* timestamp_t get_timestamp_beg_year(const uint32_t year) - Получить метку времени начала года
* timestamp_t get_last_timestamp_sunday_month(const timestamp_t timestamp = get_timestamp()) 	- Получить последнюю метку времени последнего воскресения текущего месяца
* timestamp_t get_week_start_first_timestamp(const timestamp_t timestamp = get_timestamp()) 	- Получить метку времени начала дня начала недели
* timestamp_t get_week_end_first_timestamp(const timestamp_t timestamp = get_timestamp()) 		- Получить метку времени начала дня конца недели
* timestamp_t get_first_timestamp_next_day(const timestamp_t timestamp, const uint32_t days) 	- Получить метку времени начала дня через указанное количество дней
* timestamp_t get_first_timestamp_for_day(const uint32_t day) 	- Получить метку времени начала дня от начала unix-времени
* timestamp_t get_timestamp_day(const uint32_t unix_day) 		- Получить метку времени дня UINX-времени

* uint32_t get_num_days_month(const uint32_t month, const uint32_t year) 	- Получить количество дней в месяце
* uint32_t get_num_days_month(const timestamp_t timestamp) 					- Получить количество дней в месяце
* uint32_t get_weekday(const uint32_t day, const uint32_t month, const uint32_t year) - Получить день недели
* uint32_t get_weekday(const timestamp_t timestamp = get_timestamp()) 		- Получить день недели
* uint64_t get_minute(const timestamp_t timestamp = get_timestamp()) 		- Получить минуту с начала UNIX
* uint32_t get_minute_day(const timestamp_t timestamp = get_timestamp()) 	- Получить минуту дня
* uint32_t get_minute_hour(const timestamp_t timestamp = get_timestamp()) 	- Получить минуту часа
* uint32_t get_hour_day(const timestamp_t timestamp = get_timestamp()) 		- Получить час дня
* uint32_t get_second_day(const timestamp_t timestamp = get_timestamp()) 	- Получить секунду дня
* uint32_t get_second_minute(const timestamp_t timestamp = get_timestamp()) - Получить секунду минуты
* uint32_t get_day(const timestamp_t timestamp = get_timestamp()) 	- Получить день
* uint32_t get_year(const timestamp_t timestamp = get_timestamp()) 	- Получить год
* uint32_t get_day_year(const timestamp_t timestamp = get_timestamp()) 		- Получить день года
* uint32_t get_month(const timestamp_t timestamp = get_timestamp()) 		- Получить месяц года
* uint32_t get_day_in_year(const timestamp_t timestamp = get_timestamp()) 	- Получить количество дней в текущем году
* uint32_t get_day_month(const timestamp_t timestamp = get_timestamp()) 	- Получить день месяца
* uint32_t get_first_timestamp_period(const uint32_t period, const timestamp_t timestamp  = get_timestamp()) 	- Получить метку времени в начале периода
* uint32_t get_last_timestamp_period(const uint32_t period, const timestamp_t timestamp  = get_timestamp()) 	- Получить метку времени в конце периода

### Задержка

* void delay_ms(const uint64_t milliseconds) - Задержка на указанное количество миллисекунд
* void delay(const uint64_t seconds) - Задержка на указанное количество секунд

## Быстрый обзор

+ Получить Unix-время из даты и стандартного времени

```C++
using namespace ztime;
// дата 24.05.2018 время 23:59:59
ztime::timestamp_t epoch = get_timestamp(24, 5, 2018, 23, 59, 59);
```
+ Класс для хранения времени

```C++
using namespace ztime;

// Инициализируем датой 24.05.2018
DateTime iTime(24,5,2018);

// Второй вариант инициализации с указанием времени
iTime = DateTime(24,5,2018, 0, 0, 0);
// iTime = DateTime(24,5,2018);

// Третий вариант инициализации (Инициализация с указанием unix-времени в формате ISO)
// iTime = DateTime("2013-12-06T15:23:01+00:00");

// Или инициализируем Unix epoch или Unix time или POSIX time или Unix timestamp
ztime::timestamp_t unix_epoch = 1527120000;

iTime.set_timestamp(unix_epoch);

// Переменные класса DateTime
iTime.day = 24; 	// день
iTime.month = 5; 	// месяц
iTime.year = 2018 	// год
iTime.hour = 0; 	// час
iTime.minute = 0; 	// минуты
iTime.second = 0; 	// секунды

// Получить Unix epoch или Unix time или POSIX time или Unix timestamp 
unix_epoch = iTime.get_timestamp();

// Вывести время и дату на экран
iTime.print();

// Получить дату и время в виде строки
std::string str = iTime.get_str_date_time(); // В строке будет 24.05.2018 00:00:00
```

+ Получить стандартное время

```C++
using namespace ztime;

ztime::timestamp_t unix_epoch = 1527120000;
DateTime iTime = convert_timestamp_to_datetime(unix_epoch);
```

+ Получить день недели

```C++
using namespace ztime;

// Получить номер дня недели
uint32_t wday = get_weekday(24,5,2018);

if(wday == SUN) std::cout << "SUN" << std::endl; // Если функция вернула 0 или Воскресенье
else if(wday == MON) std::cout << "MON" << std::endl; // Если функция вернула 1 или Понедельник
else if(wday == TUS) std::cout << "TUS" << std::endl;
else if(wday == WED) std::cout << "WED" << std::endl;
else if(wday == FRI) std::cout << "FRI" << std::endl;
else if(wday == SAT) std::cout << "SAT" << std::endl;

ztime::timestamp_t unix_epoch = 1527120000;

// Второй вариант функции для определения дня недели
wday = get_weekday(unix_epoch);

// Получить день недели через функцию класса DateTime
DateTime iTime(24,5,2018);
wday = iTime.get_weekday();
```

+ Конвертировать строку в формате ISO в данные класса DateTime

```C++
using namespace ztime;

DateTime iTime;
std::string strISOformattedUTCdatetime = "2013-12-06T15:23:01+00:00";
if(convert_iso(strISOformattedUTCdatetime, iTime) == true) {
  iTime.print();
}
```

+ Перевод времени CET во время GMT и обратно с учетом перехода на зимнее время

```C++
using namespace ztime;
// получаем время GMT для примера
DateTime startTime(20,3,2018);

ztime::timestamp_t startGMT = startTime.get_timestamp();
// переводим время GMT во время CET
DateTime realCET(convert_gmt_to_cet(startGMT));
realCET.print();
// переводим время CET во время GMT
DateTime realGMT(convert_cet_to_gmt(realCET.get_timestamp()));
realGMT.print();
```

+ Получить Unix-время компьютера

```C++
using namespace ztime;

ztime::timestamp_t t = get_timestamp();
std::string str = get_str_date_time(); // В строке время будет предсталвено как в примере (24.05.2018 00:00:00)
```

+ Получить количество дней в месяце и день года

```C++
using namespace ztime;

uint32_t month = 2;
uint32_t year = 2018;
std::cout << get_num_days_month(month, year) << std::endl;

DateTime iTime(20,3,2018);

// Получить количество дней в текущем (март) месяце
std::cout << iTime.get_num_days_current_month() << std::endl;
// Получить день года
std::cout << get_day_year(iTime.get_timestamp()) << std::endl;

```

+ Проверка високосного года

```C++
using namespace ztime;

uint32_t year = 2018;
if(is_leap_year(year)) {
	// если год високосный, то условие сработает
}

DateTime iTime(20,3,2018);

if(iTime.is_leap_year()) {
	// если год високосный, то условие сработает
}
```

+ Получение метки времени начала дня

```C++
using namespace ztime;

ztime::timestamp_t t = get_timestamp();
t = get_first_timestamp_day(t);

```

+ OLE Automation Date (Дата автоматизации OLE)

```C++
ztime::timestamp_t timestamp = ztime::get_timestamp(1,1,2019);
ztime::oadate_t oadate = ztime::convert_timestamp_to_oadate(timestamp);
cout << "oadate " << oadate << endl;
cout << "timestamp " << timestamp << endl;
cout << "date " << ztime::get_str_date_time(ztime::convert_oadate_to_timestamp(oadate)) << endl;
ztime::timestamp_t stop_timestamp = ztime::get_timestamp(31,12,9999);
cout << "31.12.9999 oadate " << ztime::convert_timestamp_to_oadate(stop_timestamp) << endl;
cout << "31.12.9999 oadate " << ztime::get_oadate(31,12,9999) << endl;
cout.precision(12);
ztime::timestamp_t last_timestamp = ztime::get_timestamp();
while(true) {
	if(ztime::get_timestamp() > last_timestamp) {
		std::cout << "oadate " << ztime::get_ole_automation_date() << std::endl;
		last_timestamp = ztime::get_timestamp();
	}
}
```
