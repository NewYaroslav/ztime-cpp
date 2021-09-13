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
#ifndef ZTIME_CPU_TIME_HPP_INCLUDED
#define ZTIME_CPU_TIME_HPP_INCLUDED

#if defined(__MINGW32__) || defined(__MINGW64__) || defined(_WIN32)
#include <Windows.h>

#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <time.h>

#else
#error "Unable to define get_cpu_time( ) for an unknown OS."
#endif

#include <limits>

namespace ztime {

    /** \brief Получить количество процессорного времени, используемого текущим процессом
     *
     * Значение возвращается в секундах или NaN, 0, если произошла ошибка.
     * Оригинал функции: https://habr.com/ru/post/282301/
     * \return Возвращает количество процессорного времени, используемого текущим процессом
     */
    double get_cpu_time() {
#       if defined(__MINGW32__) || defined(__MINGW64__) || defined(_WIN32)
        /* Для Windows */
        FILETIME create_time;
        FILETIME exit_time;
        FILETIME kernel_time;
        FILETIME user_time;
        if (GetProcessTimes(GetCurrentProcess(), &create_time, &exit_time, &kernel_time, &user_time) != -1) {
            ULARGE_INTEGER li = {{user_time.dwLowDateTime, user_time.dwHighDateTime }};
            return li.QuadPart / 10000000.;
        }

#       elif   defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
        /* AIX, BSD, Cygwin, HP-UX, Linux, OSX, and Solaris */

#       if defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
        /* Предпочитаю высокое разрешение POSIX таймеры, если есть. */
        {
            clockid_t id;
            struct timespec ts;

#       if _POSIX_CPUTIME > 0

            /* Идентификаторы часов зависят от ОС. Если возможно, запросите идентификатор. */
            if (clock_getcpuclockid(0, &id) == -1) {
#               if defined(CLOCK_PROCESS_CPUTIME_ID)
                /* Использовать известный идентификатор часов для AIX, Linux, or Solaris. */
                id = CLOCK_PROCESS_CPUTIME_ID;
#               elif defined(CLOCK_VIRTUAL)
                /* Использовать известный идентификатор часов для BSD или HP-UX. */
                id = CLOCK_VIRTUAL;
#               else
                id = (clockid_t) - 1;
#               endif
            }

#       else

#           if defined(CLOCK_PROCESS_CPUTIME_ID)
            /* Использовать известный идентификатор часов для AIX, Linux, or Solaris. */
            id = CLOCK_PROCESS_CPUTIME_ID;
#           elif defined(CLOCK_VIRTUAL)
            /* Использовать известный идентификатор часов для BSD или HP-UX. */
            id = CLOCK_VIRTUAL;
#           else
            id = (clockid_t) - 1;
#           endif

#       endif

            if (id != (clockid_t) - 1 && clock_gettime(id, &ts) != -1) {
                return (double) ts.tv_sec + (double) ts.tv_nsec / 1000000000.0d;
            }
        }
#       endif

#       if defined(RUSAGE_SELF)
        {
            struct rusage rusage;
            if (getrusage(RUSAGE_SELF, &rusage) != -1) {
                return (double) rusage.ru_utime.tv_sec +
                    (double) rusage.ru_utime.tv_usec / 1000000.0d;
            }
        }
#       endif

#       if defined(_SC_CLK_TCK)
        {
            const double ticks = (double) sysconf(_SC_CLK_TCK);
            struct tms tms;
            if (times(&tms) != (clock_t) - 1) {
                return (double) tms.tms_utime / ticks;
            }
        }
#       endif

#       if defined(CLOCKS_PER_SEC)
        {
            clock_t cl = clock();
            if (cl != (clock_t) - 1) {
                return (double) cl / (double) CLOCKS_PER_SEC;
            }
        }
#       endif

#       endif
        return std::numeric_limits<double>::quiet_NaN();
    }

}
#endif // ZTIME_CPU_TIME_HPP_INCLUDED
