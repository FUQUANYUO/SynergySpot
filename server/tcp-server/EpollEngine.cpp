//
// Created by FU-QAQ on 2024/4/22.
//

#include <unistd.h>

#include "TaskQueue.h"
#include "ThreadPool.h"
#include "../Server.h"
#include "EpollEngine.h"


// 服务端对象
extern TcpServer s;
extern ThreadPool * pool;
extern void working(void *arg);

EpollEngine::EpollEngine() : epfd(-1){
    epfd = epoll_create(1);
    if(epfd == -1){
        LOG("Failed to create epoll instance.")
    }
}

EpollEngine::~EpollEngine() {
    if(epfd != -1){
        close(epfd);
    }
}

void EpollEngine::handleEvents(int numEvents) {
    for(int i = 0; i < numEvents; ++i){
        int fd = events[i].data.fd;
        if(events[i].events & EPOLLIN){
            // 是否为服务端监听
            if(s.getLisentFD() == fd){
                TcpSocket * tp = s.acceptConn();
                auto * info = new SockInfo;
                info->tcp = tp;
                info->ssid = "";
                fd_sockets[tp->getFD()] = info;
                this->addEvent(tp->getFD(),EPOLLIN);
            }else{
                auto * readArgs = new ReadArgs;
                readArgs->fd = fd;
                readArgs->en = this;
                Task task(working, readArgs);
                pool->addTask(task);
            }
        }
    }
}

int EpollEngine::waitForEvents() {
    int numEvents = epoll_wait(epfd, events, MAXEVENTS, -1);
    if (numEvents < 0) {
        LOG("Failed to wait for epoll events");
    }
    return numEvents;
}

bool EpollEngine::addEvent(int fd, uint32_t eventCode) {
    epoll_event ev{};
    ev.data.fd = fd;
    ev.events = eventCode | EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
    return true;
}

int EpollEngine::getFd() const {
    return epfd;
}

void EpollEngine::deleteEvent(int fd,epoll_event *ev) {
    epoll_ctl(epfd,EPOLL_CTL_DEL,fd,ev);
}