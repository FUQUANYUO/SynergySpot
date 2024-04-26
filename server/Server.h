#ifndef _SERVER_H_
#define _SERVER_H_

#include "tcp-server/TcpServer.h"
#include <list>
#include <map>
#include <unordered_map>
#include <utility>
#include "EpollEngine.h"


struct SockInfo {
    std::string ssid;
    TcpSocket* tcp;
};

//待发送的消息表      key: 发送人账号    v:发送的聊天记录
typedef std::unordered_map<std::string, std::list<std::string>> chatLog;

//待发送的消息表      key: 发送人账号    v:发送的聊天记录
extern std::unordered_map<std::string, chatLog> sentLog;
//在线人员表         key:账号  v:与这个账号通信的Socket指针
extern std::unordered_map<std::string, SockInfo *> onlineList;
// 文件描述符 : TcpSocket
extern std::unordered_map<int,SockInfo*> fd_sockets;

// 读事件传入参数结构体
struct ReadArgs{
    int fd;
    EpollEngine *en;
};

#endif