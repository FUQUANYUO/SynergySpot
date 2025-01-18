#include "TcpSocket.h"
#include <iostream>
#include <cstring>
#include <unistd.h>

using namespace std;
TcpSocket::TcpSocket() {
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
}

TcpSocket::TcpSocket(int socket) {
    m_fd = socket;
}

TcpSocket::~TcpSocket() {
    if (m_fd > 0) {
        close(m_fd);
    }
}

int TcpSocket::connectToHost(string ip, unsigned short port) {
    // 连接服务器IP port
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    inet_pton(AF_INET, ip.data(), &saddr.sin_addr.s_addr);
    int ret = connect(m_fd, (struct sockaddr *) &saddr, sizeof(saddr));
    if (ret == -1) {
        perror("connect");
        return -1;
    }
    cout << "成功和服务器建立连接..." << endl;
    return ret;
}

int TcpSocket::sendMsg(std::string msg,char business_type) {
    // 申请内存空间: 数据长度 + 包头4字节(存储数据长度)
    char *data = new char[msg.size() + 8];
    int bigLen = htonl(msg.size());
    int bigType = htonl(business_type);
    memcpy(data, &bigLen, 4);
    memcpy(data + 4,&bigType,4);
    memcpy(data + 8, msg.data(), msg.size());
    // 发送数据
    int ret = writen(data, msg.size() + 8);
    delete[] data;
    return ret;
}

int TcpSocket::recvMsg(string &msg, char &business_type) {
    // 接收数据
    int len = 0;
    business_type = -1;
    if(readn((char *) &len, 4) <= 0){
        return -1;
    }
    readn((char *) &business_type, 1);
    len = ntohl(len);
    cout << "数据块大小: " << len << endl;

    // 根据读出的长度分配内存
    char *buf = new char[len + 1];
    int ret = readn(buf, len);
    if (ret == -1)
    {
        return -1;
    }
    if (ret != len) {
        msg = "";
        business_type = -1;
        return 0;
    }
    buf[len] = '\0';
    msg = buf;
    delete[] buf;
    return 1;
}

int TcpSocket::readn(char *buf, int size) {
    int nread = 0;
    int left = size;
    char *p = buf;

    while (left > 0) {
        if ((nread = read(m_fd, p, left)) > 0) {
            p += nread;
            left -= nread;
        } else {
            return nread;
        }
    }
    return size;
}

int TcpSocket::writen(const char *msg, int size) {
    int left = size;
    int nwrite = 0;
    const char* p = msg;

    while (left > 0)
    {
        if ((nwrite = write(m_fd, p, left)) > 0)
        {
            p += nwrite;
            left -= nwrite;
        }
        else if (nwrite == -1)
        {
            return -1;
        }
    }
    return size - left;
}
