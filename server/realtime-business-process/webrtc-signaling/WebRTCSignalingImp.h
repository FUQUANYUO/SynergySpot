//
// Created by FU-QAQ on 2025/3/30.
//

#ifndef WEBRTCSIGNALINGIMP_H
#define WEBRTCSIGNALINGIMP_H

#include <mutex>
#include <unordered_map>
#include <string>
#include <queue>
#include <condition_variable>
#include "webrtc-signaling/WebRTCSignaling.grpc.pb.h"

// Structure to hold pending signaling messages for each client
struct ClientSignalingQueue {
    std::deque<SignalingMessage> messages;
    std::mutex mutex;
    std::condition_variable cv;
    bool active = true;
};

// WebRTC signaling service implementation
class WebRTCSignalingServiceImpl final : public WebRTCSignalingService::Service {
public:
    // Create an offer to initiate a call
    grpc::Status CreateOffer(
        grpc::ServerContext* context,
        const SignalingRequest* request,
        SignalingResponse* response) override;

    // Handle answer from remote peer
    grpc::Status HandleAnswer(
        grpc::ServerContext* context,
        const SignalingRequest* request,
        SignalingResponse* response) override;

    // Exchange ICE candidates
    grpc::Status ExchangeIceCandidate(
        grpc::ServerContext* context,
        const IceCandidateRequest* request,
        SignalingResponse* response) override;

    // Bi-directional stream for real-time signaling updates
    grpc::Status SignalingStream(
        grpc::ServerContext* context,
        grpc::ServerReaderWriter<SignalingMessage, SignalingMessage>* stream) override;

    WebRTCSignalingServiceImpl() = default;
    ~WebRTCSignalingServiceImpl() = default;
private:
    // Add a message to a client's queue
    void queueSignalingMessage(const std::string& ssid, const SignalingMessage& message);

    // Client connection management
    std::mutex _clientsMutex;
    std::unordered_map<std::string, std::shared_ptr<ClientSignalingQueue>> _clientQueues;

    // Active call sessions (caller SSID -> callee SSID)
    std::mutex _callSessionsMutex;
    std::unordered_map<std::string, std::string> _callSessions;
};



#endif //WEBRTCSIGNALINGIMP_H
