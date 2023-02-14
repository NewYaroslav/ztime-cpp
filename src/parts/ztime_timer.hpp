#ifndef ZTIME_TIMER_HPP_INCLUDED
#define ZTIME_TIMER_HPP_INCLUDED

#include <chrono>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>
#include <functional>

namespace ztime {

    /** \brief Class Timer - A Timer for Various Time Measurements
     */
    class Timer {
    public:

        std::atomic<uint32_t> period_ms = ATOMIC_VAR_INIT(0);

        Timer() :  m_start_time(clock_t::now()) {};

        /** \brief Non-blocking async timer constructor
         * \param callback Your callback function that will be called asynchronously with a period of period_ms
         */
        Timer(const uint32_t interval_ms, const std::function<void()> &callback) : Timer() {
            if (callback) {
                period_ms = interval_ms;
                m_timer_future = std::async(std::launch::async, [&, callback]() {

                    std::mutex mtx;
                    std::condition_variable cv;

                    using ms_t = std::chrono::duration<uint32_t, std::ratio<1, 1000>>;
                    std::chrono::time_point<clock_t> start_time = clock_t::now();

                    while (!false) {
                        std::unique_lock<std::mutex> lock(mtx);
                        cv.wait_for(lock, std::chrono::milliseconds(1));

                        if (m_shutdown) return;
                        if (!period_ms) continue;

                        const uint32_t elapsed =
                            std::chrono::duration_cast<ms_t>(
                                clock_t::now() - start_time).count();

                        if (elapsed >= period_ms) {
                            start_time = clock_t::now();
                            callback();
                        }
                    }
                });
            }
        }

        Timer(const std::function<void()> &callback) : Timer(0, callback) {

        }

        ~Timer() {
            m_shutdown = true;
            if(m_timer_future.valid()) {
                try {
                    m_timer_future.wait();
                    m_timer_future.get();
                } catch(...) {}
            }
        }

        /** \brief Reset the timer value
         * This method should only be used with the elapsed() method.
         * When using the get_average_measurements() method, resetting the timer is not necessary!
         */
        inline void reset() noexcept {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_start_time = clock_t::now();
        }

        /** \brief Get the time measurement.
         * \return Returns the time in seconds since the class was initialized or after reset()
         */
        inline double elapsed() const noexcept {
            return std::chrono::duration_cast<second_t>(clock_t::now() - get_m_start_time()).count();
        }

        /** \brief Resets the average delay measurements
         * This method resets the delay measurement results,
         * made using the start_delay and stop_delay methods
         */
        inline void reset_avg() noexcept {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_sum = 0;
            m_count = 0;
        }

        /** \brief Start delay measurement
         * This method should be used with the stop_delay()
         * and delay_avg() methods
         */
        inline void start_delay() noexcept {
            reset();
        }

        /** \brief Stop delay measurement
         * This method should be used with the start_delay()
         * and delay_avg() methods
         */
        inline void stop_delay() noexcept {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_sum += std::chrono::duration_cast<second_t>(clock_t::now() - m_start_time).count();
            ++m_count;
        }

        /** \brief Get average delay time
         * This method should be used with the reset_avg(), start_delay()
         * and stop_delay() methods. The method returns the average result of the delay time measurements
         * \return Average time of measurements in seconds
         */
        inline double delay_avg() const noexcept {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_sum / (double)m_count;
        }

//------------------------------------------------------------------------------
        /** \brief Get the time measurement
         * \return Returns the time in seconds since the class was initialized or after reset()
         */
        inline double __attribute__((deprecated)) get_elapsed() const noexcept {
            return elapsed();
        }

        /** \brief Resets the average delay measurements
         * This method resets the delay measurement results,
         * made using the start_measurement and stop_measurement methods
         */
        inline void __attribute__((deprecated)) reset_measurement() noexcept {
            reset_avg();
        }

        /** \brief Start delay measurement
         * This method should be used with the stop_measurement()
         * and delay_avg() methods
         */
        inline void __attribute__((deprecated)) start_measurement() noexcept {
            reset();
        }

        /** \brief Stop delay measurement
         * This method should be used with the start_measurement()
         * and get_average_measurements() methods
         */
        inline void __attribute__((deprecated)) stop_measurement() noexcept {
            stop_delay();
        }

        /** \brief Get average delay time
         * This method should be used with the reset_measurement(), start_measurement()
         * and stop_measurement() methods. The method returns the average result of the delay time measurements
         * \return Average time of measurements in seconds
         */
        inline double __attribute__((deprecated)) get_average_measurements() const noexcept {
            return delay_avg();
        }
//------------------------------------------------------------------------------

    private:
        /*
            steady_clock represents a monotonic clock.
            The points in time on these clocks cannot decrease as
            physical time moves forward, and the time between these clock's
            ticks remains constant. These clocks are not tied to wall clock time
            (e.g., it can be time since the last reboot)
            and are best suited for measuring intervals.

            steady_clock представляет собой монотонные часы.
            Точки времени на этих часах не могут уменьшаться по мере того,
            как физическое время движется вперед,
            и время между тактами этих часов остается постоянным.
            Эти часы не связаны со временем настенных часов
            (например, это может быть время с момента последней перезагрузки)
            и больше всего подходят для измерения интервалов.

         */
        using clock_t = std::chrono::steady_clock;
        using second_t = std::chrono::duration<double, std::ratio<1>>;

        std::chrono::time_point<clock_t> m_start_time;

        mutable std::mutex  m_mutex;
        double      m_sum = 0;
        uint64_t    m_count = 0;

        inline std::chrono::time_point<clock_t> get_m_start_time() const noexcept {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_start_time;
        }

        std::future<void> m_timer_future;
        std::atomic<bool> m_shutdown = ATOMIC_VAR_INIT(false);
    };

}

#endif // ZTIME_TIMER_HPP_INCLUDED
