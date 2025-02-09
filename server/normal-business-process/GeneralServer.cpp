#include <nlohmann/json.hpp>
#include <thread>
#include "GeneralServer.h"
#include "thread-pool/ThreadPool.h"
#include "yaml-cpp/yaml.h"

#include "data-process/service/user/UserService.h"
#include "data-process/service/friendship/FriendshipService.h"
#include "data-process/service/group/GroupService.h"
#include "data-process/service/district/DistrictService.h"
#include "data-process/service/file/FileService.h"
#include "data-process/service/message/MessageService.h"
#include "data-process/service/sticker/StickerService.h"

#include "do-business/do-email-code/DoEmailCode.h"

using namespace std;

std::string yamlPath = "../../../src/conf/serverInfo.yaml";

// 待发送的消息表      key: 发送人账号    v:发送的聊天记录
std::unordered_map<std::string, chatLog> sentLog;
// 在线人员表         key:账号          v:与这个账号通信的Socket指针
std::unordered_map<std::string, SockInfo *> onlineList;
// 文件描述符 : TcpSocket
std::unordered_map<int,SockInfo*> fd_sockets;
// 离线业务处理       key: 发送人账号    v:业务包 & 业务类型
std::unordered_map<std::string,std::deque<std::pair<std::string,SSDTO::BusinessType>>> businessSent;
// 构造json工具map   使用在并发环境下
std::unordered_map<std::string,nlohmann::json> jmp;

// 限制子线程操作数据库的并发操作
std::mutex m;
// 限制子线程添加群聊消息的并发操作
std::mutex groupM;

void writeRequestToEpoll(EpollEngine& en,int &fd,std::string &dto,char &businessType);

