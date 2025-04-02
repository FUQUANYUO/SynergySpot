#ifndef TESTWINDOW_H
#define TESTWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoSink>
#include <QMutex>
#include "RealtimeCommHandler.h"

class VideoWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit VideoWindow(QWidget *parent = nullptr);
    ~VideoWindow();

    private slots:
    void endVideoCall();
    void handleVideoFrame(const QVideoFrame &frame);
    void updateRemoteVideo(const QImage &frame);
    void onCallBtn();

private:
    void setupUI();
    void initCamera();
    void setupConnections();
    void updateLocalVideo(const QImage &frame);
    void initSignaling();  // 新增信令初始化方法

    RealtimeCommHandler* _commHandler;

    // UI 组件
    QLabel *_localVideoLabel;
    QLabel *_remoteVideoLabel;
    QPushButton *_hangupButton;
    QPushButton *_startBtn;

    // 视频捕获组件
    QCamera *_camera;
    QMediaCaptureSession *_captureSession;
    QVideoSink *_videoSink;

    // 帧处理
    QMutex _frameMutex;
    QImage _lastLocalFrame;
    QTimer *_frameSendTimer;
    int _targetFPS = 15;

    // 固定SSID
    const QString LOCAL_SSID = "100";
    const QString REMOTE_SSID = "1000";
};

#endif // TESTWINDOW_H