//
// Created by FU-QAQ on 2024/3/1.
//

#ifndef SYNERGYSPOT_BUSINESSLISTEN_H
#define SYNERGYSPOT_BUSINESSLISTEN_H

#include <QThread>
#include <QEventLoop>
#include <memory>
#include "net-work/ClientConServer.h"

//-----------      proto-file -----------//
#include "forward_msg/ForwardMsgDTO.pb.h"
#include "disconnect/DisconnectDTO.pb.h"
#include "login/LoginDTO.pb.h"

//-----------      core      -----------//
// verify account
#include "land/land-check/LoginVerify.h"

// 回调函数宏
#define DO_CALLBACK_CLASS(__CLASS_NAME__,__FUNC_NAME__,__FUNC_RETURN_TYPE__)    \
class __CLASS_NAME__ : public QObject{                                          \
public:                                                                         \
    explicit __CLASS_NAME__(QObject * parent,std::string dto = "")              \
        : QObject(parent), dto(std::move(dto)){}                                \
    __FUNC_RETURN_TYPE__ __FUNC_NAME__();                                       \
    ~__CLASS_NAME__() = default;                                                \
private:                                                                        \
    std::string dto;                                                            \
};


/*
 * emit bl.signal -> alloc.slots
 * emit alloc.signal -> bl.signal
 * */

namespace DoBusiness{
    // 分配业务
    class AllocBusiness : public QObject{
        Q_OBJECT
    public:
        explicit AllocBusiness();
        ~AllocBusiness();
        void run();
    public slots:
        // 断开连接
        void DisConnFromSer();

        // 连接服务端
        void ConToSer();

        // 邮箱验证
        void QueryEmailVerifyCode(const std::string& outEdto);

        // 账号验证
        void VerifyAcc(const std::string& outLdto);

        // 转发信息
        void ForwardBySer(const std::string& outFmdto);

        // 查询联系人
        void QueryContactList(const std::string& outGfdto);

        // 注册账号
        void QueryEnrollAccount(const std::string& outErdto);

        // 收到好友请求
        void AddFriend(const std::string& outFdto);

        // 搜索好友请求
        void QuerySearchRequest(const std::string& outSdto);
    signals:
        // 断开连接
        void DISCONNECTFROMSER();

        // 未读业务
        void BUSINESSTODO();

        // 收到邮箱验证信息
        void GET_EMAILCODE(const std::string rawEdto);

        // 登录验证信号
        void LAND_SUCCESS();
        void LAND_FAIL();

        // 接收消息
        void RECV_MSG(const std::string rawFmdto);

        // 收到联系人信息
        void GET_CONTACTLIST(const std::string rawGfdto);

        // 获取注册账号结果
        void GET_ENROLLRES(const std::string rawErdto);

        // 添加好友
        void RECV_FRIEND_REQUEST(const std::string rawFdto);

        // 获取搜索好友结果
        void GET_SEARCH_RES(const std::string rawSdto);
    private:
        ClientConServer * _ccon;
    };
    // 下面的命令类需要手动去实现成员函数的实现

    // 获取登录验证结果
    DO_CALLBACK_CLASS(RecvVerifyRes,parseVerifyRes,void)
    // 获取注册结果
    DO_CALLBACK_CLASS(GetEnrollRes,getEnrollRes,void)
    // 接收消息任务
    DO_CALLBACK_CLASS(RecvMsgTask,forwardMsg,void)
    // 获取联系人列表
    DO_CALLBACK_CLASS(GetContactList,getContactList,void)
    // 获取邮箱验证码
    DO_CALLBACK_CLASS(GetEmailCode,getEmailCode,void)
    // 接收好友申请
    DO_CALLBACK_CLASS(RecvFriendRequest,recvFriendRequest,void)
    // 获取搜索好友结果
    DO_CALLBACK_CLASS(GetRearchRes,getRearchRes,void)
}

class BusinessListen : public QObject{
    Q_OBJECT
public:
    BusinessListen();
    ~BusinessListen();
signals:
    // 建立与服务器的连接
    void CONTOSER();

    // 邮箱验证
    void REQUEST_EMAILCODE(const std::string outEdto);
    void GET_EMAILCODE(const std::string rawEdto);

    // 登录验证信号
    void VERIFY_ACCOUNT(const std::string outLdto);
    void LAND_SUCCESS();
    void LAND_FAIL();

    // 接收消息
    void FORWARD_MSG(const std::string outFmdto);
    void RECV_MSG(const std::string rawFmdto);

    // 获取联系人列表
    void REQUEST_CONTACTLIST(const std::string outGfdto);
    void GET_CONTACTLIST(const std::string rawGfdto);

    // 注册账号
    void REQUEST_ENROLLACC(const std::string outErdto);
    void GET_ENROLLRES(const std::string rawErdto);

    // 添加好友
    void ADD_FRIEND(const std::string outFdto);
    void RECV_FRIEND_QUEST(const std::string rawFdto);

    // 搜索好友
    void REQUEST_SEARCH(const std::string outSdto);
    void GET_SEARCH_RES(const std::string rawSdto);
private:
    DoBusiness::AllocBusiness * allocBusiness;
    QThread * _t;
};
#endif//SYNERGYSPOT_BUSINESSLISTEN_H
