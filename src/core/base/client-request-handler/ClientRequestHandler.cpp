//
// Created by FUQAQ on 2025/1/7.
//

#include "ClientRequestHandler.h"

#include "help.h"

#include <QThread>
#include <QTimer>
#include <functional>

//-----------      core      -----------//
#include "net-work/ClientConServer.h"
#include "common-data/CommonData.h"

// 发送包装宏
#define SEND_PACKAGE(__DTO_OBJ__, __DTO_TYPE__, __LOG__)    \
    QByteArray packet;                                      \
    QDataStream stream(&packet, QIODevice::WriteOnly);      \
    stream << static_cast<quint32>(__DTO_OBJ__.size());     \
    stream << static_cast<quint32>(__DTO_TYPE__);           \
    packet.append(__DTO_OBJ__.c_str(), __DTO_OBJ__.size()); \
    emit sigWriteToSocket(packet);                          \
    LOG(__LOG__)


// 任务类定义
#define TASK_CLASS(__CLASS_NAME__,__DTO__,__DTO_TYPE__,__LOG__) \
class __CLASS_NAME__ : public TaskBase { \
public: \
    void __CLASS_NAME__() = default;    \
    void ~__CLASS_NAME__() = default;   \
    void process() override { \
        SEND_PACKAGE(__DTO__, __DTO_TYPE__, __LOG__) \
    } \
};

ClientRequestHandler* ClientRequestHandler::_instance = nullptr;
static std::mutex m;

ClientRequestHandler* ClientRequestHandler::getInstance() {
    if (_instance == nullptr) {
        m.lock();
        if (_instance == nullptr) {
            _instance = new ClientRequestHandler();
        }
        m.unlock();
    }
    return _instance;
}

void ClientRequestHandler::destroyInstance() {
    if (_instance != nullptr) {
        m.lock();
        if (_instance != nullptr) {
            _instance->deleteLater();
            _instance = nullptr;
        }
        m.unlock();
    }
}
void ClientRequestHandler::addRequest(SSDTO::BusinessType type, std::string dto) {
    switch (type) {
        case SSDTO::LOGIN_CHECK:
            emit sigVerifyAccountRequest(dto);
            break;
        case SSDTO::C_MESSAGE_CONTENT:
            emit sigForwardMessageRequest(dto);
            break;
        case SSDTO::R_FRIENDSHIP_LIST:
            emit sigContactListRequest(dto);
            break;
        case SSDTO::EMAIL_VERIFY:
            emit sigEmailCodeRequest(dto);
            break;
        case SSDTO::ENROLL_ACCOUNT:
            emit sigEnrollAccountRequest(dto);
            break;
        case SSDTO::MAKE_FRIEND_REQUEST:
            emit sigAddFriendRequest(dto);
            break;
        case SSDTO::FUZZY_SEARCH:
            emit sigFuzzySearchRequest(dto);
            break;
        case SSDTO::R_USER_BASE_INFO:
            emit sigQueryUserBaseInfoRequest(dto);
            break;
        case SSDTO::R_GROUP_BASE_INFO:
            emit sigQueryGroupBaseInfoRequest(dto);
            break;
        case SSDTO::R_GROUP_NOTICE:
            emit sigQueryGroupNoticesRequest(dto);
            break;
        default:
            LOG("Unsupported business type:" << type)
            break;
    }
}

