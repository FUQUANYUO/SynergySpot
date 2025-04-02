//
// Created by FUQAQ on 2025/1/12.
//

#ifndef REALTIMECOMMHANDLER_H
#define REALTIMECOMMHANDLER_H

#include "heartbeat/HeartbeatDTO.grpc.pb.h"
#include "file-transfer/FileTransferDTO.grpc.pb.h"
#include "webrtc-signaling/WebRTCSignaling.grpc.pb.h"

#include "web-rtc-api/WebRTCHandler.h"

#include <grpcpp/grpcpp.h>
#include <QApplication>

#include "CommonData.h"

class QLocalSocket;
class AsyncDownloadTask;
class SyncUploadTask;

class VideoWindow;

class AsyncTask {
public:
    virtual ~AsyncTask() = default;
    virtual void proceed(bool ok) = 0;  // 纯虚函数，子类实现具体逻辑
};

class RealtimeCommHandler : public QObject {
    Q_OBJECT
public:
    explicit RealtimeCommHandler(QObject* parent = nullptr);
    ~RealtimeCommHandler();

    // 开始通讯
    int startGrpcService();

    // 获取异步队列
    grpc::CompletionQueue *getCompletionQueue();

    void setUserSSID(const QString& ssid);

    // 获取当前 ssid
    QString getUserSSID();

    // MD5 verify
    QString calculateChunkMD5(const QByteArray &data);

    // 上传成功结束回调
    void onDownloadSuccess(AsyncDownloadTask* task);

    // 上传失败结束回调
    void onDownloadFailed(const std::string& error);

    // 下载成功结束回调
    void onUploadSuccess(SyncUploadTask* task);

    // 下载失败结束回调
    void onUploadFailed(const std::string& error);

    // 连接信令服务器
    void connectToSignalingServer();

    // 启动/结束 视频通话
    bool startVideoCall(const QString& targetSsid);
    void endVideoCall();

    // 远端请求
    void handleRemoteOffer(const QString& sdp, const QString& senderSsid);

    // 远端响应
    void handleRemoteAnswer(const QString& sdp);

    void handleRemoteIceCandidate(const QString& candidate, const QString& mid);

    bool isCallActive();
signals:
    void sigConnectionLost();

    void sigUploadProgress(int percent);
    void sigUploadFinished(const QJsonObject &resp);

    void sigDownloadProgress(int percent);
    void sigDownloadFinished(const QJsonObject &resp);

    void sigSignalingConnected();
    void sigRemoteOfferReceived(const QString& sdp, const QString& senderSsid);
    void sigRemoteAnswerReceived(const QString& sdp);
    void sigRemoteIceCandidateReceived(const QString& candidate, const QString& mid);
    void sigRemoteVideoFrameReceived(const QImage &frame);
    void sigCallStateChanged(int state);

    void sigGRPCDisconnect();
public:
    static constexpr int CHUNK_SIZE = 3 * 1024 * 1024; // 4MB

public slots:
    void sltCheckHeartbeat();

    // send res to main process
    void sltSendResponse(const QJsonObject &resp);

    void sltSendVideoFrame(const QImage &frame);
private:
    // void initWebRTC();
    void setupGRPCChannel();

    void processCommand(const QString& line);

    // file
    void handleUploadCommand(const QString& localUrl,const QString& type, FileStorageDTO fileDTO);
    void handleDownloadCommand(const QString& saveLocPath,const QString& type,const FileStorageDTO& fileDTO);

    void setupWebRTCSignaling();
    void sendSignalingMessage(const SignalingMessage& message);
    void processSignalingStream();

private:
    bool _isCallActive;
    bool _isHeartbeatActive;
    int  _coolDownTime;
    int  _maxRetryCount;
    int  _tryLinkCount;

    QString  curUserSSID = "100";
    QString _currentRemoteId = "1000";
    QTimer* _heartbeatTimer;
    QLocalSocket * _pIPCSocket;

    std::shared_ptr<grpc::Channel> _channel;
    std::unique_ptr<FileTransferService::Stub> _fileStub;
    std::unique_ptr<MediaService::Stub> _mediaStub;
    std::unique_ptr<WebRTCSignalingService::Stub> _signalingStub;

    // async do
    grpc::CompletionQueue _cq;
    std::atomic<bool> _shutdown{false};
    std::thread _cqThread;

    std::unique_ptr<grpc::ClientReaderWriter<SignalingMessage, SignalingMessage>> _signalingStream;
    std::unique_ptr<grpc::ClientContext>    _signalingContext;
    std::unique_ptr<WebRTCHandler>          _webRTCHandler;
    std::atomic<bool>                       _isSignalingActive{false};
    std::atomic<bool>                       _streamShutdown{false}; // 流式读取终止标志
    std::thread                             _signalingThread;
    std::mutex                              _streamMutex; // 流式读写互斥锁
};



#endif //REALTIMECOMMHANDLER_H
