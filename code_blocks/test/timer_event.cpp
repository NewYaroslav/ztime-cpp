#include <iostream>
#include <ztime_timer_event.hpp>
#include <ztime.hpp>

int main() {
//------------------------------------------------------------------------------
    std::cout << "test async-timer" << std::endl;
    {
        ztime::Timer timer_a;
        ztime::Timer timer(1000, ztime::Timer::TimerMode::STRICT_INTERVAL, [&](){
            std::cout << "elapsed: " << timer_a.elapsed() << std::endl;
            timer_a.reset();
        });
        std::system("pause");
        std::cout << "destroy" << std::endl;
    }
    std::cout << "destroy ok" << std::endl;
//------------------------------------------------------------------------------
    std::system("pause");
    std::cout << "start #1" << std::endl;
    {
        ztime::Timer timer_a;
        ztime::TimerEvent event;

        event.add([&]()->size_t {
            std::cout << "test#A elapsed: " << timer_a.elapsed() << std::endl;
            timer_a.reset();
            return 1000;
        }, 1000);

        std::system("pause");
        std::cout << "destroy" << std::endl;
    }
    std::cout << "destroy ok" << std::endl;
    std::system("pause");
    std::cout << "start #2" << std::endl;
    {
        ztime::Timer timer_a;
        ztime::Timer timer_b;
        ztime::Timer timer_c;
        ztime::Timer timer_d;
        ztime::Timer timer_f;

        ztime::TimerEvent event;

        event.add([&]()->size_t {
            std::cout << "test#A elapsed: " << timer_a.elapsed() << std::endl;
            timer_a.reset();
            return 1000;
        }, 1000);

        event.add([&]()->size_t {
            std::cout << "test#B elapsed: " << timer_b.elapsed() << std::endl;
            timer_b.reset();
            return 500;
        }, 500);

        event.add([&]()->size_t {
            std::cout << "test#C elapsed: " << timer_c.elapsed() << std::endl;
            timer_c.reset();
            return 5000;
        }, 5000);

        event.add([&]()->size_t {
            std::cout << "test#F elapsed: " << timer_f.elapsed() << std::endl;
            timer_f.reset();
            return 0; // завершаем событие
        }, 15000);

        {
            ztime::TimerEvent event_d;
            event_d.add([&]()->size_t {
                std::cout << "test#D elapsed: " << timer_d.elapsed() << std::endl;
                timer_d.reset();
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                return 1000;
            }, 1000, 1);
            std::system("pause");
            std::cout << "test destroy event_d" << std::endl;
        }

        std::system("pause");
        std::cout << "destroy" << std::endl;
    }
    std::cout << "destroy ok" << std::endl;
    std::system("pause");
    return 0;
}
