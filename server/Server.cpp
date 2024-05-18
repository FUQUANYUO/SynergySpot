#include <nlohmann/json.hpp>
#include <thread>
#include "Server.h"
#include "ThreadPool.h"
#include "yaml-cpp/yaml.h"

#include "DoBusinessBase.h"
#include "do-forwardmsg/DoForwardMsg.h"
#include "do-login/DoLogin.h"
#include "do-get-friendlist/DoGetFriendList.h"
#include "do-email-code/DoEmailCode.h"
#include "do-enroll-account/DoEnrollAccount.h"
#include "do-add-friend/DoAddFriend.h"
#include "do-search-friend/DoSearchFriend.h"

using namespace std;

std::string yamlPath = "../../src/conf/serverInfo.yaml";

// 待发送的消息表      key: 发送人账号    v:发送的聊天记录
std::unordered_map<std::string, chatLog> sentLog;
// 在线人员表         key:账号          v:与这个账号通信的Socket指针
std::unordered_map<std::string, SockInfo *> onlineList;
// 文件描述符 : TcpSocket
std::unordered_map<int,SockInfo*> fd_sockets;
// 离线业务处理       key: 发送人账号    v:业务包 & 业务类型
std::unordered_map<std::string,std::deque<std::pair<std::string,SSDTO::Business_Type>>> businessSent;
// 构造json工具map   使用在并发环境下
std::unordered_map<std::string,nlohmann::json> jmp;

// 限制子线程操作数据库的并发操作
std::mutex m;
// 限制子线程添加群聊消息的并发操作
std::mutex groupM;

void writeRequestToEpoll(EpollEngine& en,int &fd,std::string &dto,char &business_type);

void working(void *arg){
    string ssid;
    auto readArgs = reinterpret_cast<ReadArgs*>(arg);
    int &fd = readArgs->fd;
    EpollEngine &en = *readArgs->en;

    // 读取数据
    char business_type;
    string dto;
    SockInfo *info = fd_sockets[fd];
    int res = (info->tcp->recvMsg(dto,business_type));
    // 断开连接
    if(res == -1){
        LOG("client( " << fd << " , " << info->ssid << " ) disconnected.")
        // 从在线列表移除
        if(!info->ssid.empty())
            onlineList.erase(info->ssid);
        // 去掉账户关联文件描述符
        fd_sockets.erase(fd);
        delete info->tcp;
        delete info;
        en.deleteEvent(fd);
        LOG("user go offline")
        return ;
    }
    if(dto.empty()){
        LOG("dto is empty! occur error please check program！")
        return ;
    }

    // 登录
    if (business_type == SSDTO::Business_Type::LOGIN) {
        DoLogin dl;
        bool isPass = dl.execVerifyLogin(dto, ssid);
        string resDto = dl.sendVerifyRes(isPass);
        info->tcp->sendMsg(resDto, SSDTO::Business_Type::LOGIN);

        if(isPass){
            // 如果在线则顶掉
            auto isOnline = onlineList.find(ssid);
            if (isOnline != onlineList.end()) {
                onlineList.erase(ssid);
                LOG("account[" << ssid << "] is take over in another IP");
            }
            // 加入在线列表
            onlineList[ssid] = info;
            // 维护SocketInfo
            fd_sockets[fd]->ssid = ssid;

            DoForwardMsg::execForwardByMap(ssid);
            DoBusinessBase::execBusinessForwardByMap(ssid);

            // 回发联系人列表业务
            DoGetFriendList dgflist;
            info->tcp->sendMsg(dgflist.execQueryFriendList(dto), SSDTO::Business_Type::GET_CONTACTLIST);
        }
    }
    // 消息转发
    else if (business_type == SSDTO::Business_Type::FOWARD_MSG) {
        DoForwardMsg dfmsg;
        dfmsg.execForward(dto);
    }
    // 获取好友列表
    else if (business_type == SSDTO::Business_Type::GET_CONTACTLIST) {
        DoGetFriendList dgflist;
        info->tcp->sendMsg(dgflist.execQueryFriendList(dto), SSDTO::Business_Type::GET_CONTACTLIST);
    }
    // 连接断开
    else if (business_type == SSDTO::Business_Type::DISCONNECT) {
        onlineList.erase(info->ssid);
        LOG("user go offline")
    }
    // 获取邮件验证码
    else if(business_type == SSDTO::Business_Type::GET_EMAILCODE){
        DoEmailCode doEmailCode;
        info->tcp->sendMsg(doEmailCode.sendEmailCode(dto),SSDTO::Business_Type::GET_EMAILCODE);
    }
    // 账号注册
    else if(business_type == SSDTO::Business_Type::ENROLL){
        DoEnrollAccount doEnrollAccount;
        info->tcp->sendMsg(doEnrollAccount.execEnrollAccount(dto),SSDTO::Business_Type::ENROLL);
    }
    // 添加好友
    else if(business_type == SSDTO::Business_Type::ADD_FRIEND){
        DoAddFriend doAddFriend;
        std::string out = doAddFriend.execAddFriend(dto);
        if(!out.empty()) {
            info->tcp->sendMsg(out, SSDTO::Business_Type::ADD_FRIEND);
        }
    }
    // 搜索好友
    else if(business_type == SSDTO::Business_Type::FRIEND_SEARCH){
        DoSearchFriend doSearchFriend;
        info->tcp->sendMsg(doSearchFriend.execSearchFriendByKeyword(dto),SSDTO::Business_Type::FRIEND_SEARCH);
    }
    else {
        LOG("some error occur in parse business!")
    }
}

// 创建监听的套接字
TcpServer s;
ThreadPool * pool = nullptr;

int main() {
    YAML::Node node = YAML::LoadFile(yamlPath);
    if (node.IsNull()) return -1;
    int listenPort = node["host-info"]["listenPort"].as<int>();
    int poolMin = node["thread-pool"]["minSize"].as<int>();
    int poolMax = node["thread-pool"]["maxSize"].as<int>();

    // 创建线程池
    ThreadPool Pool(poolMin, poolMax);
    pool = &Pool;
    // 绑定本地的IP port并设置监听
    s.setListen(listenPort);
    EpollEngine en;
    // 添加服务端监听事件
    en.addEvent(s.getLisentFD(),EPOLLIN);

    while (1) {
        // 等待服务端监听响应
        int numsOfReady = en.waitForEvents();
        if (numsOfReady == -1) {
            LOG("errno value: " << errno)
            // 处理错误情况，可能需要退出循环或重启epoll
        }
        else {
            en.handleEvents(numsOfReady);
        }
    }
    return 0;
}
