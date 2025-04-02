//
// Created by FU-QAQ on 2025/3/27.
//
#pragma once
#ifndef WEBRTCHANDLER_H
#define WEBRTCHANDLER_H

#include <QObject>
#include <QString>
#include <memory>
#include <rtc/rtc.hpp>

// Forward declarations for libdatachannel types
namespace rtc {
    class PeerConnection;
    class DataChannel;
    struct Configuration;
    class Description;
    struct Candidate;
}

class RealtimeCommHandler;

class WebRTCHandler : public QObject {
    Q_OBJECT
public:
    explicit WebRTCHandler(RealtimeCommHandler* commHandler, QObject* parent = nullptr);
    ~WebRTCHandler();

    // Initialize WebRTC connection
    void initialize();

    // Create offer to start a call
    void createOffer(const QString& targetSsid);

    // Handle remote offer/answer
    void handleOffer(const QString& sdp, const QString& senderSsid);
    void handleAnswer(const QString& sdp, const QString& remoteSsid);

    // Handle remote ICE candidate
    void handleRemoteCandidate(const QString& candidate, const QString& mid);

    // 上传视频图像
    void sendVideoFrame(const std::string &frame);

    // End current call
    void endCall();

signals:
    // Signal emitted when local description is created
    void sigLocalDescriptionCreated(const QString& sdp, const QString& type);

    // Signal emitted when ICE candidate is found
    void sigIceCandidateFound(const QString& candidate, const QString& mid);

    // Signal emitted when data channel message is received
    void sigDataChannelMessageReceived(const QString& message);

    void sigDataChannelPicReceived(const QImage &pic);

    // Signal emitted when connection state changes
    void sigConnectionStateChanged(int state);

private:
    // Setup ICE servers and configuration
    void setupICEServers();

    // Setup data channel handlers
    void setupDataChannel(std::shared_ptr<rtc::DataChannel> dc);

private:
    RealtimeCommHandler* _commHandler;
    std::shared_ptr<rtc::PeerConnection> _peerConnection;
    std::shared_ptr<rtc::DataChannel>    _dataChannel;
    std::shared_ptr<rtc::Configuration>  _config;
    std::queue<rtc::Candidate>           _pendingCandidates; // 缓存候选
    std::mutex                           _candidateMutex;

    bool _isInitialized = false;
    QString _currentRemoteSsid;
};

#endif // WEBRTCHANDLER_H