ClientRequestHandler::ClientRequestHandler(QObject* parent) : QObject(parent) {
    businessProcessor = new BusinessLayer::BusinessProcessor();
    _handlerThread = new QThread;
    businessProcessor->moveToThread(_handlerThread);

    // 连接业务信号槽
    connect(this, &ClientRequestHandler::sigEmailCodeRequest,                businessProcessor,    businessProcessor->getMappingFunction("request")->value(SSDTO::EMAIL_VERIFY),Qt::QueuedConnection);
    connect(this, &ClientRequestHandler::sigVerifyAccountRequest,            businessProcessor,    businessProcessor->getMappingFunction("request")->value(SSDTO::LOGIN_CHECK),Qt::QueuedConnection);
    connect(this, &ClientRequestHandler::sigForwardMessageRequest,           businessProcessor,    businessProcessor->getMappingFunction("request")->value(SSDTO::C_MESSAGE_CONTENT),Qt::QueuedConnection);
    connect(this, &ClientRequestHandler::sigContactListRequest,              businessProcessor,    businessProcessor->getMappingFunction("request")->value(SSDTO::R_FRIENDSHIP_LIST),Qt::QueuedConnection);
    connect(this, &ClientRequestHandler::sigEnrollAccountRequest,            businessProcessor,    businessProcessor->getMappingFunction("request")->value(SSDTO::ENROLL_ACCOUNT),Qt::QueuedConnection);
    connect(this, &ClientRequestHandler::sigAddFriendRequest,                businessProcessor,    businessProcessor->getMappingFunction("request")->value(SSDTO::MAKE_FRIEND_REQUEST),Qt::QueuedConnection);
    connect(this, &ClientRequestHandler::sigFuzzySearchRequest,              businessProcessor,    businessProcessor->getMappingFunction("request")->value(SSDTO::FUZZY_SEARCH),Qt::QueuedConnection);
    connect(this, &ClientRequestHandler::sigQueryUserBaseInfoRequest,        businessProcessor,    businessProcessor->getMappingFunction("request")->value(SSDTO::R_USER_BASE_INFO),Qt::QueuedConnection);
    connect(this, &ClientRequestHandler::sigQueryGroupBaseInfoRequest,       businessProcessor,    businessProcessor->getMappingFunction("request")->value(SSDTO::R_GROUP_BASE_INFO),Qt::QueuedConnection);
    connect(this, &ClientRequestHandler::sigQueryGroupNoticesRequest,        businessProcessor,    businessProcessor->getMappingFunction("request")->value(SSDTO::R_GROUP_NOTICE),Qt::QueuedConnection);
    connect(this, &ClientRequestHandler::sigQueryNewMessageRequest,          businessProcessor,    businessProcessor->getMappingFunction("request")->value(SSDTO::R_MESSAGE_CONTENT),Qt::QueuedConnection);
    connect(this, &ClientRequestHandler::sigQueryFileInfoRequest,            businessProcessor,    businessProcessor->getMappingFunction("request")->value(SSDTO::R_FILE),Qt::QueuedConnection);
    connect(this, &ClientRequestHandler::sigQueryMsgPicInfoRequest,          businessProcessor,    businessProcessor->getMappingFunction("request")->value(SSDTO::R_MESSAGE_PIC_INFO),Qt::QueuedConnection);

    // 连接响应信号槽
    connect(businessProcessor, &BusinessLayer::BusinessProcessor::sigEmailCodeResponse,              this, &ClientRequestHandler::sigEmailCodeResponse);
    connect(businessProcessor, &BusinessLayer::BusinessProcessor::sigLoginResult,                    this, &ClientRequestHandler::sigLoginResult);
    connect(businessProcessor, &BusinessLayer::BusinessProcessor::sigForwardMessageResponse,         this, &ClientRequestHandler::sigForwardMessageResponse);
    connect(businessProcessor, &BusinessLayer::BusinessProcessor::sigContactListResponse,            this, &ClientRequestHandler::sigContactListResponse);
    connect(businessProcessor, &BusinessLayer::BusinessProcessor::sigEnrollAccountResponse,          this, &ClientRequestHandler::sigEnrollAccountResponse);
    connect(businessProcessor, &BusinessLayer::BusinessProcessor::sigFriendRequestResponse,          this, &ClientRequestHandler::sigFriendRequestResponse);
    connect(businessProcessor, &BusinessLayer::BusinessProcessor::sigFuzzySearchResponse,            this, &ClientRequestHandler::sigFuzzySearchResponse);
    connect(businessProcessor, &BusinessLayer::BusinessProcessor::sigConnServerFailed,               this, &ClientRequestHandler::sigConnServerFailed);
    connect(businessProcessor, &BusinessLayer::BusinessProcessor::sigStartGRPCService,               this, &ClientRequestHandler::sigStartGRPCService);
    connect(businessProcessor, &BusinessLayer::BusinessProcessor::sigQueryUserBaseInfoResponse,      this, &ClientRequestHandler::sigQueryUserBaseInfoResponse);
    connect(businessProcessor, &BusinessLayer::BusinessProcessor::sigQueryGroupBaseInfoResponse,     this, &ClientRequestHandler::sigQueryGroupBaseInfoResponse);
    connect(businessProcessor, &BusinessLayer::BusinessProcessor::sigQueryGroupNoticesResponse,      this, &ClientRequestHandler::sigQueryGroupNoticesResponse);
    connect(businessProcessor, &BusinessLayer::BusinessProcessor::sigQueryNewMessageResponse,        this, &ClientRequestHandler::sigQueryNewMessageResponse);
    connect(businessProcessor, &BusinessLayer::BusinessProcessor::sigQueryFileInfoResponse,          this, &ClientRequestHandler::sigQueryFileInfoResponse);
    connect(businessProcessor, &BusinessLayer::BusinessProcessor::sigQueryMsgPicInfoResponse,        this, &ClientRequestHandler::sigQueryMsgPicInfoResponse);

    // 启动业务线程
    _handlerThread->start();
}

