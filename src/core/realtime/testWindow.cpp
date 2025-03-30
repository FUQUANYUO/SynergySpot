#include "testWindow.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QMediaDevices>
#include <QTimer>

VideoWindow::VideoWindow(QWidget *parent)
    : QMainWindow(parent),
      _camera(nullptr),
      _captureSession(nullptr),
      _videoSink(nullptr) {

    _commHandler = new RealtimeCommHandler(this);
    _commHandler->setUserSSID(LOCAL_SSID); // 设置固定本地SSID

    _frameSendTimer = new QTimer(this);
    _frameSendTimer->setInterval(1000 / _targetFPS);

    setupUI();
    initCamera();
    setupConnections();
}

VideoWindow::~VideoWindow() {
    if (_camera && _camera->isActive()) {
        _camera->stop();
    }
}

void VideoWindow::setupUI() {
    // 视频显示区域
    _localVideoLabel = new QLabel(this);
    _localVideoLabel->setFixedSize(640, 480);
    _localVideoLabel->setStyleSheet("background-color: black;");

    _remoteVideoLabel = new QLabel(this);
    _remoteVideoLabel->setFixedSize(640, 480);
    _remoteVideoLabel->setStyleSheet("background-color: black;");

    // 控制按钮
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
    controlLayout->addWidget(_callButton);
    controlLayout->addWidget(_hangupButton);

    mainLayout->addLayout(videoLayout);
    mainLayout->addLayout(controlLayout);

    setCentralWidget(centralWidget);

    // 信号连接
    connect(_callButton, &QPushButton::clicked, this, &VideoWindow::startVideoCall);
    connect(_hangupButton, &QPushButton::clicked, this, &VideoWindow::endVideoCall);
}

void VideoWindow::initCamera() {
    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    if (cameras.isEmpty()) {
        QMessageBox::warning(this, "错误", "未找到可用的摄像头");
        return;
    }

    _camera = new QCamera(cameras.first(), this);
    _captureSession = new QMediaCaptureSession(this);
    _captureSession->setCamera(_camera);
    _videoSink = new QVideoSink(this);
    _captureSession->setVideoOutput(_videoSink);
    _camera->start();
}

void VideoWindow::setupConnections() {
    connect(_videoSink, &QVideoSink::videoFrameChanged,
            this, &VideoWindow::handleVideoFrame);

    connect(_frameSendTimer, &QTimer::timeout, this, [this]() {
        QMutexLocker locker(&_frameMutex);
        if (!_lastLocalFrame.isNull() && _commHandler->isCallActive()) {
            _commHandler->sltSendVideoFrame(_lastLocalFrame.scaled(640, 480, Qt::KeepAspectRatio));
        }
    });

    connect(_commHandler, &RealtimeCommHandler::sigRemoteVideoFrameReceived,
            this, &VideoWindow::updateRemoteVideo);

    connect(_commHandler, &RealtimeCommHandler::sigCallStateChanged, [this](int state) {
        _hangupButton->setEnabled(state == 1);
        if (state == 1) {
            _frameSendTimer->start(1000 / _targetFPS);
        }else {
            _frameSendTimer->stop();
        }
    });
}

void VideoWindow::startVideoCall() {
    if (_commHandler->startVideoCall(REMOTE_SSID)) {
        QMessageBox::information(this, "提示", QString("已向 %1 发起通话请求").arg(REMOTE_SSID));
    }
}

void VideoWindow::endVideoCall() {
    _commHandler->endVideoCall();
}

void VideoWindow::handleVideoFrame(const QVideoFrame &frame) {
    QImage image = frame.toImage();
    if (!image.isNull()) {
        QMutexLocker locker(&_frameMutex);
        _lastLocalFrame = image.copy();
        updateLocalVideo(image);
    }
}

void VideoWindow::updateLocalVideo(const QImage &frame) {
    _localVideoLabel->setPixmap(QPixmap::fromImage(frame));
}

void VideoWindow::updateRemoteVideo(const QImage &frame) {
    _remoteVideoLabel->setPixmap(QPixmap::fromImage(frame));
}