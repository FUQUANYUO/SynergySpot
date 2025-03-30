// VideoWindow.cpp
#include "testWindow.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoSink>

VideoWindow::VideoWindow(QWidget *parent)
    : QMainWindow(parent) {
    _commHandler = new RealtimeCommHandler(this);

    setupUI();
    initWebRTC();
}

VideoWindow::~VideoWindow() {

}

void VideoWindow::setupUI() {
    // 视频显示区域
    _localVideoLabel = new QLabel(this);
    _localVideoLabel->setFixedSize(640, 480);
    _localVideoLabel->setStyleSheet("background-color: black;");

    _remoteVideoLabel = new QLabel(this);
    _remoteVideoLabel->setFixedSize(640, 480);
    _remoteVideoLabel->setStyleSheet("background-color: black;");

    // 控制区域
    _ssidEdit = new QLineEdit(this);
    _ssidEdit->setPlaceholderText("输入你的SSID");

    _targetSsidEdit = new QLineEdit(this);
    _targetSsidEdit->setPlaceholderText("输入目标SSID");

    _callButton = new QPushButton("发起通话", this);
    _hangupButton = new QPushButton("结束通话", this);
    _hangupButton->setEnabled(false);

    // 布局
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout *videoLayout = new QHBoxLayout;
    videoLayout->addWidget(_localVideoLabel);
    videoLayout->addWidget(_remoteVideoLabel);

    QHBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->addWidget(_ssidEdit);
    controlLayout->addWidget(_targetSsidEdit);
    controlLayout->addWidget(_callButton);
    controlLayout->addWidget(_hangupButton);

    mainLayout->addLayout(videoLayout);
    mainLayout->addLayout(controlLayout);

    setCentralWidget(centralWidget);

    // 信号连接
    connect(_callButton, &QPushButton::clicked, this, &VideoWindow::startVideoCall);
    connect(_hangupButton, &QPushButton::clicked, this, &VideoWindow::endVideoCall);
    connect(_commHandler, &RealtimeCommHandler::sigCallStateChanged, [this](int state) {
        _hangupButton->setEnabled(state == 1);
    });
}

void VideoWindow::initWebRTC() {
    // 初始化本地视频采集
    _camera = new QCamera(this);
    _captureSession = new QMediaCaptureSession(this);
    _captureSession->setCamera(_camera);

    // 创建视频接收器并连接到槽函数
    _videoSink = new QVideoSink(this);
    _captureSession->setVideoOutput(_videoSink);

    connect(_videoSink, &QVideoSink::videoFrameChanged, this, [this](const QVideoFrame &frame) {
        QImage image = frame.toImage();
        if (!image.isNull()) {
            updateLocalVideo(image.scaled(640, 480));
        }
    });

    _camera->start();
}

void VideoWindow::setUserSSID() {
    _commHandler->getUserSSID();
}

void VideoWindow::startVideoCall() {
    QString targetSsid = _targetSsidEdit->text();
    if (targetSsid.isEmpty()) {
        QMessageBox::warning(this, "错误", "请输入目标SSID");
        return;
    }

    if (_commHandler->startVideoCall(targetSsid)) {
        QMessageBox::information(this, "提示", "已发起通话请求");
    }
}

void VideoWindow::endVideoCall() {
    _commHandler->endVideoCall();
}

void VideoWindow::updateLocalVideo(const QImage &frame) {
    _localVideoLabel->setPixmap(QPixmap::fromImage(frame));
}

void VideoWindow::updateRemoteVideo(const QImage &frame) {
    _remoteVideoLabel->setPixmap(QPixmap::fromImage(frame));
}