ClientRequestHandler::~ClientRequestHandler() {
    businessProcessor->disConnectFromSer();
    _handlerThread->quit();
    _handlerThread->wait();
    delete businessProcessor;
    delete _handlerThread;
}

BusinessLayer::BusinessProcessor::BusinessProcessor(QObject* parent) : QObject(parent) {
    _ccon = new ClientConServer();

    connect(this, &BusinessProcessor::sigWriteToSocket, _ccon, [=](const QByteArray& data) {
        if(_ccon && _ccon->isConnected()) {
            _ccon->enqueueData(data);
        }
    },Qt::QueuedConnection);

    connect(_ccon->getClientSocket(), &QTcpSocket::connected,this, [=]() {
        if (_ccon && _ccon->isConnected()) {
            LOG("Connected the server : " << _ccon->getServerIP().toStdString());
        }
        else {
            LOG("Can't connect the server : " << _ccon->getServerIP().toStdString());
        }
    });

    // wait 6s check net stable which the net connected the server
    QTimer::singleShot(2000,this, [=]() {
        if (_ccon && !_ccon->isConnected()) {
            emit sigConnServerFailed();
        }
        else {
            // start to emit heart check by grpc
            emit sigStartGRPCService();
        }
    });

    connect(_ccon->getClientSocket(), &QTcpSocket::readyRead, this, [this]() {
        _msgBuffer.append(_ccon->getClientSocket()->readAll());
    
        while (true) {
            // 读取消息头
            if (_expectedSize == -1 && _msgBuffer.size() >= 8) {
                QDataStream headerStream(_msgBuffer);
                headerStream >> _expectedSize;
                headerStream >> _currentType;
                _msgBuffer.remove(0, 8);
                
                // 添加长度校验
                if (_expectedSize < 0 || _expectedSize > 10 * 1024 * 1024) {
                    LOG_ERROR("Invalid message size: " << _expectedSize);
                    _ccon->getClientSocket()->close();
                    return;
                }
            }

            // 读取消息体
            if (_expectedSize != -1 && _msgBuffer.size() >= _expectedSize) {
                QByteArray data = _msgBuffer.left(_expectedSize);
                handleResponse(_currentType, std::string(data.constData(), data.size()));
                
                _msgBuffer.remove(0, _expectedSize);  // 移除已处理的数据
                _expectedSize = -1;  // 重置状态
            } else {
                break;
            }
        }
    });

    // register slot function
    // request slot manager by dto type mapping
    {
        // login
        _requestHandlerMap[SSDTO::LOGIN_CHECK] = [=](const std::string & dto) {
            SEND_PACKAGE(dto,SSDTO::LOGIN_CHECK,"login dto has been send to server...")
        };

        // store msg
        _requestHandlerMap[SSDTO::C_MESSAGE_CONTENT] = [=](const std::string & dto) {
            SEND_PACKAGE(dto,SSDTO::C_MESSAGE_CONTENT,"msg_forward dto has been send to server...")
        };

        // disconnect by user
        _requestHandlerMap[SSDTO::DISCONNECT] = [=](const std::string & dto) {
            SEND_PACKAGE(dto,SSDTO::DISCONNECT,"disconnect notice has been send to server...")
        };

        // query contact list
        _requestHandlerMap[SSDTO::R_FRIENDSHIP_LIST] = [=](const std::string & dto) {
            SEND_PACKAGE(dto,SSDTO::R_FRIENDSHIP_LIST,"query of contact list dto has been send to server...")
        };

        // query email code
        _requestHandlerMap[SSDTO::EMAIL_VERIFY] = [=](const std::string & dto) {
            SEND_PACKAGE(dto,SSDTO::EMAIL_VERIFY,"query of email code dto has been send to server...")
        };

        // enroll account
        _requestHandlerMap[SSDTO::ENROLL_ACCOUNT] = [=](const std::string & dto) {
            SEND_PACKAGE(dto,SSDTO::ENROLL_ACCOUNT,"query of enroll dto has been send to server...")
        };

        // add friendship
        _requestHandlerMap[SSDTO::MAKE_FRIEND_REQUEST] = [=](const std::string & dto) {
            SEND_PACKAGE(dto,SSDTO::MAKE_FRIEND_REQUEST,"query of add friend dto has been send to server...")
        };

        // query search request
        _requestHandlerMap[SSDTO::FUZZY_SEARCH] = [=](const std::string & dto) {
            SEND_PACKAGE(dto,SSDTO::FUZZY_SEARCH,"query of search fuzzy dto has been send to server...")
        };

        // query user base info request
        _requestHandlerMap[SSDTO::R_USER_BASE_INFO] = [=](const std::string & dto) {
            SEND_PACKAGE(dto,SSDTO::R_USER_BASE_INFO,"query of user base info dto has been send to server...")
        };

        // query group base info request
        _requestHandlerMap[SSDTO::R_GROUP_BASE_INFO] = [=](const std::string & dto) {
            SEND_PACKAGE(dto,SSDTO::R_GROUP_BASE_INFO,"query of group base info dto has been send to server...")
        };

        // query group notice info request
        _requestHandlerMap[SSDTO::R_GROUP_NOTICE] = [=](const std::string & dto) {
            SEND_PACKAGE(dto,SSDTO::R_GROUP_NOTICE,"query of group notices dto has been send to server...")
        };

        // query message content
        _requestHandlerMap[SSDTO::R_MESSAGE_CONTENT] = [=](const std::string & dto) {
            SEND_PACKAGE(dto,SSDTO::R_MESSAGE_CONTENT,"query of msg content dto has been send to server...")
        };

        // query file info
        _requestHandlerMap[SSDTO::R_FILE] = [=](const std::string & dto) {
            SEND_PACKAGE(dto,SSDTO::R_FILE,"query of file info dto has been send to server...")
        };

        // query msg pic info
        _requestHandlerMap[SSDTO::R_MESSAGE_PIC_INFO] = [=](const std::string & dto) {
            SEND_PACKAGE(dto,SSDTO::R_MESSAGE_PIC_INFO,"query of pic embedding msg dto has been send to server...")
        };
    }

    // response slot manager by dto type mapping
    {
        // login
        _responseHandlerMap[SSDTO::LOGIN_CHECK] = [=](const std::string & dto) {
            LOG("login response")
            emit sigLoginResult(dto);
        };

        // forward msg
        _responseHandlerMap[SSDTO::C_MESSAGE_CONTENT] = [=](const std::string & dto) {
            LOG("forward response")
            emit sigForwardMessageResponse(dto);
        };

        // friendship
        _responseHandlerMap[SSDTO::R_FRIENDSHIP_LIST] = [=](const std::string & dto) {
            LOG("friendship response")
            emit sigContactListResponse(dto);
        };

        // email code
        _responseHandlerMap[SSDTO::EMAIL_VERIFY] = [=](const std::string & dto) {
            LOG("email response")
            emit sigEmailCodeResponse(dto);
        };

        // enroll account
        _responseHandlerMap[SSDTO::ENROLL_ACCOUNT] = [=](const std::string & dto) {
            LOG("enroll response")
            emit sigEnrollAccountResponse(dto);
        };

        // user make friend response
        _responseHandlerMap[SSDTO::MAKE_FRIEND_RESPONSE] = [=](const std::string & dto) {
            LOG("make friend response")
            emit sigFriendRequestResponse(dto,false);
        };

        // other user ask for be your friend
        _responseHandlerMap[SSDTO::MAKE_FRIEND_REQUEST] = [=](const std::string & dto) {
            LOG("make friend response")
            emit sigFriendRequestResponse(dto,true);
        };

        // search fuzzy
        _responseHandlerMap[SSDTO::FUZZY_SEARCH] = [=](const std::string & dto) {
            LOG("search fuzzy response")
            emit sigFuzzySearchResponse(dto);
        };

        // query user base info request
        _responseHandlerMap[SSDTO::R_USER_BASE_INFO] = [=](const std::string & dto) {
            LOG("get user base info response")
            emit sigQueryUserBaseInfoResponse(dto);
        };

        // query group base info request
        _responseHandlerMap[SSDTO::R_GROUP_BASE_INFO] = [=](const std::string & dto) {
            LOG("get group base info response")
            emit sigQueryGroupBaseInfoResponse(dto);
        };

        // query group notice info request
        _responseHandlerMap[SSDTO::R_GROUP_NOTICE] = [=](const std::string & dto) {
            LOG("get group notice info response")
            emit sigQueryGroupNoticesResponse(dto);
        };

        // query message content
        _responseHandlerMap[SSDTO::R_MESSAGE_CONTENT] = [=](const std::string & dto) {
            LOG("get message content response")
            emit sigQueryNewMessageResponse(dto);
        };

        // file info
        _responseHandlerMap[SSDTO::R_FILE] = [=](const std::string & dto) {
            LOG("get file response")
            emit sigQueryFileInfoResponse(dto);
        };

        // query msg pic info
        _responseHandlerMap[SSDTO::R_MESSAGE_PIC_INFO] = [=](const std::string & dto) {
            LOG("get msg pic response")
            emit sigQueryMsgPicInfoResponse(dto);
        };
    }

    if (_ccon && !_ccon->isConnected()) {
        _ccon->connToSer();
    }
}

BusinessLayer::BusinessProcessor::~BusinessProcessor() {
    _ccon->getClientSocket()->disconnectFromHost();
    delete _ccon;
}

void BusinessLayer::BusinessProcessor::handleResponse(SSDTO::BusinessType type, const std::string &dto) {
    switch (type) {
        // case SSDTO::FILE_TRANSFER_REQUEST:
        //     // _pool.enqueue(new FileTransferTask(dto));  // 提交到文件传输线程池
        //     break;
        // // 其他任务类型
        default:
            _responseHandlerMap[type](dto);// 默认处理方式
            break;
    }
}

void BusinessLayer::BusinessProcessor::disConnectFromSer(){
    // out line notice
    std::string outDisDto;
    SSDTO::DisconnectDTO ddto;
    ddto.set_ssid(g_pCommonData->getCurUserInfo().ssid.toStdString());
    ddto.set_ip("");
    ddto.SerializeToString(&outDisDto);

    _responseHandlerMap[SSDTO::DISCONNECT](outDisDto);
}