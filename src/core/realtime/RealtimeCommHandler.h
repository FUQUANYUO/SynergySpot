//
// Created by FUQAQ on 2025/1/12.
//

#ifndef REALTIMECOMMHANDLER_H
#define REALTIMECOMMHANDLER_H

#include "heartbeat/HeartbeatDTO.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <QCoreApplication>

#include "CommonData.h"

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
    private slots:
        void sltCheckHeartbeat();
    private:
        void initWebRTC();
        void setupGRPCChannel();

//        std::unique_ptr<WebRTCInterface> _webrtc;
        bool _isCallActive;
        bool _isHeartbeatActive;
        int  _coolDownTime;
        int  _maxRetryCount;
        QString _currentRemoteId;
        QTimer* _heartbeatTimer;
        std::shared_ptr<grpc::Channel> _channel;
        std::unique_ptr<MediaService::Stub> _stub;
    };
} // namespace RealtimeComm



#endif //REALTIMECOMMHANDLER_H
