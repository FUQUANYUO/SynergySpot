#include "tcp-server/TcpServer.h"
#include <list>
#include <map>
#include <unordered_map>
struct SockInfo {
    TcpServer *s;
    TcpSocket *tcp;
    struct sockaddr_in addr;
};

//在线人员表  key:账号  v:与这个账号通信的Socket指针
std::unordered_map<std::string, TcpSocket *> onlineList;

//未发送的聊天记录表  key:接收消息的账号  v:接收的消息
typedef std::unordered_map<std::string, std::list<std::string>> chatLog;

//待发送的消息表      key: 发送人账号    v:发送的聊天记录
std::unordered_map<std::string, chatLog> sentLog;