//
// Created by FUQAQ on 2025/1/12.
//

#include "RealtimeCommHandler.h"
#include "yaml-cpp/yaml.h"

#include "help.h"

#include <openssl/md5.h>
#include <fstream>

#include "file-transfer/FileTransferDTO.grpc.pb.h"

#include <QDir>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocalSocket>

#define SEND_ERROR_RESPONSE(__MSG__)                \
    resp["status"]  = "error";                      \
    resp["message"] = __MSG__;

#define UPLOAD_FILE_RES(__CONTENT__,__SUCCESS__)    \
    QJsonObject resp;                               \
    if(__SUCCESS__){                                \
        resp["status"]  = "success";                \
        resp["type"]    = "file-upload";            \
        resp["file-id"] = __CONTENT__;              \
    }else {                                         \
        SEND_ERROR_RESPONSE(__CONTENT__)            \
    }                                               \
    emit sigUploadFinished(resp);

#define DOWNLOAD_FILE_RES(__CONTENT__,__SUCCESS__)  \
    QJsonObject resp;                               \
    if(__SUCCESS__){                                \
        resp["status"]      = "success";            \
        resp["type"]        = "file-download";      \
        resp["local-path"]  = __CONTENT__;          \
    }else {                                         \
        SEND_ERROR_RESPONSE(__CONTENT__)            \
    }                                               \
    emit sigDownloadFinished(resp);

