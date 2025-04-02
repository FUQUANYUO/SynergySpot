//
// Created by FUQAQ on 2025/1/12.
//

#include "business/download/AsyncDownloadTask.h"
#include "business/upload/SyncUploadTask.h"
#include "yaml-cpp/yaml.h"

#include "help.h"

#include <openssl/md5.h>
#include <fstream>

#include <QDir>
#include <QTimer>
#include <QBuffer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtNetwork/QLocalSocket>
#include <utility>

#include "testWindow.h"


#define SEND_ERROR_RESPONSE(__MSG__)                        \
    resp["status"]  = "error";                              \
    resp["message"] = __MSG__;

RealtimeCommHandler::RealtimeCommHandler(QObject* parent)
    : QObject(parent)
    , _isCallActive(false)
    , _isHeartbeatActive(false)
    , _coolDownTime(30 * 1000)
    , _heartbeatTimer(new QTimer(this))
{
    // initWebRTC();
    setupGRPCChannel();
    setupWebRTCSignaling();

    // 设置心跳定时器
    connect(_heartbeatTimer, &QTimer::timeout, this, &RealtimeCommHandler::sltCheckHeartbeat);

    _heartbeatTimer->setInterval(_coolDownTime);
    _heartbeatTimer->start();
    _isHeartbeatActive = true;

    _pIPCSocket = new QLocalSocket(this);
    _pIPCSocket->connectToServer("SynergySpotIPC");

    if (!_pIPCSocket->waitForConnected()) {
        LOG_ERROR("Failed to connect to client:" << _pIPCSocket->errorString().toStdString());
    }

    connect(_pIPCSocket, &QLocalSocket::readyRead, _pIPCSocket,[&]() {
        QByteArray data = _pIPCSocket->readAll();
        LOG_INFO(data.toStdString())
        processCommand(data);
    });

    connect(this,&RealtimeCommHandler::sigUploadFinished,
            this,&RealtimeCommHandler::sltSendResponse,
            Qt::QueuedConnection);
    connect(this, &RealtimeCommHandler::sigDownloadFinished,
            this, &RealtimeCommHandler::sltSendResponse,
            Qt::QueuedConnection);

    // connect(this, &RealtimeCommHandler::sigUploadProgress,
    //     this, &RealtimeCommHandler::sltUpdateProgress,
    //     Qt::QueuedConnection);
    // connect(this, &RealtimeCommHandler::sigDownloadProgress,
    //     this, &RealtimeCommHandler::sltUpdateProgress,
    //     Qt::QueuedConnection);

    connect(this,&RealtimeCommHandler::sigGRPCDisconnect,this,[=]() {
        QJsonObject resp;
        QString msg = "grpc occur link error, retry to linking << [" +
            QString::number(_tryLinkCount) + "/" + QString::number(_maxRetryCount) + "]";
        SEND_ERROR_RESPONSE(msg)
    });

    connect(this, &RealtimeCommHandler::sigRemoteOfferReceived,
            this, &RealtimeCommHandler::handleRemoteOffer);
    connect(this, &RealtimeCommHandler::sigRemoteAnswerReceived,
            this, &RealtimeCommHandler::handleRemoteAnswer);
    connect(this, &RealtimeCommHandler::sigRemoteIceCandidateReceived,
            this, &RealtimeCommHandler::handleRemoteIceCandidate);

    _cqThread = std::thread([this]() {
        void* tag;
        bool ok;
        while (_cq.Next(&tag, &ok)) {
            auto* task = static_cast<AsyncTask*>(tag);
            task->proceed(ok);
        }
    });
}

RealtimeCommHandler::~RealtimeCommHandler() {
    // 先停止心跳等依赖流的组件
    _heartbeatTimer->stop();
    _isHeartbeatActive = false;

    _streamShutdown = true; // 设置关闭标志
    {
        std::lock_guard<std::mutex> lock(_streamMutex);
        if (_signalingStream) {
            _signalingStream->WritesDone(); // 通知服务端不再发送数据
            _signalingStream->Finish().IgnoreError(); // 安全关闭流
            _signalingStream.reset();
        }
    }
    if (_signalingThread.joinable()) {
        _signalingThread.join(); // 等待信令线程退出
    }

    if (_isCallActive) {
        endVideoCall();
    }
    _shutdown = true;
    _cq.Shutdown();
    if (_cqThread.joinable()) _cqThread.join();
}

int RealtimeCommHandler::startGrpcService() {
    // return QApplication::exec();
    return 0;
}

