#ifndef GENERALSERVER_H_
#define GENERALSERVER_H_

#include "tcp-server/TcpServer.h"
#include <deque>
#include <memory>
#include <map>
#include <unordered_map>
#include <utility>
#include <mutex>
#include "tcp-server/EpollEngine.h"
#include "DTO.pb.h"


struct SockInfo {
    std::string ssid;
    std::shared_ptr<TcpSocket> tcp;
    std::vector<char> recvBuffer;
};

// 待发送的消息表      key: 发送人账号      v: 转发的消息
typedef std::unordered_map<std::string, std::deque<std::string>> chatLog;

// 待发送的消息表      key: 发送人账号      v: 发送的聊天记录
extern std::unordered_map<std::string, chatLog> sentLog;
// 在线人员表         key: 发送人账号      v:与这个账号通信的Socket指针
extern std::unordered_map<std::string, SockInfo *> onlineList;
// 文件描述符 : TcpSocket
extern std::unordered_map<int, std::shared_ptr<SockInfo>> fd_sockets;
// 离线业务处理       key: 发送人账号      v:业务包 & 业务类型
extern std::unordered_map<std::string,std::deque<std::pair<std::string,SSDTO::BusinessType>>> businessSent;

// 读事件传入参数结构体
struct ReadArgs{
    int fd;
    EpollEngine *en;
};

#endif