void working(void *arg){
    string ssid;
    auto readArgs = reinterpret_cast<ReadArgs*>(arg);
    int &fd = readArgs->fd;
    EpollEngine &en = *readArgs->en;

    // 读取数据
    char businessType;
    string dto;
    SockInfo *info = fd_sockets[fd];
    int res = (info->tcp->recvMsg(dto,businessType));

    // 断开连接
    if(res == -1){
        LOG_INFO("client( " << fd << " , " << info->ssid << " ) disconnected.")
        // 从在线列表移除
        if(!info->ssid.empty())
            onlineList.erase(info->ssid);

        // 去掉账户关联文件描述符
        fd_sockets.erase(fd);
        delete info->tcp;
        delete info;

        en.deleteEvent(fd);
        return ;
    }
    if(dto.empty()){
        LOG_ERROR("dto is empty! occur error please check program！")
        return ;
    }

    // 登录
    if (businessType == SSDTO::BusinessType::LOGIN_CHECK) {
        UserService uService;
        SSDTO::LoginCheckDTO ldto;
        ldto.ParseFromString(dto);
        
        bool isPass = uService.loginCheck({ldto.ssid(),"-1",ldto.password()});
        ldto.set_is_pass(isPass);
        
        string resDto;
        ldto.SerializeToString(&resDto);
        info->tcp->sendMsg(resDto, SSDTO::BusinessType::LOGIN_CHECK);

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

            auto resBusiness = businessSent.find(ldto.ssid());
            if(resBusiness != businessSent.end()){ // 存在未发送的业务
                for(const auto& it : resBusiness->second){
                    onlineList[ldto.ssid()]->tcp->sendMsg(it.first,it.second);
                }
            }
        }
    }
    // 消息转发
    else if (businessType == SSDTO::BusinessType::C_MESSAGE_CONTENT) {
        MessageService mService;
        SSDTO::MessageContentDTO mdto;
        mdto.ParseFromString(dto);

        // 持久化
        mService.insertUserMessage({
            -1,mdto.sender_ssid(),static_cast<uint8_t>(mdto.content_type()),mdto.content(),mdto.file_id(),
            {-1,mdto.recipient().message_id(),static_cast<uint8_t>(mdto.recipient().recipient_type()),
                mdto.recipient().recipient_ssid(),mdto.recipient().read_status()},mdto.create_time()
        });

        // 在线转发
        std::string targetSSID = mdto.recipient().recipient_ssid();
        std::string senderSSID = mdto.sender_ssid();
        // 群组遍历在线依次转发
        if (mdto.recipient().recipient_type() == 2) {
            GroupMemberDAO gDAO;
            vector<GroupMemberInfoDO> members = gDAO.listMembers(targetSSID);
            for (const auto& member : members ) {
                auto targetSockInfo = onlineList.find(member.ssidMember);
                if (targetSockInfo != onlineList.end()) {
                    targetSockInfo->second->tcp->sendMsg(dto,SSDTO::C_MESSAGE_CONTENT);
                }
            }
        }else { // 非群组单次转发
            auto targetSockInfo = onlineList.find(targetSSID);
            if (targetSockInfo != onlineList.end()) {
                targetSockInfo->second->tcp->sendMsg(dto,SSDTO::C_MESSAGE_CONTENT);
            }
        }
    }
    // 获取好友列表
    else if (businessType == SSDTO::BusinessType::R_FRIENDSHIP_LIST) {
        FriendshipService fService;
        SSDTO::GetAllUserFriendship fdto;
        fdto.ParseFromString(dto);

        for (const auto& it :  fService.getAllFriendshipBySSID(fdto.ssid())) {
            SSDTO::UserBaseInfoDTO * udto = fdto.add_friends();
            udto->set_ssid(it.ssid);
            udto->set_ssname(it.ssname);
            udto->set_avatar_path(it.avatarPath);
            udto->set_sex(it.sex);
            udto->set_personal_sign(it.personalSign);
            udto->set_thumb_up_count(it.thumbUpCount);
            udto->set_birthday(it.birthday);
            udto->set_region(it.region);
            udto->set_create_time(it.createTime);
        }

        std::string resDto;
        fdto.SerializeToString(&resDto);

        info->tcp->sendMsg(resDto, SSDTO::BusinessType::R_FRIENDSHIP_LIST);
    }
    // 连接断开
    else if (businessType == SSDTO::BusinessType::DISCONNECT) {
        onlineList.erase(info->ssid);
        LOG("user go offline : " << ssid)
    }
    // 获取邮件验证码
    else if(businessType == SSDTO::BusinessType::EMAIL_VERIFY){
        DoEmailCode doEmailCode;
        info->tcp->sendMsg(doEmailCode.sendEmailCode(dto),SSDTO::BusinessType::EMAIL_VERIFY);
    }
    // 账号注册
    else if(businessType == SSDTO::BusinessType::ENROLL_ACCOUNT){
        UserService uService;
        SSDTO::EnrollAccountDTO edto;
        edto.ParseFromString(dto);

        std::string newSSID = uService.enrollAcc({"",edto.email(),edto.password(),edto.password_salt()});
        edto.set_ssid(newSSID);

        std::string resDto;
        edto.SerializeToString(&resDto);
        info->tcp->sendMsg(resDto, SSDTO::BusinessType::ENROLL_ACCOUNT);
    }
    // 添加好友
    else if(businessType == SSDTO::BusinessType::MAKE_FRIEND_REQUEST){
        SSDTO::MakeFriendDTO mdto;
        mdto.ParseFromString(dto);
        mdto.set_accept(false);
        mdto.SerializeToString(&dto);

        std::string targetSSID = mdto.recipient();
        auto targetSockInfo = onlineList.find(targetSSID);
        if (targetSockInfo != onlineList.end()) {
            targetSockInfo->second->tcp->sendMsg(dto,SSDTO::MAKE_FRIEND_REQUEST);
        }else {
            auto resDequeue = businessSent.find(targetSSID);
            if (resDequeue != businessSent.end()) {
                resDequeue->second.push_back({targetSSID,SSDTO::MAKE_FRIEND_REQUEST});
            }else {
                businessSent[targetSSID] = {{targetSSID,SSDTO::MAKE_FRIEND_REQUEST}};
            }
        }
    }
    else if(businessType == SSDTO::BusinessType::MAKE_FRIEND_RESPONSE) {
        SSDTO::MakeFriendDTO mdto;
        mdto.ParseFromString(dto);

        std::string targetSSID = mdto.sender();
        if (mdto.accept()) { // 持久化好友
            FriendshipService fService;
            uint8_t friendType = (mdto.isgroup()?2:1);
            std::string grouping = mdto.isgroup()?"我加入的群聊":u8"我的好友";
            fService.addFriendship({-1,mdto.sender(),grouping,"",mdto.sender(),1,friendType});
        }

        auto targetSockInfo = onlineList.find(targetSSID);
        if (targetSockInfo != onlineList.end()) {
            targetSockInfo->second->tcp->sendMsg(dto,SSDTO::MAKE_FRIEND_RESPONSE);
        }else {
            auto resDequeue = businessSent.find(targetSSID);
            if (resDequeue != businessSent.end()) {
                resDequeue->second.push_back({targetSSID,SSDTO::MAKE_FRIEND_RESPONSE});
            }else {
                businessSent[targetSSID] = {{targetSSID,SSDTO::MAKE_FRIEND_RESPONSE}};
            }
        }
    }
    // 搜索好友
    else if(businessType == SSDTO::BusinessType::SEARCH_USER){
        UserService uService;
        SSDTO::FuzzySearchDTO fdto;
        fdto.ParseFromString(dto);

        for (const auto& it : uService.fuzzyMatch(fdto.ssid(),fdto.name())) {
            SSDTO::UserBaseInfoDTO * udto = fdto.add_userinfos();
            udto->set_ssid(it.ssid);
            udto->set_ssname(it.ssname);
            udto->set_avatar_path(it.avatarPath);
            udto->set_sex(it.sex);
            udto->set_personal_sign(it.personalSign);
            udto->set_thumb_up_count(it.thumbUpCount);
            udto->set_birthday(it.birthday);
            udto->set_region(it.region);
            udto->set_create_time(it.createTime);
        }

        std::string resDto;
        fdto.SerializeToString(&resDto);
        info->tcp->sendMsg(resDto, SSDTO::BusinessType::SEARCH_USER);
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