grpc::CompletionQueue *RealtimeCommHandler::getCompletionQueue() {
    return &_cq;
}
void RealtimeCommHandler::setUserSSID(const QString &ssid) {
    curUserSSID = ssid;
}

QString RealtimeCommHandler::getUserSSID() {
    return curUserSSID;
}

void RealtimeCommHandler::onDownloadSuccess(AsyncDownloadTask *task) {
    QJsonObject resp;
    resp["status"]          = "success";
    resp["type"]            = "file-download";
    resp["file-id"]         = task->_dto.fileId;
    resp["uploader-ssid"]   = task->_dto.uploaderSsid;
    resp["file-name"]       = task->_dto.fileName;
    resp["file-size"]       = task->_dto.fileSize;
    resp["file-type"]       = task->_dto.fileType;
    resp["storage-path"]    = task->_dto.storagePath;
    resp["upload-time"]     = task->_dto.uploadTime;
    resp["business-type"]   = task->_businessType;
    resp["local-path"]      = task->_savePath;
    LOG_INFO("D: ---- file download success!: " << task->_savePath.toStdString() << " ----")
    emit sigDownloadFinished(resp);
}

void RealtimeCommHandler::onDownloadFailed(const std::string &error) {
    QJsonObject resp;
    resp["status"] = "error";
    resp["message"] = QString::fromStdString(error);
    LOG_ERROR("D : occur failed : " << error)
    emit sigDownloadFinished(resp);
}

void RealtimeCommHandler::onUploadSuccess(SyncUploadTask *task) {
    QJsonObject resp;
    resp["status"]       = "success";
    resp["type"]         = "file-upload";
    resp["file-id"]      = task->_dto.fileId;
    resp["uploader-ssid"]= task->_dto.uploaderSsid;
    resp["file-name"]    = task->_dto.fileName;
    resp["file-size"]    = task->_dto.fileSize;
    resp["file-type"]    = task->_dto.fileType;
    resp["storage-path"] = task->_dto.storagePath;
    resp["upload-time"]  = task->_dto.uploadTime;
    resp["business-type"]= task->_type;
    resp["local-path"]   = task->_localUrl;
    LOG_INFO("D: ---- upload success! file id: " << task->_dto.fileId.toStdString() << " ----")
    emit sigUploadFinished(resp);
}

void RealtimeCommHandler::onUploadFailed(const std::string &error) {
    QJsonObject resp;
    resp["status"] = "error";
    resp["message"] = QString::fromStdString(error);
    LOG_ERROR("U : occur failed : " << error)
    emit sigUploadFinished(resp);
}
void RealtimeCommHandler::connectToSignalingServer() {
    _webRTCHandler->initialize();
}

bool RealtimeCommHandler::startVideoCall(const QString &targetSsid) {
    if (_isCallActive) {
        LOG_ERROR("Call already in progress");
        return false;
    }
    _webRTCHandler->createOffer(targetSsid);
    _isCallActive = true;
    return true;
}

void RealtimeCommHandler::endVideoCall() {
    if (!_isCallActive) return;

    SignalingMessage msg;
    msg.set_type(SignalingMessage::HANGUP);
    msg.set_sender_ssid(curUserSSID.toStdString());
    msg.set_target_ssid(_currentRemoteId.toStdString());
    sendSignalingMessage(msg);

    _webRTCHandler->endCall();
    _isCallActive = false;
    emit sigCallStateChanged(0);
}

void RealtimeCommHandler::handleRemoteOffer(const QString &sdp, const QString &senderSsid) {
    _webRTCHandler->initialize();
    _webRTCHandler->handleOffer(sdp, senderSsid);
    _isCallActive = true;
    emit sigCallStateChanged(1);
}

void RealtimeCommHandler::handleRemoteAnswer(const QString &sdp) {
    _webRTCHandler->handleAnswer(sdp, _currentRemoteId);
}

void RealtimeCommHandler::handleRemoteIceCandidate(const QString &candidate, const QString &mid) {
    _webRTCHandler->handleRemoteCandidate(candidate, mid);
}

bool RealtimeCommHandler::isCallActive() {
    return _isCallActive;
}

void RealtimeCommHandler::sltSendResponse(const QJsonObject &resp) {
    if (_pIPCSocket != nullptr) {
        if (_pIPCSocket->state() == QLocalSocket::ConnectedState) {
            _pIPCSocket->write(QJsonDocument(resp).toJson());
            _pIPCSocket->flush();
        } else {
            LOG_ERROR("_pIPCSocket is not connected")
        }
    } else {
        LOG_ERROR("When program run find _pIPCSocket is nullptr");
    }
}

