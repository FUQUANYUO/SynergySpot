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

int TcpSocket::sendMsg(std::string msg,int business_type) {
    // 协议头：4 字节长度 + 4 字节业务类型
    int total_size = msg.size() + 8;
    char *data = new char[total_size];

    // 填充协议头
    int bigLen = htonl(msg.size());
    int bigType = htonl(business_type);  // 正确：business_type 为 int
    memcpy(data, &bigLen, 4);
    memcpy(data + 4, &bigType, 4);

    // 填充数据
    memcpy(data + 8, msg.data(), msg.size());

    // 发送数据
    int ret = writen(data, total_size);
    delete[] data;
    return ret;
}

int TcpSocket::recvMsg(std::string &msg, int &business_type) {  // 改为 int 类型
    // 读取协议头（8 字节）
    int len = 0, type = 0;
    if (readn((char*)&len, 4) <= 0 || readn((char*)&type, 4) <= 0) {
        return -1;
    }

    // 转换字节序
    len = ntohl(len);
    business_type = ntohl(type);

    // 读取数据
    char *buf = new char[len + 1];
    int ret = readn(buf, len);
    if (ret != len) {
        delete[] buf;
        return -1;
    }
    buf[len] = '\0';
    msg = buf;
    delete[] buf;
    return 0;
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
