//
// Created by FUQAQ on 2025/1/7.
//

#include "ThreadPoolManager.h"

ThreadPool::ThreadPool(size_t numThreads) : _stop(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        _workers.emplace_back([this] {
            while (true) {
                std::unique_ptr<TaskBase> task;
                {
                    std::unique_lock<std::mutex> lock(_queueMutex);
                    _condition.wait(lock, [this] {
                        return _stop || !_tasks.empty();
                    });

                    if (_stop && _tasks.empty()) {
                        return; // 线程池停止且任务队列为空，退出线程
                    }

                    task = std::move(_tasks.front());
                    _tasks.pop();
                }

                if (task) {
                    task->process(); // 执行任务
                }
            }
        });
    }
}

template <typename F, typename ... Args>
void ThreadPool::enqueue(F &&f, Args &&...args){
    auto task = std::make_unique<FunctionTask>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    {
        std::unique_lock<std::mutex> lock(_queueMutex);
        if (_stop) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }
        _tasks.emplace(std::move(task));
    }

    _condition.notify_one(); // 唤醒一个线程
}


ThreadPool::~ThreadPool() {
    _stop = true;
    _condition.notify_all(); // 唤醒所有线程
    for (auto& worker : _workers) {
        if (worker.joinable()) {
            worker.join(); // 等待所有线程结束
        }
    }
}