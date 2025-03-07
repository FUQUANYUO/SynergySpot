#pragma once

#include "../tcp-socket/TcpSocket.h"

#include <memory>


class TcpServer {
public:
    TcpServer();
    ~TcpServer();
    int setListen(unsigned short port);
    std::shared_ptr<TcpSocket> acceptConn();
    int getListenFD();
private:
    int m_fd;// 监听的套接字
};
