//
// Created by FUQAQ on 2025/1/12.
//

#ifndef REALTIMECOMMHANDLER_H
#define REALTIMECOMMHANDLER_H

#include "heartbeat/HeartbeatDTO.grpc.pb.h"
#include "file-transfer/FileTransferDTO.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <QCoreApplication>

#include "CommonData.h"

class QLocalSocket;

namespace RealtimeComm {
    class RealtimeCommHandler : public QObject {
        Q_OBJECT
    public:
        explicit RealtimeCommHandler(QObject* parent = nullptr);
        ~RealtimeCommHandler();

        // 开始通讯
        int startGrpcService();
//        // 初始化音视频通话
//        bool initVideoCall(const QString& remoteId);
//        // 结束音视频通话
//        void endVideoCall();
    signals:
        void sigConnectionLost();

        void sigUploadProgress(int percent);
        void sigUploadFinished(const QJsonObject &resp);

        void sigDownloadProgress(int percent);
        void sigDownloadFinished(const QJsonObject &resp);

    private slots:
        void sltCheckHeartbeat();

        // send res to main process
        void sltSendResponse(const QJsonObject &resp);
    private:
        void initWebRTC();
        void setupGRPCChannel();

        void processCommand(const QString& line);

        // file
        void handleUploadCommand(const QString& localUrl);
        void handleDownloadCommand(const QString& fileId,const QString & filePath,const QString & savePath);

        // MD5 verify
        QString calculateChunkMD5(const QByteArray &data);

//        std::unique_ptr<WebRTCInterface> _webrtc;
private:
        bool _isCallActive;
        bool _isHeartbeatActive;
        int  _coolDownTime;
        int  _maxRetryCount;
        QString _currentRemoteId;
        QTimer* _heartbeatTimer;
        std::shared_ptr<grpc::Channel> _channel;
        std::unique_ptr<FileTransferService::Stub> _fileStub;
        std::unique_ptr<MediaService::Stub> _mediaStub;
        QLocalSocket * _pIPCSocket;

        static constexpr int CHUNK_SIZE = 4 * 1024 * 1024; // 4MB
    };
} // namespace RealtimeComm



#endif //REALTIMECOMMHANDLER_H
