//
// Created by zjh on 2024/2/19.
//

#include "TaskQueue.h"
TaskQueue::TaskQueue() {
    pthread_mutex_init(&m_mutex, NULL);
}

TaskQueue::~TaskQueue() {
    pthread_mutex_destroy(&m_mutex);
}

void TaskQueue::addTask(Task &task) {
    pthread_mutex_lock(&m_mutex);
    m_queue.push(task);
    pthread_mutex_unlock(&m_mutex);
}

void TaskQueue::addTask(callback func, void *arg) {
    pthread_mutex_lock(&m_mutex);
    Task task;
    task.function = func;
    task.arg = arg;
    m_queue.push(task);
    pthread_mutex_unlock(&m_mutex);
}

Task TaskQueue::takeTask() {
    Task t;
    pthread_mutex_lock(&m_mutex);
    if (m_queue.size() > 0) {
        t = m_queue.front();
        m_queue.pop();
    }
    pthread_mutex_unlock(&m_mutex);
    return t;
}