namespace RealtimeComm {
    RealtimeCommHandler::RealtimeCommHandler(QObject* parent)
        : QObject(parent)
        , _isCallActive(false)
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
            LOG_ERROR("Failed to connect to server:" << _pIPCSocket->errorString().toStdString());
        }

        connect(_pIPCSocket, &QLocalSocket::readyRead, _pIPCSocket,[&]() {
            QByteArray data = _pIPCSocket->readAll();
            LOG_INFO(data.toStdString())
            processCommand(data);
        });

        connect(this,&RealtimeCommHandler::sigUploadFinished,
                this,&RealtimeCommHandler::sltSendResponse);
        connect(this,&RealtimeCommHandler::sigDownloadFinished,
                this,&RealtimeCommHandler::sltSendResponse);

        g_pCommonData->initCurUserInfoDir();
    }

    RealtimeCommHandler::~RealtimeCommHandler() {
        // if (_isCallActive) {
        //     endVideoCall();
        // }
    }

    int RealtimeCommHandler::startGrpcService() {
        return QCoreApplication::exec();
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
                handleUploadCommand(cmd["local-url"].toString());
            } else if (command == "download") {
                handleDownloadCommand(
                           cmd["file-id"].toString(),
                         cmd["file-path"].toString(),
                        cmd["save-path"].toString());
            }
        }
    }

    void RealtimeCommHandler::handleUploadCommand(const QString &localUrl) {
        QFile localFile(localUrl);

        if (!localFile.open(QIODevice::ReadOnly)) {
            LOG_ERROR("U: cant open local file: " << localUrl.toStdString())
            return;
        }

        FileInitRequest initRequest;
        initRequest.set_file_name(QFileInfo(localUrl).fileName().toStdString());
        initRequest.set_file_size(localFile.size());
        initRequest.set_ssid(g_pCommonData->getCurUserInfo().ssid.toStdString());

        grpc::ClientContext initContext;
        FileInitResponse initResponse;
        grpc::Status initStatus = _fileStub->InitUpload(&initContext, initRequest, &initResponse);

        if (!initStatus.ok() || !initResponse.success()) {
            LOG_ERROR("U: file transfer init file: " << initStatus.error_message())
            UPLOAD_FILE_RES("grpc server cant offer",false)
            return;
        }
        std::string fileId = initResponse.file_id();
        LOG_INFO("U: ---- begin upload file: " << fileId << " ----")

        grpc::ClientContext uploadContext;
        FileResponse uploadResponse;
        std::unique_ptr<grpc::ClientWriter<FileChunk>> writer(
            _fileStub->UploadFile(&uploadContext, &uploadResponse));

        const qint64 totalSize = localFile.size();
        const int totalChunks = (totalSize + CHUNK_SIZE - 1) / CHUNK_SIZE;
        int currentChunk = 0;
        bool uploadSuccess = true;

        while (!localFile.atEnd() && uploadSuccess) {
            QByteArray chunkData = localFile.read(CHUNK_SIZE);

            // 构建分块请求
            FileChunk chunk;
            chunk.set_file_id(fileId);
            chunk.set_chunk_number(currentChunk);
            chunk.set_data(chunkData.constData(), chunkData.size());
            chunk.set_checksum(calculateChunkMD5(chunkData).toStdString());

            // 发送分块
            if (!writer->Write(chunk)) {
                LOG_ERROR("U: failed to write file chunk: " << currentChunk)
                uploadSuccess = false;
                break;
            }

            // 更新进度
            currentChunk++;
            int progress = static_cast<int>(currentChunk * 100.0 / totalChunks);
            emit sigUploadProgress(progress);
        }

        // 完成上传
        writer->WritesDone();
        grpc::Status finishStatus = writer->Finish();
        if (finishStatus.ok() && uploadResponse.success()) {
            LOG_INFO("U: ---- file upload success!: " << fileId << " ----")
            UPLOAD_FILE_RES(QString::fromStdString(fileId),true)
        } else {
            LOG_ERROR("U: ---- file upload failed!: " << finishStatus.error_message() << " ----")
            UPLOAD_FILE_RES("transfer terminate!",false)
        }
    }

    void RealtimeCommHandler::handleDownloadCommand(
        const QString &fileId,
        const QString & filePath,
        const QString & savePath)
    {
        // 准备下载请求
        FileRequest request;
        request.set_file_id(fileId.toStdString());
        request.set_file_path(filePath.toStdString());

        // 创建下载上下文
        grpc::ClientContext context;
        std::unique_ptr<grpc::ClientReader<FileChunk>> reader(
            _fileStub->DownloadFile(&context, request));

        // 创建临时目录
        QString tempDir = QString::fromStdString(g_pCommonData->getDataPath(file)) + "/" +
            QString::fromStdString(GetCurTime::getTimeObj()->getCurTime("%Y-%m-%d"));

        // 文件元数据
        QString finalFileName;
        QMap<int, QString> chunkPaths;
        bool downloadSuccess = true;

        // 接收数据流
        FileChunk chunk;
        LOG_INFO("D: ---- begin get file chunk! ----")
        while (reader->Read(&chunk)) {
            // 首次接收初始化文件名
            if (finalFileName.isEmpty()) {
                finalFileName = QString("%1/%2").arg(tempDir).arg(QString::fromStdString(chunk.file_id()));
            }

            // 校验数据块
            QByteArray data(chunk.data().data(), chunk.data().size());
            QString receivedMd5 = QString::fromStdString(chunk.checksum());
            QString calculatedMd5 = calculateChunkMD5(data);

            if (receivedMd5 != calculatedMd5) {
                LOG_ERROR("md5 verify failed!: " << chunk.chunk_number())
                downloadSuccess = false;
                break;
            }

            // 保存分块到临时文件
            QString chunkPath = QString("%1.tmp_%2").arg(finalFileName).arg(chunk.chunk_number());
            QFile chunkFile(chunkPath);
            if (!chunkFile.open(QIODevice::WriteOnly)) {
                LOG_ERROR("D: cant write tmp chunk: " << chunkPath.toStdString())
                downloadSuccess = false;
                break;
            }
            chunkFile.write(data);
            chunkFile.close();
            chunkPaths[chunk.chunk_number()] = chunkPath;

            // 更新进度
            emit sigDownloadProgress(static_cast<int>(
                chunkPaths.size() * 100.0 / chunk.total_chunks()));
        }

        // 检查最终状态
        grpc::Status status = reader->Finish();
        if (!status.ok() || !downloadSuccess) {
            // 清理临时文件
            for (const QString &path : chunkPaths) {
                QFile::remove(path);
            }
            DOWNLOAD_FILE_RES("D: download failed!: " + QString::fromStdString(status.error_message()),false)
            return;
        }

        // 合并文件
        QFile finalFile(savePath);
        if (!finalFile.open(QIODevice::WriteOnly)) {
            LOG_ERROR("D: cant create final merge file: " << savePath.toStdString())
            DOWNLOAD_FILE_RES("D: cant save final file!",false)
            return;
        }

        // 按顺序合并分块
        for (int i=0; i<chunkPaths.size(); ++i) {
            QFile chunkFile(chunkPaths[i]);
            if (!chunkFile.open(QIODevice::ReadOnly)) {
                finalFile.close();
                QFile::remove(savePath);
                DOWNLOAD_FILE_RES("D: file chunk occur error!",false)
                return;
            }
            finalFile.write(chunkFile.readAll());
            chunkFile.close();
            QFile::remove(chunkPaths[i]);
        }
        finalFile.close();

        LOG_INFO("D: ---- file download success!: " << savePath.toStdString() << " ----")
        DOWNLOAD_FILE_RES(savePath,true)
    }

    QString RealtimeCommHandler::calculateChunkMD5(const QByteArray &data) {
        unsigned char digest[MD5_DIGEST_LENGTH];
        MD5((const unsigned char*)data.constData(), data.size(), digest);

        char mdStr[33];
        for(int i=0; i<16; i++)
            snprintf(&mdStr[i*2], 3, "%02x", (unsigned int)digest[i]);

        return QString(mdStr);
    }

    // bool RealtimeCommHandler::initVideoCall(const QString& remoteId) {
    //     if (_isCallActive) {
    //         qWarning() << "Call already in progress";
    //         return false;
    //     }
    //
    //     _currentRemoteId = remoteId;
    //     startChildProcess();
    //
    //     // 创建新的WebRTC连接
    //     _webrtc->createPeerConnection();
    //     _webrtc->setRemoteDescription(remoteId);
    //
    //     _isCallActive = true;
    //     emit signalCallStateChanged(1); // 1 = 通话开始
    //     return true;
    // }

    // void RealtimeCommHandler::endVideoCall() {
    //     if (!_isCallActive) return;
    //
    //     _webrtc->closePeerConnection();
    //     if (_mediaProcess) {
    //         _mediaProcess->terminate();
    //         _mediaProcess->waitForFinished();
    //     }
    //
    //     _isCallActive = false;
    //     _currentRemoteId.clear();
    //     emit signalCallStateChanged(0); // 0 = 通话结束
    // }

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
                LOG_INFO("Heartbeat response: alive = " << alive)
            } else {
                static int count = 0;
                count++;
                LOG_INFO("RPC failed:" << status.error_message())
                if (count > _maxRetryCount) {
                    LOG_INFO("the progress over cause of try count has exceeded max retry count")
                    _isHeartbeatActive = false;
                    QCoreApplication::exit(1);
                }
            }
        }
    }
} // namespace RealtimeComm