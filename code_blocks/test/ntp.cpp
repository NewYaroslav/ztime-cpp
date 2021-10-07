#include <iostream>
#include "ztime_ntp.hpp"
#include "ztime.hpp"


int main() {
    std::cout << "test Ntp" << std::endl;
    std::cout << "ztime::ntp::init return " << ztime::ntp::init() << std::endl;
    std::cout << "ztime::ntp::init return " << ztime::ntp::init() << std::endl;
    std::cout << "-ntp steady timestamp us  " << ztime::ntp::get_steady_timestamp_us() << std::endl;
    std::cout << "-ntp timestamp us         " << ztime::ntp::get_timestamp_us() << std::endl;
    std::cout << "-ntp timestamp            " << ztime::ntp::get_timestamp() << std::endl;
    std::cout << "-ntp offset us            " << ztime::ntp::get_offset_us() << std::endl;
    std::cout << "-pc  timestamp us         " << ztime::get_timestamp_us() << std::endl;

    {
        size_t index = 0;
        while (index < 100) {
            std::cout << "-ntp steady timestamp us  " << ztime::ntp::get_steady_timestamp_us() << std::endl;
            ++index;
            ztime::delay_ms(1000);
        }
    }


    // проверяем NTP клиент
    std::cout << "test NtpClient" << std::endl;
    {
        ztime::NtpClient client;
        client.on_error = [&](const int64_t code, const std::string &message) {
            std::cout << "error code " << code << " message " << message << std::endl;
        };
        client.on_update = [&](const int64_t offset) {
            std::cout << "ntp offset " << offset << std::endl;
        };
        client.set_host("us.pool.ntp.org");
        //client.set_host("ntp1.vniiftri.ru");

        size_t index = 0;
        while (index < 2) {
            if (client.busy()) {
                ztime::delay_ms(10);
                continue;
            }
            client.make_measurement();
            std::cout << "-ntp timestamp us " << client.get_timestamp_us() << std::endl;
            std::cout << "-ntp timestamp    " << client.get_timestamp() << std::endl;
            std::cout << "-ntp offset us    " << client.get_offset_us() << std::endl;
            std::cout << "-pc  timestamp us " << ztime::get_timestamp_us() << std::endl;
            ++index;
        }
    }
    // Проверяем пулл NTP клиентов
    std::cout << "test NtpClientPool" << std::endl;
    {
        ztime::NtpClientPool client_poll;
        size_t index = 0;
        while (index < 2) {
            if (client_poll.busy()) {
                ztime::delay_ms(10);
                continue;
            }
            client_poll.make_measurement();
            std::cout << "-ntp timestamp_us " << client_poll.get_timestamp_us() << std::endl;
            std::cout << "-ntp timestamp us " << client_poll.get_steady_timestamp_us() << std::endl;
            std::cout << "-ntp timestamp    " << client_poll.get_timestamp() << std::endl;
            std::cout << "-ntp offset_us    " << client_poll.get_offset_us() << std::endl;
            std::cout << "-pc  timestamp_us " << ztime::get_timestamp_us() << std::endl;
            ++index;
        }
    }

    return 0;
}
