#include <iostream>
#include <ztime_sync.hpp>

int main() {
    std::cout << "Hello world!" << std::endl;
    ztime::TimeSync iTimeSync;
    while(!iTimeSync.is_time_sync()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    };

    ztime::ftimestamp_t last_utc = iTimeSync.get_ftimestamp();
    while(true) {
        ztime::ftimestamp_t real_utc = iTimeSync.get_ftimestamp();
        ztime::ftimestamp_t pc_utc = ztime::get_ftimestamp();
        if(real_utc - last_utc > 0.1) {
            std::cout << "accuracy: " << iTimeSync.get_accuracy() << " sync utc: " << ztime::get_str_time_ms(real_utc) << " pc utc: " << ztime::get_str_time_ms(pc_utc) << "\r";
            last_utc = real_utc;
        }
    };
    return 0;
}

