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

#include"core/builtin/thread_pool.h"

using namespace atina::server::core::builtin;

thread_pool::thread_pool(size_t __si_threads)
    : _stop(false)
{
    for (size_t i = 0 ; i < __si_threads ; i++)
    {
        this->_workers.emplace_back(std::make_pair(
            [this, i]
            {
                this -> _workers[i].second = true;
                for ( ; ; )
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->_mtx);
                        this->_cv.wait(
                            lock,
                            [this]{ return this->_stop || !this->_tasks.empty(); }
                        );
                        if (this->_stop && this->_tasks.empty())
                        {
                            return;
                        }
                        task = std::move(this->_tasks.front());
                        this->_tasks.pop();
                        this->_workers[i].second = false;
                    }
                    task();
                    this->_workers[i].second = true;
                }
            },
            true // thread has nothing to do
        ));
    }
    return;
}

thread_pool::~thread_pool(){
    {
        std::unique_lock<std::mutex> lock(this->_mtx);
        this->_stop = true;
    }
    this->_cv.notify_all();
    for (auto& worker : this->_workers)
    {
        worker.first.join();
    }
    return;
}

bool thread_pool::is_terminated(){
    std::unique_lock<std::mutex> lock(this->_mtx);
    for (const auto& it : this->_workers)
    {
        if (!it.second)
        {
            return false;
        }
    }
    if (!(this->_tasks.empty()))
    {
        return false;
    }
    return true;
}

size_t thread_pool::get_task_in_queue_num(){
    std::unique_lock<std::mutex> lock(this->_mtx);
    return this->_tasks.size();
}
