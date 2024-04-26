//
// Created by FU-QAQ on 2024/4/22.
//

#ifndef SERVER_EPOLLENGINE_H
#define SERVER_EPOLLENGINE_H

#include "../../src/utils/help.h"

#include <sys/epoll.h>
#include <vector>

#define MAXEVENTS 1024

class EpollEngine {
public:
    EpollEngine();
    ~EpollEngine();

    // 添加事件
    bool addEvent(int fd,uint32_t events);

    // 等待事件响应
    int waitForEvents();

    // 处理事件
    void handleEvents(int numEvents);

    // 获取 epoll 引擎文件描述符
    inline int getFd() const;

    // 删除事件
    void deleteEvent(int fd,epoll_event *ev = nullptr);
private:
    int epfd;
    epoll_event events[MAXEVENTS];// 存放 epoll 事件动态数组
};

#endif//SERVER_EPOLLENGINE_H
