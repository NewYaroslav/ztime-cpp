#ifndef ZTIME_TIMER_EVENT_HPP_INCLUDED
#define ZTIME_TIMER_EVENT_HPP_INCLUDED

#include <functional>
#include <thread>
#include <chrono>
#include <future>
#include <vector>
#include <map>

namespace ztime {

    class event {
    private:

        class TimerEventHandler {
        private:
            std::vector<std::function<size_t()>>                        callbacks;
            std::vector<std::chrono::high_resolution_clock::time_point> starts;
            std::vector<std::chrono::milliseconds>                      delays;

            std::mutex method_mutex;
            std::mutex timer_future_mutex;

            std::map<size_t, size_t> id_to_index;
            size_t id_counter = 0;

            inline void offset_id_to_index(const size_t index) noexcept {
                for (auto &item : id_to_index) {
                    if (item.second == index) {
                        const size_t id = item.first;
                        id_to_index.erase(id);
                        break;
                    }
                }
                for (auto &item : id_to_index) {
                    if (item.second > index) item.second -= 1;
                }
            }

            std::atomic<bool> is_shutdown = ATOMIC_VAR_INIT(false);
            std::atomic<bool> is_init = ATOMIC_VAR_INIT(false);

            std::future<void> timer_future;
        public:

            TimerEventHandler() {};

            ~TimerEventHandler() {
                is_shutdown = true;
                remove_all();
                try {
                    std::shared_future<void> share = timer_future.share();
                    if (share.valid()) {
                        share.wait();
                        share.get();
                    }
                } catch(...) {}
            };

            inline void run() noexcept {
                std::lock_guard<std::mutex> lock(timer_future_mutex);
                if (is_init) return;
                is_init = true;
                timer_future = std::async(std::launch::async, [&] {
                    while (!is_shutdown) {
                        {//>
                            std::lock_guard<std::mutex> lock(method_mutex);
                            for (size_t i = 0; i < callbacks.size(); ++i) {
                                while (i < callbacks.size()) {
                                    if ((std::chrono::high_resolution_clock::now() - starts[i]) < delays[i]) break;
                                    const size_t result = callbacks[i]();
                                    if (!result) {
                                        callbacks.erase(callbacks.begin() + i);
                                        starts.erase(starts.begin() + i);
                                        delays.erase(delays.begin() + i);
                                        offset_id_to_index(i);
                                        continue;
                                    }
                                    delays[i] = std::chrono::milliseconds(result);
                                    starts[i] += delays[i];
                                    while ((std::chrono::high_resolution_clock::now() - starts[i]) > delays[i]) {
                                        starts[i] += delays[i];
                                    }
                                    break;
                                } // while i
                            } // for i
                        }//<
                        std::this_thread::yield();
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    }
                });
            }

            inline size_t add(
                    std::function<size_t()> callback,
                    const size_t delay_ms) noexcept {
                std::lock_guard<std::mutex> lock(method_mutex);
                callbacks.push_back(callback);
                starts.push_back(std::chrono::high_resolution_clock::now());
                delays.push_back(std::chrono::milliseconds(delay_ms));
                const size_t index = callbacks.size() - 1;
                const size_t id = id_counter++;
                id_to_index[id] = index;
                return id;
            }

            inline void reset(const size_t id) noexcept {
                std::lock_guard<std::mutex> lock(method_mutex);
                auto it = id_to_index.find(id);
                if (it == id_to_index.end()) return;
                const size_t index = it->second;
                if (index >= callbacks.size()) return;
                starts[index] = std::chrono::high_resolution_clock::now();
            }

            inline void remove_all() noexcept {
                std::lock_guard<std::mutex> lock(method_mutex);
                callbacks.clear();
                starts.clear();
                delays.clear();
                id_to_index.clear();
            }

            inline void remove(const size_t id) noexcept {
                std::lock_guard<std::mutex> lock(method_mutex);
                auto it = id_to_index.find(id);
                if (it == id_to_index.end()) return;
                const size_t index = it->second;
                if (index >= callbacks.size()) return;
                callbacks.erase(callbacks.begin() + index);
                starts.erase(starts.begin() + index);
                delays.erase(delays.begin() + index);
                offset_id_to_index(index);
            }

