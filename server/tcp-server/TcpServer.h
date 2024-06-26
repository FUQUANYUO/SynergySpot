#pragma once

#include "../tcp-socket/TcpSocket.h"

class TcpServer {
public:
    TcpServer();
    ~TcpServer();
    int setListen(unsigned short port);
    TcpSocket *acceptConn();
    int getLisentFD();
private:
    int m_fd;// 监听的套接字
};
