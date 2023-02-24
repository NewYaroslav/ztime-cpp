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
#pragma once
#ifndef ZTIME_TIMER_HPP_INCLUDED
#define ZTIME_TIMER_HPP_INCLUDED

#include <chrono>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>
#include <thread>
#include <functional>

namespace ztime {

    /** \brief Class Timer - A Timer for Various Time Measurements
     */
    class Timer {
    public:

        std::atomic<uint32_t> period_ms = ATOMIC_VAR_INIT(0);

        enum class TimerMode {
            STRICT_INTERVAL,            /**< First timer mode, where the timer calls the callback at fixed intervals by resetting its internal counter before the callback is called. */
            UNSTABLE_INTERVAL,          /**< Second timer mode, where the timer resets its counter after the callback is called, making the period between callbacks unstable. */
            ONE_SHOT_AFTER_INTERVAL,    /**< Third timer mode, where the timer calls the callback only once after a set amount of time, if its counter is not reset during that time. */
        };

        Timer() :  m_start_time(clock_t::now()) {};


        Timer(  const uint32_t interval_ms,
                const TimerMode mode,
                const std::function<void()> &callback) : Timer() {
            create_event(interval_ms, mode, callback);
        }

        /** \brief Non-blocking async timer constructor
         * \param callback Your callback function that will be called asynchronously with a period of period_ms
         */
        Timer(const std::function<void()> &callback) :
            Timer(0, TimerMode::UNSTABLE_INTERVAL, callback) {
        }

        ~Timer() {
            stop_event();
        }

        bool create_event(
                const uint32_t interval_ms,
                const TimerMode mode,
                const std::function<void()> &callback) {
            std::unique_lock<std::mutex> lock(m_event_mtx);
            if (m_event_init) return false;
            if (!callback) return false;
            m_event_init = true;
            lock.unlock();

            period_ms = interval_ms;
            m_mode = mode;
            m_callback = callback;
            m_timer_future = std::async(std::launch::async, [this]() {

                using ms_t = std::chrono::duration<uint32_t, std::ratio<1, 1000>>;
                using ms64_t = std::chrono::duration<uint64_t, std::ratio<1, 1000>>;

                if (m_mode == TimerMode::ONE_SHOT_AFTER_INTERVAL) {
                    m_event_start_time = clock_t::now();
                }
                std::chrono::time_point<clock_t> start_time = clock_t::now();

                while (!false) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));

                    if (m_shutdown) return;
                    if (!period_ms) continue;

                    if (m_mode == TimerMode::ONE_SHOT_AFTER_INTERVAL) {
                        std::unique_lock<std::mutex> lock(m_event_time);
                        const uint64_t elapsed = get_elapsed<uint64_t, ms64_t>(m_event_start_time);
                        if (elapsed < period_ms) continue;
                        if (m_once_event) continue;
                        m_once_event = true;
                        lock.unlock();
                        m_callback();
                        continue;
                    }

                    const uint32_t elapsed = get_elapsed<uint32_t, ms_t>(start_time);

                    if (elapsed >= period_ms) {
                        if (m_mode == TimerMode::STRICT_INTERVAL) {
                            start_time = clock_t::now();
                        }
                        m_callback();
                        if (m_mode == TimerMode::UNSTABLE_INTERVAL) {
                            start_time = clock_t::now();
                        }
                    }
                }
            });
            return true;
        }

        bool create_event(
                const uint32_t interval_ms,
                const TimerMode mode,
                void (*callback_ptr)(void *user_data),
                void *user_data = nullptr) {
            std::unique_lock<std::mutex> lock(m_event_mtx);
            if (m_event_init) return false;
            if (!callback_ptr) return false;
            m_event_init = true;
            lock.unlock();

            m_timer_future = std::async(std::launch::async, [this]() {
                using ms_t = std::chrono::duration<uint32_t, std::ratio<1, 1000>>;
                using ms64_t = std::chrono::duration<uint64_t, std::ratio<1, 1000>>;

                if (m_mode == TimerMode::ONE_SHOT_AFTER_INTERVAL) {
                    m_event_start_time = clock_t::now();
                }
                std::chrono::time_point<clock_t> start_time = clock_t::now();

                while (!false) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));

                    if (m_shutdown) return;
                    if (!period_ms) continue;

                    if (m_mode == TimerMode::ONE_SHOT_AFTER_INTERVAL) {
                        std::unique_lock<std::mutex> lock(m_event_time);
                        const uint64_t elapsed = get_elapsed<uint64_t, ms64_t>(m_event_start_time);
                        if (elapsed < period_ms) continue;
                        if (m_once_event) continue;
                        m_once_event = true;
                        lock.unlock();
                        m_callback_ptr(m_user_data);
                        continue;
                    }

                    const uint32_t elapsed = get_elapsed<uint32_t, ms_t>(start_time);

                    if (elapsed >= period_ms) {
                        if (m_mode == TimerMode::STRICT_INTERVAL) {
                            start_time = clock_t::now();
                        }
                        m_callback_ptr(m_user_data);
                        if (m_mode == TimerMode::UNSTABLE_INTERVAL) {
                            start_time = clock_t::now();
                        }
                    }
                }
            });
            return true;
        }

        bool create_event(const std::function<void()> &callback) {
            return create_event(0, TimerMode::UNSTABLE_INTERVAL, callback);
        }

        /** \brief Reset the event timer counter
         * This method resets the event timer counter in ONE_SHOT_AFTER_INTERVAL mode
         * The method should be called within the specified interval to prevent the timer from calling the callback
         */
        inline void reset_event() noexcept {
            std::unique_lock<std::mutex> lock(m_event_time);
            m_once_event = false;
            m_event_start_time = clock_t::now();
        }

        inline void stop_event() {
            if(m_timer_future.valid()) {
                m_shutdown = true;
                try {
                    m_timer_future.wait();
                    m_timer_future.get();
                } catch(...) {}
            }
        }

        inline void set_name(const std::string &name) noexcept {
            std::unique_lock<std::mutex> lock(m_name_mtx);
            m_name = name;
        }

        inline std::string get_name() noexcept {
            std::unique_lock<std::mutex> lock(m_name_mtx);
            return m_name;
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

        template<class T1, class T2>
        inline T1 get_elapsed(std::chrono::time_point<clock_t> &start_time) noexcept {
            return std::chrono::duration_cast<T2>(
                clock_t::now() - start_time).count();
        }

        std::string m_name;
        std::mutex  m_name_mtx;

        std::mutex                          m_event_time;
        std::chrono::time_point<clock_t>    m_event_start_time;
        bool                                m_once_event = false;

        TimerMode               m_mode = TimerMode::UNSTABLE_INTERVAL;
        void                    (*m_callback_ptr)(void *user_data) = nullptr;
        void                    *m_user_data = nullptr;
        std::function<void()>   m_callback = nullptr;
        std::future<void>       m_timer_future;
        std::mutex              m_event_mtx;
        bool                    m_event_init = false;

        std::atomic<bool> m_shutdown = ATOMIC_VAR_INIT(false);
    };

}

#endif // ZTIME_TIMER_HPP_INCLUDED
