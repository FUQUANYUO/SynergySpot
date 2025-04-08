//
// Created by FUQAQ on 2025/1/12.
//

#include "business/download/AsyncDownloadTask.h"
#include "business/upload/SyncUploadTask.h"
#include "yaml-cpp/yaml.h"

#include "help.h"
#include "trtc-realtime-comm/video-audio-call-page/VideoAudioCallPage.h"
#include "trtc-realtime-comm/video-audio-call-page/VideoAudioInvitePage.h"

#include <openssl/md5.h>
#include <fstream>

#include <QDir>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtNetwork/QLocalSocket>
#include <utility>

#define SEND_ERROR_RESPONSE(__MSG__)                        \
    resp["status"]  = "error";                              \
    resp["message"] = __MSG__;

RealtimeCommHandler::RealtimeCommHandler(QObject* parent)
    : QObject(parent)
    , _isHeartbeatActive(false)
    , _coolDownTime(30 * 1000)
    , _heartbeatTimer(new QTimer(this))
{
    // initWebRTC();
    setupGRPCChannel();

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
    if (_isCallActive) {
        endVideoCall();
    }
    _shutdown = true;
    _cq.Shutdown();
    if (_cqThread.joinable()) _cqThread.join();
}

int RealtimeCommHandler::startGrpcService() {
    return QCoreApplication::exec();
}

grpc::CompletionQueue *RealtimeCommHandler::getCompletionQueue() {
    return &_cq;
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

void RealtimeCommHandler::onStartVideoFailed(int errorCode) {
    QJsonObject resp;
    resp["status"]       = "error";
    resp["type"]         = "video-call";
    resp["error-code"]   = QString::number(errorCode);
    emit sigUploadFinished(resp);
}

void RealtimeCommHandler::sltSendResponse(const QJsonObject &resp)  {
    if (_pIPCSocket != nullptr) {
        if (_pIPCSocket->state() == QLocalSocket::ConnectedState) {
            _pIPCSocket->write(QJsonDocument(resp).toJson());
            _pIPCSocket->flush();
        }else {
            LOG_ERROR("_pIPCSocket is not connected")
        }
    }
    else {
        LOG_ERROR("When program run find _pIPCSocket is nullptr");
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
        else if (command == "video-call") {
            QString targetSSID     = cmd["target-ssid"].toString();
            bool isOtherUserInvite = cmd["is-other-invite"].toBool();
            QString userSig        = cmd["user-sig"].toString();
            int res = startVideoCall(targetSSID,isOtherUserInvite,userSig);
            if (res != 0) {
                onStartVideoFailed(res);
            }
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

QString RealtimeCommHandler::calculateChunkMD5(const QByteArray &data) {
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5((const unsigned char*)data.constData(), data.size(), digest);

    char mdStr[33];
    for(int i=0; i<16; i++)
        snprintf(&mdStr[i*2], 3, "%02x", (unsigned int)digest[i]);

    return QString(mdStr);
}

int RealtimeCommHandler::startVideoCall(const QString &remoteId,bool isOtherUserInvite,const QString& userSig) {
    if (_isCallActive) {
        if (isOtherUserInvite) {
            LOG_WARNING("some one call again in other call holding! call id: " << remoteId.toStdString());
            return -2;
        }
        LOG_WARNING("you can't call other user when you holding call")
        return -1;
    }
    if (userSig.isEmpty()) {
        LOG_ERROR("user sig is empty")
        return -3;
    }
    _isCallActive = true;
    // init call video page
    _videoAudioInvitePage = std::make_unique<VideoAudioInvitePage>(g_pCommonData->getCurUserInfo().ssid,remoteId,!isOtherUserInvite);

    _videoAudioInvitePage->show();

    connect(_videoAudioInvitePage.get(),&VideoAudioInvitePage::sigUserProcessResult,this,[=](bool isAccept) {
        if (isAccept) {
            auto curInfo = g_pCommonData->getCurUserInfo();
            _videoAudioCallPage = std::make_unique<VideoAudioCallPage>(
                curInfo.ssid,remoteId,(!isOtherUserInvite)?curInfo.ssid.toInt():remoteId.toInt(),userSig);
            _videoAudioCallPage->setUserSig(userSig);
            _videoAudioCallPage->show();

            connect(_videoAudioCallPage.get(),&VideoAudioCallPage::sigVideoHangUp,this,[=]() {
                _videoAudioCallPage->hide();
                _videoAudioCallPage.reset();

                _isCallActive = false;
            });
        }
        _videoAudioInvitePage->hide();
        _videoAudioInvitePage.reset();

        _isCallActive = false;
    });
    return 0;
}

void RealtimeCommHandler::endVideoCall() {
    if (!_isCallActive)return ;

    _isCallActive = false;
    _videoAudioCallPage.reset();
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