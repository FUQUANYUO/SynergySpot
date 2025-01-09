//
// Created by FUQAQ on 2025/1/7.
//

#ifndef THREADPOOLMANAGER_H
#define THREADPOOLMANAGER_H

#include "TaskBase.h"

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads);

    ~ThreadPool();

    // 提交任务到线程池
    template<typename F, typename... Args>
    void enqueue(F &&f, Args &&...args);

private:
    std::vector<std::thread> _workers;           // 工作线程
    std::queue<std::unique_ptr<TaskBase>> _tasks;// 任务队列
    std::mutex _queueMutex;                      // 保护任务队列的互斥锁
    std::condition_variable _condition;          // 条件变量，用于线程间通信
    std::atomic<bool> _stop;                     // 线程池是否停止
};


#endif //THREADPOOLMANAGER_H
