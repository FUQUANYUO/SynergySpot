//
// Created by FU-QAQ on 2025/3/27.
//

#include "WebRTCHandler.h"
#include "help.h"

#include <QtCore>
#include <nlohmann/json.hpp>
#include <QImage>

using json = nlohmann::json;

WebRTCHandler::WebRTCHandler(RealtimeCommHandler* commHandler, QObject* parent)
    : QObject(parent)
    , _commHandler(commHandler)
{
    rtc::InitLogger(rtc::LogLevel::Warning);
}

WebRTCHandler::~WebRTCHandler() {
    endCall();
}

void WebRTCHandler::initialize() {
    if (_isInitialized) {
        LOG_WARNING("WebRTC already initialized, skipping re-initialization");
        return;
    }

    // Create RTC configuration with ICE servers
    setupICEServers();

    // Create peer connection with config
    _peerConnection = std::make_shared<rtc::PeerConnection>(*_config);

    // Setup connection state callback
    _peerConnection->onStateChange([this](rtc::PeerConnection::State state) {
        emit sigConnectionStateChanged(static_cast<int>(state));

        switch (state) {
        case rtc::PeerConnection::State::Connected:
            LOG_INFO("WebRTC connection established");
            break;
        case rtc::PeerConnection::State::Failed:
            LOG_ERROR("WebRTC connection failed");
            endCall();
            break;
        case rtc::PeerConnection::State::Disconnected:
            LOG_WARNING("WebRTC connection disconnected");
            break;
        case rtc::PeerConnection::State::Closed:
            LOG_INFO("WebRTC connection closed");
            break;
        default:
            // Other states: New, Connecting
            break;
        }
    });

    // Setup data channel callback
    _peerConnection->onDataChannel([this](std::shared_ptr<rtc::DataChannel> dc) {
        LOG_INFO("Data channel established: " << dc->label());
        setupDataChannel(dc);
    });

    // Setup ICE candidate callback
    _peerConnection->onLocalCandidate([this](rtc::Candidate candidate) {
        QString candidateStr = QString::fromStdString(candidate.candidate());
        QString mid = QString::fromStdString(candidate.mid());
        emit sigIceCandidateFound(candidateStr, mid);
        LOG_INFO("Local ICE candidate found: " << candidateStr.toStdString());
    });

    // Setup local description callback
    _peerConnection->onLocalDescription([this](rtc::Description description) {
        QString sdp = QString::fromStdString(std::string());
        QString type = description.typeString() == "offer" ? "offer" : "answer";
        LOG_INFO("Local description created of type: " << type.toStdString());
        emit sigLocalDescriptionCreated(sdp, type);
    });

    _isInitialized = true;
    LOG_INFO("WebRTC initialized");
}

void WebRTCHandler::setupICEServers() {
    _config = std::make_shared<rtc::Configuration>();

    // Add STUN server
    _config->iceServers.emplace_back("stun:stun.l.google.com:19302");

    _config->iceServers.emplace_back("stun:stun.qq.com:3478");     // 腾讯云STUN
    _config->iceServers.emplace_back("stun:stun.miwifi.com:3478"); // 小米路由器STUN
    _config->iceServers.emplace_back("stun:stun.xten.com:3478");   // 国内第三方服务

    // 备用国际STUN
    _config->iceServers.emplace_back("stun:stun.l.google.com:19302");
    _config->iceServers.emplace_back("stun:stun1.l.google.com:19302");

    // Example TURN server (uncomment and adjust if needed)
    // rtc::IceServer turnServer;
    // turnServer.urls.emplace_back("turn:turn.example.org:3478");
    // turnServer.username = "username";
    // turnServer.password = "password";
    // _config->iceServers.push_back(turnServer);
}

