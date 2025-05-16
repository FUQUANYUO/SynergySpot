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
#include <ela-widget-tools/ElaMessageBar.h>

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
    _pIPCSocket->connectToServer("SynergySpotIPC-" + g_pCommonData->getCurUserInfo().ssid);

    _timer      = new QTimer(this);

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

    for (int i = 0; i < 4; ++i) {
        _workers.emplace_back([this]() {
            void* tag;
            bool ok;
            while (!_shutdown && _cq.Next(&tag, &ok)) {
                auto* task = static_cast<AsyncTask*>(tag);
                task->proceed(ok);
            }
        });
    }
}

RealtimeCommHandler::~RealtimeCommHandler() {
    if (_isCallActive) {
        endVideoCall();
    }
    _shutdown = true;
    _cq.Shutdown();
    for (auto& thread : _workers) {
        if (thread.joinable()) thread.join();
    }
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
            // add char '\t\n' to divided json info
            QByteArray data = QJsonDocument(resp).toJson();
            data.append("\t\n");
            _pIPCSocket->write(data);
            _pIPCSocket->flush();
        } else {
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
            if (cmd["file-id"].toString().isEmpty() || cmd["file-id"].toString() == "-1") {
                LOG_WARNING("file-id is -1 skip down request")
                return;
            }

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
        auto stub = FileTransferService::NewStub(_channel);
        std::unique_ptr<SyncUploadTask> _upload(new SyncUploadTask{this,std::move(stub),localUrl,type,std::move(fileDTO)});
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
    _timer->stop();

    if (_videoAudioCallPage) {
        _videoAudioCallPage->deleteLater();
        _videoAudioCallPage = nullptr;
    }
    if (_videoAudioInvitePage) {
        _videoAudioInvitePage->deleteLater();
        _videoAudioInvitePage = nullptr;
    }

    // init call video page
    if (_videoAudioInvitePage == nullptr) {
        disconnect(_videoAudioInvitePage, 0, this, 0);
        _videoAudioInvitePage = new VideoAudioInvitePage(g_pCommonData->getCurUserInfo().ssid,remoteId,!isOtherUserInvite);
    }
    _videoAudioInvitePage->show();
    _videoAudioInvitePage->raise();

    if (!isOtherUserInvite) {
        auto curInfo = g_pCommonData->getCurUserInfo();
        _videoAudioCallPage = new VideoAudioCallPage(
                curInfo.ssid,remoteId,curInfo.ssid.toInt(),userSig);
        _videoAudioCallPage->setUserSig(userSig);
        _videoAudioCallPage->setMyName(curInfo.username.toStdString());
        _videoAudioCallPage->hide();

        connect(_videoAudioCallPage,&VideoAudioCallPage::sigVideoHangUp,this,[=]() {
            _videoAudioCallPage->hide();
            _videoAudioCallPage->deleteLater();
            _videoAudioCallPage = nullptr;

            _timer->stop();

            _isCallActive = false;
        });

        connect(_videoAudioCallPage,&VideoAudioCallPage::sigRemoteUserEnterRoom,this,[=](std::string userId) {
            _videoAudioInvitePage->hide();
            _videoAudioInvitePage->deleteLater();
            _videoAudioInvitePage = nullptr;

            _timer->stop();

            _videoAudioCallPage->show();
            _videoAudioCallPage->raise();
            _isCallActive = true;
        });

        connect(_videoAudioCallPage,&VideoAudioCallPage::sigRemoteUserLeaveRoom,this,[=]() {
            _videoAudioCallPage->hide();
            _videoAudioCallPage->deleteLater();
            _videoAudioCallPage = nullptr;

            _isCallActive = false;
        });

        connect(_timer,&QTimer::timeout,this,[=]() {
            _videoAudioInvitePage->setHangUpBtnEnable(false);
            ElaMessageBar::warning(ElaMessageBarType::Top, "无人响应", "对方暂时无法接听!", 3000, _videoAudioInvitePage);
            _videoAudioCallPage->exitRoom();
            _videoAudioCallPage->deleteLater();
            _videoAudioCallPage = nullptr;

            _isCallActive = false;

            QTimer::singleShot(5000,this,[=] {
                _timer->stop();
                _videoAudioInvitePage->hide();
                _videoAudioInvitePage->deleteLater();
                _videoAudioInvitePage = nullptr;
            });
        });
        _timer->start(30000);// 30s等待
    }

    connect(_videoAudioInvitePage,&VideoAudioInvitePage::sigUserProcessResult,this,[=](bool isAccept) {
        if (isAccept) {
            _videoAudioInvitePage->setHangUpBtnEnable(false);
            auto curInfo = g_pCommonData->getCurUserInfo();
            _videoAudioCallPage = new VideoAudioCallPage(
                curInfo.ssid,remoteId,(!isOtherUserInvite)?curInfo.ssid.toInt():remoteId.toInt(),userSig);
            _videoAudioCallPage->setUserSig(userSig);
            _videoAudioCallPage->show();
            _videoAudioCallPage->raise();

            connect(_videoAudioCallPage,&VideoAudioCallPage::sigVideoHangUp,this,[=]() {
                _videoAudioCallPage->hide();
                _videoAudioCallPage->deleteLater();
                _videoAudioCallPage = nullptr;

                _isCallActive = false;
            });

            connect(_videoAudioCallPage,&VideoAudioCallPage::sigRemoteUserLeaveRoom,this,[=]() {
                _videoAudioCallPage->hide();
                _videoAudioCallPage->deleteLater();
                _videoAudioCallPage = nullptr;

                _isCallActive = false;
            });
        }else {
            _timer->stop();
            _videoAudioInvitePage->setHangUpBtnEnable(false);
            _videoAudioCallPage->exitRoom();
            _videoAudioCallPage->deleteLater();
            _videoAudioCallPage = nullptr;

            _isCallActive = false;

            QTimer::singleShot(2000,this,[=] {
               _videoAudioInvitePage->hide();
               _videoAudioInvitePage->deleteLater();
                _videoAudioInvitePage = nullptr;
            });
        }
        _isCallActive = false;
    });
    return 0;
}

void RealtimeCommHandler::endVideoCall() {
    if (!_isCallActive)return ;

    _isCallActive = false;
    if (_videoAudioCallPage)
        _videoAudioCallPage->deleteLater();
    if (_videoAudioInvitePage)
        _videoAudioInvitePage->deleteLater();
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