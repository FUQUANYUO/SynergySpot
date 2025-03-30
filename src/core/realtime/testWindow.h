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
        void startVideoCall();
    void endVideoCall();
    void handleVideoFrame(const QVideoFrame &frame);
    void updateRemoteVideo(const QImage &frame);

private:
    void setupUI();
    void initCamera();
    void setupConnections();
    void updateLocalVideo(const QImage &frame);

    RealtimeCommHandler* _commHandler;

    // UI 组件
    QLabel *_localVideoLabel;
    QLabel *_remoteVideoLabel;
    QPushButton *_callButton;
    QPushButton *_hangupButton;

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
    const QString LOCAL_SSID = "user123";
    const QString REMOTE_SSID = "target456";
};

#endif // TESTWINDOW_H