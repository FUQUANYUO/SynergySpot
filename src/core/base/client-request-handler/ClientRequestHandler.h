//
// Created by FUQAQ on 2025/1/7.
//

#ifndef CLIENTREQUESTHANDLER_H
#define CLIENTREQUESTHANDLER_H

#include <QObject>
#include <QMap>
#include "DTO.pb.h"
#include "task-queue/ThreadPoolManager.h"

class ClientConServer;

namespace BusinessLayer {
    class BusinessProcessor;
}

#define g_pClientRequestHandler ClientRequestHandler::getInstance()

class BusinessLayer::BusinessProcessor;

class ClientRequestHandler : public QObject {
    Q_OBJECT
public:
    static ClientRequestHandler* getInstance();
    static void destroyInstance();

    void addRequest(SSDTO::BusinessType type,std::string rawData);
signals:
    // --------------- 请求信号 --------------- //

    // 请求验证码
    void sigEmailCodeRequest(const std::string& dto);
    // 验证账号
    void sigVerifyAccountRequest(const std::string& dto);
    // 转发消息
    void sigForwardMessageRequest(const std::string& dto);
    // 请求联系人列表
    void sigContactListRequest(const std::string& dto);
    // 注册账号
    void sigEnrollAccountRequest(const std::string& dto);
    // 添加好友
    void sigAddFriendRequest(const std::string& dto);
    // 搜索好友
    void sigSearchFriendRequest(const std::string& dto);
    // 查询用户基础信息
    void sigQueryUserBaseInfoRequest(const std::string& dto);
    // 查询群组基础信息
    void sigQueryGroupBaseInfoRequest(const std::string& dto);
    // 查询群组公告
    void sigQueryGroupNoticesRequest(const std::string& dto);
    // 获取最新消息
    void sigQueryNewMessageRequest(const std::string& dto);
    // 查询文件相关信息
    void sigQueryFileInfoRequest(const std::string& dto);
    // 获取消息中的图片相关的信息
    void sigQueryMsgPicInfoRequest(const std::string& dto);


    // --------------- 响应信号 --------------- //
    void sigEmailCodeResponse(const std::string& dto);

    void sigLoginResult(const std::string& dto);

    void sigForwardMessageResponse(const std::string& dto);
    void sigContactListResponse(const std::string& dto);
    void sigEnrollAccountResponse(const std::string& dto);
    void sigFriendRequestResponse(const std::string& dto);
    void sigSearchFriendResponse(const std::string& dto);
    void sigQueryUserBaseInfoResponse(const std::string& dto);
    void sigQueryGroupBaseInfoResponse(const std::string& dto);
    void sigQueryGroupNoticesResponse(const std::string& dto);
    void sigQueryNewMessageResponse(const std::string& dto);
    void sigQueryFileInfoResponse(const std::string& dto);
    void sigQueryMsgPicInfoResponse(const std::string& dto);

    void sigConnServerFailed();
    void sigStartGRPCService();
private:
    explicit ClientRequestHandler(QObject* parent = nullptr);
    ~ClientRequestHandler();

    BusinessLayer::BusinessProcessor* businessProcessor;
    QThread * _handlerThread;
    static ClientRequestHandler* _instance;
};

namespace BusinessLayer {
    class BusinessProcessor : public QObject {
        Q_OBJECT
    public:
        explicit BusinessProcessor(QObject* parent = nullptr);
        ~BusinessProcessor();

        inline QMap<SSDTO::BusinessType , std::function<void(const std::string&)>> * getMappingFunction(const QString& type)
        {
            if (type == "request")
                return &_requestHandlerMap;
            if (type == "response")
                return &_responseHandlerMap;
            return nullptr;
        }

        void disConnectFromSer();
    signals:
        // avoid socket warning
        void sigWriteToSocket(QByteArray data);

        void sigEmailCodeResponse(const std::string& dto);

        void sigLoginResult(const std::string& dto);

        void sigForwardMessageResponse(const std::string& dto);
        void sigContactListResponse(const std::string& dto);
        void sigEnrollAccountResponse(const std::string& dto);
        void sigFriendRequestResponse(const std::string& dto);
        void sigSearchFriendResponse(const std::string& dto);
        void sigQueryUserBaseInfoResponse(const std::string& dto);
        void sigQueryGroupBaseInfoResponse(const std::string& dto);
        void sigQueryGroupNoticesResponse(const std::string& dto);
        void sigQueryNewMessageResponse(const std::string& dto);
        void sigQueryFileInfoResponse(const std::string& dto);
        void sigQueryMsgPicInfoResponse(const std::string& dto);

        void sigConnServerFailed();
        void sigStartGRPCService();
    private:
        void handleResponse(SSDTO::BusinessType type,const std::string& dto);

        ClientConServer* _ccon;
        ThreadPool _pool{10};
        QMap<SSDTO::BusinessType , std::function<void(const std::string&)>> _requestHandlerMap;
        QMap<SSDTO::BusinessType , std::function<void(const std::string&)>> _responseHandlerMap;
    };
}

#endif //CLIENTREQUESTHANDLER_H
