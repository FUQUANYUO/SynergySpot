#ifndef _SERVER_H_
#define _SERVER_H_

#include "tcp-server/TcpServer.h"
#include <list>
#include <map>
#include <unordered_map>
struct SockInfo {
    TcpServer *s;
    TcpSocket *tcp;
    struct sockaddr_in addr;
};

//待发送的消息表      key: 发送人账号    v:发送的聊天记录
typedef std::unordered_map<std::string, std::list<std::string>> chatLog;
#endif