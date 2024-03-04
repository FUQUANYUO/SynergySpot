#pragma once
#include <arpa/inet.h>
#include <string>
class TcpSocket {
public:
    TcpSocket();
    TcpSocket(int socket);
    ~TcpSocket();
    int connectToHost(std::string ip, unsigned short port);
    int sendMsg(std::string msg,char business_type);
    int recvMsg(std::string &msg, char &business_type);

private:
    int readn(char *buf, int size);
    int writen(const char *msg, int size);

private:
    int m_fd;// 通信的套接字
};