            inline size_t get_num_events() noexcept {
                std::lock_guard<std::mutex> lock(method_mutex);
                return callbacks.size();
            }
        }; // TimerEvent

        inline static std::map<size_t, TimerEventHandler> handlers;
        inline static std::mutex handlers_mutex;

    public:

        /** \brief Таймер события
         * Данный класс реализует событие по таймеру
         * Особенности:
         * 1. Событие происходит в отдельном потоке
         * 2. Можно настроить поток для событий
         * 3. Можно настраивать время следующего события (вернуть его внутри callback)
         * 4. Время измеряется в миллисекундах
         * 5. Деструктор класса уничтожает события
         * 6. Можно использовать один и тот же поток для разных экземпляров класса
         * 7. Если событие длилось дольше своего периода, следующее событие переносится на картное периоду время
         * 8. Если вернуть длительность 0 внутри callback, событие будет удалено
         * 9. Следующие события строго смещаются на значение delay_ms
         */
        class TimerEvent {
        private:
            std::vector<size_t> indexes;
            std::vector<size_t> thread_indexes;
            std::mutex method_mutex;
        public:

            TimerEvent() {}

            ~TimerEvent() {
                remove_all();
            }

            /** \brief Добавить callback-функцию события
             * \param callback      Callback-функция события
             * \param delay_ms      Задержка времени события, в мс.
             * \param thread_index  Индекс потока события
             * \return Вернет индекс события для данного экземпляра класса
             */
            inline size_t add(
                    std::function<size_t()> callback,
                    const size_t delay_ms,
                    const size_t thread_index = 0) noexcept {
                std::lock_guard<std::mutex> lock_1(handlers_mutex);
                auto it = handlers.find(thread_index);
                if (it == handlers.end()) {
                    handlers[thread_index].run();
                }
                std::lock_guard<std::mutex> lock_2(method_mutex);
                indexes.push_back(handlers[thread_index].add(callback, delay_ms));
                thread_indexes.push_back(thread_index);
                return indexes.size() - 1;
            }

            /** \brief Сбросить таймер для события
             * \param index Индекс события для данного экземпляра класса
             */
            inline void reset(const size_t index) noexcept {
                std::lock_guard<std::mutex> lock_1(method_mutex);
                if (index >= indexes.size()) return;
                std::lock_guard<std::mutex> lock_2(handlers_mutex);
                handlers[thread_indexes[index]].reset(indexes[index]);
            }

            /** \brief Удалить событие
             * \param index Индекс события для данного экземпляра класса
             */
            inline void remove(const size_t index) noexcept {
                std::lock_guard<std::mutex> lock_1(method_mutex);
                if (index >= indexes.size()) return;
                std::lock_guard<std::mutex> lock_2(handlers_mutex);
                handlers[thread_indexes[index]].remove(indexes[index]);
            }

            /** \brief Удалить все события
             */
            inline void remove_all() noexcept {
                std::lock_guard<std::mutex> lock_1(method_mutex);
                for (size_t i = 0; i < indexes.size(); ++i) {
                    std::lock_guard<std::mutex> lock_2(handlers_mutex);
                    auto it = handlers.find(thread_indexes[i]);
                    if (it == handlers.end()) continue;
                    it->second.remove(indexes[i]);
                }
                for (size_t i = 0; i < indexes.size(); ++i) {
                    std::lock_guard<std::mutex> lock_2(handlers_mutex);
                    auto it = handlers.find(thread_indexes[i]);
                    if (it == handlers.end()) continue;
                    if (!it->second.get_num_events()) {
                        handlers.erase(thread_indexes[i]);
                    }
                }
                indexes.clear();
                thread_indexes.clear();
            }
        }; // TimerEvent

    }; // event

    using TimerEvent = event::TimerEvent;
}; // ztime

#endif // ZTIME_TIMER_EVENT_HPP_INCLUDED
