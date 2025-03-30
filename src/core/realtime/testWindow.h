//
// Created by FU-QAQ on 2025/3/30.
//

#ifndef TESTWINDOW_H
#define TESTWINDOW_H
#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoSink>
#include "RealtimeCommHandler.h"

class VideoWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit VideoWindow(QWidget *parent = nullptr);
    ~VideoWindow();

    private slots:
    void setUserSSID();
    void startVideoCall();
    void endVideoCall();
    void updateLocalVideo(const QImage &frame);
    void updateRemoteVideo(const QImage &frame);

private:
    void setupUI();
    void initWebRTC();

    RealtimeCommHandler* _commHandler;
    QLabel *_localVideoLabel;
    QLabel *_remoteVideoLabel;
    QLineEdit *_ssidEdit;
    QLineEdit *_targetSsidEdit;
    QPushButton *_callButton;
    QPushButton *_hangupButton;

    // 新增的媒体相关成员变量
    QCamera *_camera;
    QMediaCaptureSession *_captureSession;
    QVideoSink *_videoSink;
};

#endif //TESTWINDOW_H