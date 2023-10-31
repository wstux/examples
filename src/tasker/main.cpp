/*
 * experiments
 * Copyright (C) 2023  Chistyakov Alexander
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace {

class tasker
{
public:
    struct task
    {
        using task_fn_t = std::function<void()>;

        task_fn_t task_fn;
    };

    tasker(const size_t threads_cnt)
    {
        m_threads.reserve(threads_cnt);
        for (size_t i = 0; i < threads_cnt; ++i) {
            m_threads.emplace_back([this]() { thread_main(); });
        }
    }

    ~tasker()
    {
        stop();

        for (std::thread& t : m_threads) {
            t.join();
        }
    }

    void insert_task(const task& t)
    {
        insert_task_to_queue(t);
        notify_all();
    }

    bool is_stop() const { return m_is_stop; }

    void notify_all() { m_cv.notify_all(); }

    void stop() { m_is_stop = true; }

private:
    void insert_task_to_queue(const task& t)
    {
        std::lock_guard<std::mutex> lock(m_task_queue_mutex);
        m_task_queue.emplace_back(t);
    }

    void thread_main()
    {
        while (! m_is_stop) {
            wait();

            while (true) {
                task t;

                {
                    std::lock_guard<std::mutex> lock(m_task_queue_mutex);
                    if (m_task_queue.empty()) {
                        break;
                    }
                    t = m_task_queue.front();
                    m_task_queue.pop_front();
                }

                try {
                    t.task_fn();
                } catch (const std::exception& /*ex*/) {}
            }
        }
    }

    void wait()
    {
        std::unique_lock<std::mutex> lock(m_cv_mutex);
        m_cv.wait(lock, []{ return true; });
    }

private:
    std::atomic_bool m_is_stop = {false};
    std::vector<std::thread> m_threads;

    std::mutex m_task_queue_mutex;
    std::deque<task> m_task_queue;

    std::mutex m_cv_mutex;
    std::condition_variable m_cv;
};

bool is_digit(const std::string& dig)
{
    for (const char ch : dig) {
        if (! std::isdigit(ch)) {
            return false;
        }
    }
    return true;
}

} // <anonymous> namespace

int main()
{
    tasker t(std::thread::hardware_concurrency());

    while (! t.is_stop()) {
        std::string task_str;
        std::cin >> task_str;

        if (task_str == "stop") {
            t.stop();
        } else if (! is_digit(task_str)) {
            std::cout << "Invalid task '" << task_str << "'" << std::endl;
            continue;
        } else {
            size_t n = std::stol(task_str);
            tasker::task task;
            task.task_fn = [n]() {
                std::this_thread::sleep_for(std::chrono::seconds(n));

                std::thread::id id = std::this_thread::get_id();
                std::cout << "Thread '" << id << "' finished task '"
                          << std::to_string(n) << "'" << std::endl;
            };
            t.insert_task(task);
        }
    }
    t.notify_all();

    return 0;
}

