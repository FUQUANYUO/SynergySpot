//
// Created by FUQAQ on 2025/1/7.
//

#include "ClientRequestHandler.h"

#include "help.h"

#include <QThread>
#include <QTimer>
#include <functional>

//-----------    proto-file   -----------//
#include "forward_msg/ForwardMsgDTO.pb.h"
#include "disconnect/DisconnectDTO.pb.h"
#include "login/LoginDTO.pb.h"


//-----------      core      -----------//
#include "net-work/ClientConServer.h"

// 发送包装宏
#define SEND_PACKAGE(__DTO_OBJ__, __DTO_TYPE__, __LOG__)    \
    QByteArray packet;                                      \
    QDataStream stream(&packet, QIODevice::WriteOnly);      \
    stream << static_cast<quint32>(__DTO_OBJ__.size());     \
    stream << static_cast<quint8>(__DTO_TYPE__);            \
    packet.append(__DTO_OBJ__.c_str(), __DTO_OBJ__.size()); \
    _ccon->getQSocket()->write(packet);                     \
    _ccon->getQSocket()->waitForBytesWritten();             \
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
            delete _instance;
            _instance = nullptr;
        }
        m.unlock();
    }
}
void ClientRequestHandler::addRequest(SSDTO::Business_Type type, std::string dto) {
    switch (type) {
        case SSDTO::LOGIN:
            emit sigVerifyAccountRequest(dto);
            break;
        case SSDTO::FOWARD_MSG:
            emit sigForwardMessageRequest(dto);
            break;
        case SSDTO::GET_CONTACTLIST:
            emit sigContactListRequest(dto);
            break;
        case SSDTO::GET_EMAILCODE:
            emit sigEmailCodeRequest(dto);
            break;
        case SSDTO::ENROLL:
            emit sigEnrollAccountRequest(dto);
            break;
        case SSDTO::ADD_FRIEND:
            emit sigAddFriendRequest(dto);
            break;
        case SSDTO::FRIEND_SEARCH:
            emit sigSearchFriendRequest(dto);
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
    connect(this, &ClientRequestHandler::sigEmailCodeRequest,       businessProcessor,    businessProcessor->getMappingFunction("request")->value(SSDTO::GET_EMAILCODE));
    connect(this, &ClientRequestHandler::sigVerifyAccountRequest,   businessProcessor,    businessProcessor->getMappingFunction("request")->value(SSDTO::LOGIN));
    connect(this, &ClientRequestHandler::sigForwardMessageRequest,  businessProcessor,    businessProcessor->getMappingFunction("request")->value(SSDTO::FOWARD_MSG));
    connect(this, &ClientRequestHandler::sigContactListRequest,     businessProcessor,    businessProcessor->getMappingFunction("request")->value(SSDTO::GET_CONTACTLIST));
    connect(this, &ClientRequestHandler::sigEnrollAccountRequest,   businessProcessor,    businessProcessor->getMappingFunction("request")->value(SSDTO::ENROLL));
    connect(this, &ClientRequestHandler::sigAddFriendRequest,       businessProcessor,    businessProcessor->getMappingFunction("request")->value(SSDTO::ADD_FRIEND));
    connect(this, &ClientRequestHandler::sigSearchFriendRequest,    businessProcessor,    businessProcessor->getMappingFunction("request")->value(SSDTO::FRIEND_SEARCH));

    // 连接响应信号槽
    connect(businessProcessor, &BusinessLayer::BusinessProcessor::sigEmailCodeResponse,     this, &ClientRequestHandler::sigEmailCodeResponse);
    connect(businessProcessor, &BusinessLayer::BusinessProcessor::sigLoginSuccess,          this, &ClientRequestHandler::sigLoginSuccess);
    connect(businessProcessor, &BusinessLayer::BusinessProcessor::sigLoginFailed,           this, &ClientRequestHandler::sigLoginFailed);
    connect(businessProcessor, &BusinessLayer::BusinessProcessor::sigMessageResponse,       this, &ClientRequestHandler::sigMessageResponse);
    connect(businessProcessor, &BusinessLayer::BusinessProcessor::sigContactListResponse,   this, &ClientRequestHandler::sigContactListResponse);
    connect(businessProcessor, &BusinessLayer::BusinessProcessor::sigEnrollAccountResponse, this, &ClientRequestHandler::sigEnrollAccountResponse);
    connect(businessProcessor, &BusinessLayer::BusinessProcessor::sigFriendRequestResponse, this, &ClientRequestHandler::sigFriendRequestResponse);
    connect(businessProcessor, &BusinessLayer::BusinessProcessor::sigSearchFriendResponse,  this, &ClientRequestHandler::sigSearchFriendResponse);
    connect(businessProcessor, &BusinessLayer::BusinessProcessor::sigConnServerFailed,      this, &ClientRequestHandler::sigConnServerFailed);
    connect(businessProcessor, &BusinessLayer::BusinessProcessor::sigStartGRPCService,      this, &ClientRequestHandler::sigStartGRPCService);
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
    connect(_ccon->getQSocket(), &QTcpSocket::connected,this, [=]() {
        if (_ccon->getQSocket()->state() == QTcpSocket::ConnectedState) {
            LOG("Connected the server : " << _ccon->getServerIP().toStdString());
        }
        else {
            LOG("Can't connect the server : " << _ccon->getServerIP().toStdString());
        }
    });

    // wait 6s check net stable which the net connected the server
    QTimer::singleShot(6000,this, [=]() {
        if (_ccon->getQSocket()->state() != QAbstractSocket::ConnectedState) {
            emit sigConnServerFailed();
        }
        else {
            // start to emit heart check by grpc
            emit sigStartGRPCService();
        }
    });

    connect(_ccon->getQSocket(), &QTcpSocket::readyRead, this, [this]() {
        while (_ccon->getQSocket()->bytesAvailable() >= 8) {
            QByteArray array = _ccon->getQSocket()->read(4);
            int msgSize = 0;
            QDataStream sizeStream(&array, QIODevice::ReadOnly);
            sizeStream >> msgSize;

            array = _ccon->getQSocket()->read(4);
            QDataStream typeStream(&array, QIODevice::ReadOnly);
            SSDTO::Business_Type type;
            typeStream >> type;

            if (_ccon->getQSocket()->bytesAvailable() >= msgSize) {
                QByteArray dataArr = _ccon->getQSocket()->read(msgSize);
                std::string dto(dataArr.constData(),dataArr.size());
                handleResponse(type, dto);
            }else {
                LOG("current socket cache maybe has trouble , the bytesAvailable less than msgSize")
                break;
            }
        }
    });

    // register slot function
    // request slot manager by dto type mapping
    {
        // login
        _requestHandlerMap[SSDTO::LOGIN] = [=](const std::string & dto) {
            SEND_PACKAGE(dto,SSDTO::LOGIN,"login dto has been send to server...")
        };

        // forward msg
        _requestHandlerMap[SSDTO::FOWARD_MSG] = [=](const std::string & dto) {
            SEND_PACKAGE(dto,SSDTO::FOWARD_MSG,"msg_forward dto has been send to server...")
        };

        // disconnect by user
        _requestHandlerMap[SSDTO::DISCONNECT] = [=](const std::string & dto) {
            SEND_PACKAGE(dto,SSDTO::DISCONNECT,"disconnect notice has been send to server...")
        };

        // query contact list
        _requestHandlerMap[SSDTO::GET_CONTACTLIST] = [=](const std::string & dto) {
            SEND_PACKAGE(dto,SSDTO::GET_CONTACTLIST,"query of contact list dto has been send to server...")
        };

        // query email code
        _requestHandlerMap[SSDTO::GET_EMAILCODE] = [=](const std::string & dto) {
            SEND_PACKAGE(dto,SSDTO::GET_EMAILCODE,"query of email code dto has been send to server...")
        };

        // enroll account
        _requestHandlerMap[SSDTO::ENROLL] = [=](const std::string & dto) {
            SEND_PACKAGE(dto,SSDTO::ENROLL,"query of enroll dto has been send to server...")
        };

        // add friend
        _requestHandlerMap[SSDTO::ADD_FRIEND] = [=](const std::string & dto) {
            SEND_PACKAGE(dto,SSDTO::ADD_FRIEND,"query of add friend dto has been send to server...")
        };

        // query search request
        _requestHandlerMap[SSDTO::FRIEND_SEARCH] = [=](const std::string & dto) {
            SEND_PACKAGE(dto,SSDTO::FRIEND_SEARCH,"query of search friend dto has been send to server...")
        };
    }

    // response slot manager by dto type mapping
    {
        // login
        _responseHandlerMap[SSDTO::LOGIN] = [=](const std::string & dto) {
            LOG("recv server verify [" << CurSSID << "] res")
            SSDTO::Login_DTO ldto;
            ldto.ParseFromString(dto);
            if(ldto.is_pass()){
                CurSSname = ldto.ssname();
                emit sigLoginSuccess();
            }
            else{
                emit sigLoginFailed();
            }
        };

        // forward msg
        _responseHandlerMap[SSDTO::FOWARD_MSG] = [=](const std::string & dto) {
            emit sigMessageResponse(dto);
        };

        // forward msg
        _responseHandlerMap[SSDTO::GET_CONTACTLIST] = [=](const std::string & dto) {
            emit sigContactListResponse(dto);
        };

        // email code
        _responseHandlerMap[SSDTO::GET_EMAILCODE] = [=](const std::string & dto) {
            emit sigEmailCodeResponse(dto);
        };

        // enroll account
        _responseHandlerMap[SSDTO::ENROLL] = [=](const std::string & dto) {
            emit sigEnrollAccountResponse(dto);
        };

        // other friend request
        _responseHandlerMap[SSDTO::ADD_FRIEND] = [=](const std::string & dto) {
            emit sigFriendRequestResponse(dto);
        };

        // search friend
        _responseHandlerMap[SSDTO::FRIEND_SEARCH] = [=](const std::string & dto) {
            emit sigSearchFriendResponse(dto);
        };
    }

    if (_ccon->getQSocket()->state() != QAbstractSocket::ConnectedState) {
        _ccon->connToSer();
    }
}

BusinessLayer::BusinessProcessor::~BusinessProcessor() {
    _ccon->getQSocket()->disconnectFromHost();
    delete _ccon;
}

void BusinessLayer::BusinessProcessor::handleResponse(SSDTO::Business_Type type, const std::string &dto) {
    switch (type) {
        case SSDTO::FILE_TRANSFER_REQUEST:
            // _pool.enqueue(new FileTransferTask(dto));  // 提交到文件传输线程池
            break;
        case SSDTO::HEART_BEAT:
            break;
        // 其他任务类型
        default:
            _responseHandlerMap[type](dto);// 默认处理方式
            break;
    }
}

void BusinessLayer::BusinessProcessor::disConnectFromSer(){
    // out line notice
    std::string outDisDto;
    SSDTO::Disconnect_DTO ddto;
    ddto.set_ssid(CurSSID);
    ddto.set_ip("");
    ddto.set_type(SSDTO::DISCONNECT);
    ddto.SerializeToString(&outDisDto);

    _responseHandlerMap[SSDTO::DISCONNECT](outDisDto);
}