void RealtimeCommHandler::sltSendVideoFrame(const QImage &frame) {
    if (!_isCallActive || !_webRTCHandler) {
        return;
    }

    try {
        // 将QImage转换为压缩的JPEG字节数组（减少传输数据量）
        QByteArray imageData;
        QBuffer buffer(&imageData);
        buffer.open(QIODevice::WriteOnly);
        frame.save(&buffer, "JPEG", 80); // 80%质量

        // 将图像数据转换为Base64编码的字符串
        QString base64Image = QString::fromLatin1(imageData.toBase64());

        // 创建JSON消息
        QJsonObject videoFrameMsg;
        videoFrameMsg["type"] = "video-frame";
        videoFrameMsg["width"] = frame.width();
        videoFrameMsg["height"] = frame.height();
        videoFrameMsg["data"] = base64Image;

        // 通过数据通道发送
        QJsonDocument doc(videoFrameMsg);
        _webRTCHandler->sendVideoFrame(doc.toJson(QJsonDocument::Compact).toStdString());

    } catch (const std::exception &e) {
        LOG_ERROR("Failed to send video frame: " << e.what());
    }
}

void RealtimeCommHandler::setupGRPCChannel() {
    std::string yamlPath = g_pCommonData->getYamlPath();
    YAML::Node conf = YAML::LoadFile(yamlPath);
    if(!conf.IsNull()){
        std::string ip = conf["realtime-info"]["grpcIP"].as<std::string>();
        std::string port = conf["realtime-info"]["grpcPort"].as<std::string>();
        _maxRetryCount = conf["realtime-info"]["maxRetryCount"].as<int>();
        _channel = grpc::CreateChannel(ip + ":" + port, grpc::InsecureChannelCredentials());
        _mediaStub = MediaService::NewStub(_channel);
        _fileStub = FileTransferService::NewStub(_channel);
        _signalingStub = WebRTCSignalingService::NewStub(_channel);
    }
    else{
        LOG_INFO("set up grpc channel failed! conf is null, please check path! ")
    }
}

void RealtimeCommHandler::processCommand(const QString &line)  {
    QJsonDocument doc = QJsonDocument::fromJson(line.toUtf8());
    if (!doc.isNull()) {
        QJsonObject cmd = doc.object();
        QString command = cmd["command"].toString();
        if (command == "upload") {
            handleUploadCommand(cmd["local-path"].toString(),cmd["business-type"].toString(),{
                cmd["file-id"].toString(),cmd["uploader-ssid"].toString(),
                "",-1,"",""
                }
            );
        }
        else if (command == "download") {
            handleDownloadCommand(cmd["local-path"].toString(),cmd["business-type"].toString(),{
                    cmd["file-id"].toString(),cmd["uploader-ssid"].toString(),
                    "",-1,"",
                    cmd["storage-path"].toString(),
                }
            );
        }
        else if (command == "set") {
            curUserSSID = cmd["ssid"].toString();
            g_pCommonData->setCurUserInfo({curUserSSID});
        }
    }
}

void RealtimeCommHandler::handleUploadCommand(const QString &localUrl,const QString& type, FileStorageDTO fileDTO) {
    try {
        std::unique_ptr<SyncUploadTask> _upload(new SyncUploadTask{this,std::move(_fileStub),localUrl,type,std::move(fileDTO)});
        _upload->execute();
    }catch (std::exception &e) {
        onUploadFailed(e.what());
    }
}

void RealtimeCommHandler::handleDownloadCommand(const QString &saveLocPath, const QString &type, const FileStorageDTO &fileDTO) {
    try {
        auto stub = FileTransferService::NewStub(_channel);
        new AsyncDownloadTask(this, std::move(stub), fileDTO, saveLocPath, type);
    } catch (std::exception &e) {
        onDownloadFailed(e.what());
    }
}
void RealtimeCommHandler::setupWebRTCSignaling() {
    _webRTCHandler = std::make_unique<WebRTCHandler>(this);

    connect(_webRTCHandler.get(), &WebRTCHandler::sigLocalDescriptionCreated,
                [=](const QString& sdp, const QString& type) {
                    SignalingMessage msg;
                    msg.set_sender_ssid(curUserSSID.toStdString());
                    msg.set_target_ssid(_currentRemoteId.toStdString());
                    msg.set_content(sdp.toStdString());
                    msg.set_type(type == "offer" ? SignalingMessage::OFFER : SignalingMessage::ANSWER);
                    sendSignalingMessage(msg);
                });

    connect(_webRTCHandler.get(), &WebRTCHandler::sigIceCandidateFound,
            [this](const QString& candidate, const QString& mid) {
                SignalingMessage msg;
                msg.set_type(SignalingMessage::ICE_CANDIDATE);
                msg.set_sender_ssid(curUserSSID.toStdString());
                msg.set_target_ssid(_currentRemoteId.toStdString());
                msg.set_content(candidate.toStdString());
                msg.set_sdp_mid(mid.toStdString());
                sendSignalingMessage(msg);
            });

    connect(_webRTCHandler.get(), &WebRTCHandler::sigDataChannelPicReceived,
        this, [this](const QImage &frame) {
            emit sigRemoteVideoFrameReceived(frame);
        });

    std::lock_guard<std::mutex> lock(_streamMutex);
    if (_signalingStream) {
        _signalingStream->WritesDone();
        _signalingStream->Finish().IgnoreError();
        _signalingStream.reset();
    }

    _signalingContext = std::make_unique<grpc::ClientContext>();
    _signalingStream = _signalingStub->SignalingStream(_signalingContext.get());
    _isSignalingActive = true;

    // 启动信令处理线程
    _signalingThread = std::thread([this]() {
        processSignalingStream();
    });
}

