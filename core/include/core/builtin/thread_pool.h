// progschj/ThreadPool <https://github.com/progschj/ThreadPool>
// based on commit 9a42ec1, Sep 26. 2014

/**
 * Copyright (c) 2012 Jakob Progsch, Václav Zeman

 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.

 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 *    1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 
 *    2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 
 *    3. This notice may not be removed or altered from any source
 *    distribution.
*/

#pragma once

#ifndef __ATINA_SERVER_CORE_BUILTIN_THREAD_POOL_H__
#define __ATINA_SERVER_CORE_BUILTIN_THREAD_POOL_H__

#include<condition_variable>
#include<functional>
#include<future>
#include<memory>
#include<mutex>
#include<queue>
#include<stdexcept>
#include<thread>
#include<vector>

namespace atina::server::core::builtin {

    class thread_pool {

        public:
            thread_pool(size_t __si_threads);
            template<class F, class... Args>
            auto enqueue(F&& __f, Args&&... __args) 
                -> std::future<typename std::result_of<F(Args...)>::type>;
            ~thread_pool();
            bool is_terminated();
            size_t get_task_in_queue_num();

        private:
            std::vector<std::pair<std::thread, bool>> _workers;
            std::queue<std::function<void()>> _tasks;
            
            std::mutex _mtx;
            std::condition_variable _cv;
            bool _stop;

    }; // class ThreadPool

    template<class F, class... Args>
    auto thread_pool::enqueue(F&& __f, Args&&... __args) 
        -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using return_type = typename std::result_of<F(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(__f), std::forward<Args>(__args)...)
        );
            
        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(this->_mtx);

            if (this->_stop)
            {
                throw std::runtime_error(
                    "enqueue on stopped thread pool"
                );
            } // don't allow enqueueing after stopping the pool

            _tasks.emplace([task](){ (*task)(); });
        }
        _cv.notify_one();
        return res;
    } // auto thread_pool::enqueue(...)

} // namespace atina::server::core::builtin

#endif // __ATINA_SERVER_CORE_BUILTIN_THREAD_POOL_H__
