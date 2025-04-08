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

class VideoAudioCallPage;
class VideoAudioInvitePage;
class QLocalSocket;
class AsyncDownloadTask;
class SyncUploadTask;

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

    // 音视频启动失败回调
    void onStartVideoFailed(int errorCode);

    // 开始音视频通话
    int startVideoCall(const QString& remoteId,bool isOtherUserInvite,const QString& userSig);

    // 结束音视频通话
    void endVideoCall();
signals:
    void sigConnectionLost();

    void sigUploadProgress(int percent);
    void sigUploadFinished(const QJsonObject &resp);

    void sigDownloadProgress(int percent);
    void sigDownloadFinished(const QJsonObject &resp);

    void sigGRPCDisconnect();
public:
    static constexpr int CHUNK_SIZE = 3 * 1024 * 1024; // 4MB

private slots:
    void sltCheckHeartbeat();

    // send res to main process
    void sltSendResponse(const QJsonObject &resp);
private:
    // void initWebRTC();
    void setupGRPCChannel();

    void processCommand(const QString& line);

    // file
    void handleUploadCommand(const QString& localUrl,const QString& type, FileStorageDTO fileDTO);
    void handleDownloadCommand(const QString& saveLocPath,const QString& type,const FileStorageDTO& fileDTO);
private:
    bool _isHeartbeatActive;
    bool _isCallActive = false;
    int  _coolDownTime;
    int  _maxRetryCount;
    int  _tryLinkCount;

    QString  curUserSSID = "";
    QString _currentRemoteId;
    QTimer* _heartbeatTimer;
    QLocalSocket * _pIPCSocket;

    std::shared_ptr<grpc::Channel> _channel;
    std::unique_ptr<FileTransferService::Stub> _fileStub;
    std::unique_ptr<MediaService::Stub> _mediaStub;

    // async do
    grpc::CompletionQueue _cq;
    std::atomic<bool> _shutdown{false};
    std::thread _cqThread;

    // video
    std::unique_ptr<VideoAudioInvitePage> _videoAudioInvitePage;
    std::unique_ptr<VideoAudioCallPage>   _videoAudioCallPage;
};



#endif //REALTIMECOMMHANDLER_H