void RealtimeCommHandler::sendSignalingMessage(const SignalingMessage &message) {
    if (_signalingStream && !_streamShutdown) {
        if (!_signalingStream->Write(message)) {
            LOG_ERROR("发送信令消息失败");
            _streamShutdown = true; // 触发流关闭
        }else {
            LOG_INFO("信令消息发送成功: " << message.content());
        }
    }
}

void RealtimeCommHandler::processSignalingStream() {
    // 确保信令流已初始化
    {
        std::lock_guard<std::mutex> lock(_streamMutex);
        if (!_signalingStream) {
            LOG_ERROR("Signaling stream is not initialized");
            return;
        }
    }

    // 持续读取信令流中的消息
    while (!_streamShutdown) {
        SignalingMessage msg;
        {
            std::lock_guard<std::mutex> lock(_streamMutex);
            if (!_signalingStream || !_signalingStream->Read(&msg)) {
                if (_signalingStream) {
                    grpc::Status status = _signalingStream->Finish();
                    if (!status.ok()) {
                        LOG_ERROR("信令流错误: " << status.error_message());
                    }
                    _signalingStream.reset();
                }
                _isSignalingActive = false;
                break; // 流正常结束
            }
        }
        // 根据消息类型触发信号
        switch (msg.type()) {
            case SignalingMessage::OFFER:
                emit sigRemoteOfferReceived(
                    QString::fromStdString(msg.content()),
                    QString::fromStdString(msg.sender_ssid())
                );
            break;
            case SignalingMessage::ANSWER:
                emit sigRemoteAnswerReceived(
                    QString::fromStdString(msg.content())
                );
            break;
            case SignalingMessage::ICE_CANDIDATE:
                emit sigRemoteIceCandidateReceived(
                    QString::fromStdString(msg.content()),
                    QString::fromStdString(msg.sdp_mid())
                );
            break;
            case SignalingMessage::HANGUP:
                endVideoCall();
            break;
            default:
                LOG_WARNING("Unknown signaling message type: " << msg.type());
            break;
        }
    }
}

QString RealtimeCommHandler::calculateChunkMD5(const QByteArray &data) {
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5((const unsigned char*)data.constData(), data.size(), digest);

    char mdStr[33];
    for(int i=0; i<16; i++)
        snprintf(&mdStr[i*2], 3, "%02x", (unsigned int)digest[i]);

    return QString(mdStr);
}

void RealtimeCommHandler::sltCheckHeartbeat() {
    // 发送心跳包到服务器
    if (_isHeartbeatActive) {
        HeartbeatRequest request;
        request.set_client_id(g_pCommonData->getCurUserInfo().ssid.toStdString());
        request.set_timestamp(GetCurTime::getTimeObj()->getCurTimeStamp());

        HeartbeatResponse response;
        grpc::ClientContext context;

        // 调用 gRPC 服务
        grpc::Status status = _mediaStub->HeartbeatHandler(&context, request, &response);

        // 处理响应
        if (status.ok()) {
            bool alive = response.alive();
            _tryLinkCount = 0;
            LOG_INFO("Heartbeat response: alive = " << alive)
        } else {
            _tryLinkCount++;
            LOG_INFO("RPC failed:" << status.error_message())
            emit sigGRPCDisconnect();
            if (_tryLinkCount > _maxRetryCount) {
                LOG_INFO("the progress over cause of try count has exceeded max retry count")
                _isHeartbeatActive = false;
                QCoreApplication::exit(1);
            }
        }
    }
}