void WebRTCHandler::createOffer(const QString& targetSsid) {
    if (!_isInitialized) {
        LOG_ERROR("WebRTC not initialized");
        return;
    }

    _currentRemoteSsid = targetSsid;

    // Create data channel
    _dataChannel = _peerConnection->createDataChannel("main");
    setupDataChannel(_dataChannel);

    // Create offer
    _peerConnection->setLocalDescription(rtc::Description::Type::Offer);
    LOG_INFO("Creating offer for target: " << targetSsid.toStdString());
}

void WebRTCHandler::handleOffer(const QString& sdp, const QString& senderSsid) {
    if (!_isInitialized) {
        LOG_ERROR("WebRTC not initialized");
        return;
    }

    _currentRemoteSsid = senderSsid;

    // Parse SDP
    rtc::Description description(sdp.toStdString(), "offer");

    // Set remote description
    _peerConnection->setRemoteDescription(description);

    // Create answer
    _peerConnection->setLocalDescription();
    LOG_INFO("Handling offer from sender: " << senderSsid.toStdString());
}

void WebRTCHandler::handleAnswer(const QString& sdp, const QString& remoteSsid) {
    if (!_isInitialized) {
        LOG_ERROR("WebRTC not initialized");
        return;
    }

    _currentRemoteSsid = remoteSsid;

    // Parse SDP
    rtc::Description description(sdp.toStdString(), "answer");

    // Set remote description
    _peerConnection->setRemoteDescription(description);
    LOG_INFO("Remote answer set from: " << remoteSsid.toStdString());
}

void WebRTCHandler::handleRemoteCandidate(const QString &candidate, const QString &mid) {
    if (!_isInitialized) {
        LOG_ERROR("WebRTC not initialized");
        return;
    }

    // Parse ICE candidate
    try {
        rtc::Candidate ice(candidate.toStdString(), mid.toStdString());
        _peerConnection->addRemoteCandidate(ice);
        LOG_INFO("Added remote ICE candidate");
    } catch (const std::exception &e) {
        LOG_ERROR("Failed to parse remote candidate: " << e.what());
    }
}

void WebRTCHandler::sendVideoFrame(const std::string &frame) {
    _dataChannel->send(frame);
}

void WebRTCHandler::endCall() {
    if (_peerConnection) {
        _peerConnection->close();
        _peerConnection = nullptr;
    }

    if (_dataChannel) {
        _dataChannel->close();
        _dataChannel = nullptr;
    }

    _config.reset();
    _isInitialized = false;
    LOG_INFO("Call ended");
}

void WebRTCHandler::setupDataChannel(std::shared_ptr<rtc::DataChannel> dc) {
    dc->onOpen([dc]() {
        LOG_INFO("Data channel opened: " << dc->label());
    });

    dc->onClosed([]() {
        LOG_INFO("Data channel closed");
    });

    dc->onMessage([this](std::variant<rtc::binary, std::string> message) {
        if (std::holds_alternative<std::string>(message)) {
            QString msg = QString::fromStdString(std::get<std::string>(message));

            try {
                QJsonDocument doc = QJsonDocument::fromJson(msg.toUtf8());
                if (!doc.isNull() && doc.isObject()) {
                    QJsonObject obj = doc.object();
                    if (obj["type"].toString() == "video-frame") {
                        // 解码视频帧
                        QByteArray imageData = QByteArray::fromBase64(
                            obj["data"].toString().toLatin1());
                        QImage frame;
                        frame.loadFromData(imageData, "JPEG");

                        if (!frame.isNull()) {
                            QMetaObject::invokeMethod(this, [this, frame]() {
                                // 通过信号转发给RealtimeCommHandler
                                emit sigDataChannelPicReceived(frame);
                            }, Qt::QueuedConnection);
                        }
                    } else {
                        // 其他类型消息
                        QMetaObject::invokeMethod(this, [this, msg]() {
                            emit sigDataChannelMessageReceived(msg);
                        }, Qt::QueuedConnection);
                    }
                }
            } catch (...) {
                LOG_ERROR("Failed to parse video frame message");
            }
        }
    });

    dc->onError([](std::string error) {
        LOG_ERROR("Data channel error: " << error);
    });
}