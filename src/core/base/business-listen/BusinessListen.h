//
// Created by FU-QAQ on 2024/3/1.
//

#ifndef SYNERGYSPOT_BUSINESSLISTEN_H
#define SYNERGYSPOT_BUSINESSLISTEN_H

#include <QThread>
#include <QEventLoop>
#include "net-work/ClientConServer.h"

//-----------      proto-file -----------//
#include "forward_msg/ForwardMsgDTO.pb.h"
#include "login/LoginDTO.pb.h"

//-----------      core      -----------//
// verify account
#include "land/land-check/LoginVerify.h"

namespace DoBusiness{
    // 分配业务
    class AllocBusiness : public QObject{
        Q_OBJECT
    public:
        explicit AllocBusiness();
        ~AllocBusiness();
        void run();
    public slots:
        // 连接服务端
        void ConToSer();

        // 账号验证
        void VerifyAcc(const std::string& outLdto);

        // 转发信息
        void ForwardBySer(const std::string& outFmdto);
    signals:
        // 未读业务
        void BUSINESSTODO();

        // 登录验证信号
        void LAND_SUCCESS();
        void LAND_FAIL();

        // 接收消息
        void RECV_MSG(const std::string rawFmdto);
    private:
        ClientConServer * _ccon;
    };

    // 获取登录验证结果
    class RecvVerifyRes : public QObject{
    public:
        explicit RecvVerifyRes(QObject * parent,std::string dto = nullptr);

        // 解析发送验证结果
        void parseVerifyRes();

        ~RecvVerifyRes() = default;
    private:
        std::string dto;
    };

    // 获取注册结果
    class RecvEnrollRes : public QObject{
    public:
    };

    // 接收消息任务
    class RecvMsgTask :  public QObject{
    public:
        RecvMsgTask(QObject * parent,std::string dto = nullptr);

        // 转发信息
        void forwardMsg();

        ~RecvMsgTask() = default;
    private:
        std::string dto;
    };
}

class BusinessListen : public QObject{
    Q_OBJECT
public:
    BusinessListen();
    ~BusinessListen();

signals:
    // 建立与服务器的连接
    void CONTOSER();

    // 登录验证信号
    void VERIFY_ACCOUNT(const std::string outLdto);
    void LAND_SUCCESS();
    void LAND_FAIL();

    // 接收消息
    void FORWARD_MSG(const std::string outFmdto);
    void RECV_MSG(const std::string rawFmdto);
private:
    DoBusiness::AllocBusiness * allocBusiness;
    QThread * _t;
};
#endif//SYNERGYSPOT_BUSINESSLISTEN_H
