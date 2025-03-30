//
// Created by FU-QAQ on 2025/3/30.
//

#include "WebRTCSignalingImp.h"

#include "help.h"
#include <thread>

grpc::Status WebRTCSignalingServiceImpl::CreateOffer(
    grpc::ServerContext* context,
    const SignalingRequest* request,
    SignalingResponse* response)
{
    LOG_INFO("Received offer from " << request->sender_ssid()
             << " to " << request->target_ssid());

    // Prepare signaling message
    SignalingMessage message;
    message.set_type(SignalingMessage_MessageType_OFFER);
    message.set_sender_ssid(request->sender_ssid());
    message.set_target_ssid(request->target_ssid());
    message.set_content(request->sdp());

    // Queue the message for the target client
    queueSignalingMessage(request->target_ssid(), message);

    // Update call sessions map
    {
        std::lock_guard<std::mutex> lock(_callSessionsMutex);
        _callSessions[request->sender_ssid()] = request->target_ssid();
    }

    response->set_success(true);
    response->set_message("Offer sent to " + request->target_ssid());
    return grpc::Status::OK;
}

grpc::Status WebRTCSignalingServiceImpl::HandleAnswer(
    grpc::ServerContext* context,
    const SignalingRequest* request,
    SignalingResponse* response)
{
    LOG_INFO("Received answer from " << request->sender_ssid()
             << " to " << request->target_ssid());

    // Prepare signaling message
    SignalingMessage message;
    message.set_type(SignalingMessage_MessageType_ANSWER);
    message.set_sender_ssid(request->sender_ssid());
    message.set_target_ssid(request->target_ssid());
    message.set_content(request->sdp());

    // Queue the message for the target client
    queueSignalingMessage(request->target_ssid(), message);

    response->set_success(true);
    response->set_message("Answer sent to " + request->target_ssid());
    return grpc::Status::OK;
}

grpc::Status WebRTCSignalingServiceImpl::ExchangeIceCandidate(
    grpc::ServerContext* context,
    const IceCandidateRequest* request,
    SignalingResponse* response)
{
    LOG_INFO("Received ICE candidate from " << request->sender_ssid()
             << " to " << request->target_ssid());

    // Prepare signaling message
    SignalingMessage message;
    message.set_type(SignalingMessage_MessageType_ICE_CANDIDATE);
    message.set_sender_ssid(request->sender_ssid());
    message.set_target_ssid(request->target_ssid());
    message.set_content(request->candidate());
    message.set_sdp_mid(request->sdp_mid());

    // Queue the message for the target client
    queueSignalingMessage(request->target_ssid(), message);

    response->set_success(true);
    response->set_message("ICE candidate sent to " + request->target_ssid());
    return grpc::Status::OK;
}

grpc::Status WebRTCSignalingServiceImpl::SignalingStream(
    grpc::ServerContext* context,
    grpc::ServerReaderWriter<SignalingMessage, SignalingMessage>* stream)
{
    // First message should contain the client's SSID
    SignalingMessage initialMessage;
    if (!stream->Read(&initialMessage)) {
        return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Failed to read initial message");
    }

    std::string clientSsid = initialMessage.sender_ssid();
    LOG_INFO("Client connected to signaling stream: " << clientSsid);

    // Create a queue for this client
    std::shared_ptr<ClientSignalingQueue> clientQueue =
        std::make_shared<ClientSignalingQueue>();

    // Register the client
    {
        std::lock_guard<std::mutex> lock(_clientsMutex);
        _clientQueues[clientSsid] = clientQueue;
    }

    // Start a thread to read messages from the client
    std::thread readThread([this, stream, clientSsid]() {
        SignalingMessage message;
        while (stream->Read(&message)) {
            if (message.type() == SignalingMessage_MessageType_HANGUP) {
                // Handle call termination
                std::string targetSsid;
                {
                    std::lock_guard<std::mutex> lock(_callSessionsMutex);
                    auto it = _callSessions.find(clientSsid);
                    if (it != _callSessions.end()) {
                        targetSsid = it->second;
                        _callSessions.erase(it);
                    }
                }

                if (!targetSsid.empty()) {
                    // Forward hangup to the other party
                    message.set_target_ssid(targetSsid);
                    queueSignalingMessage(targetSsid, message);
                }
            } else if (!message.target_ssid().empty()) {
                // Forward the message to the target
                queueSignalingMessage(message.target_ssid(), message);
            }
        }
    });

    // Send queued messages to the client
    {
        std::unique_lock<std::mutex> lock(clientQueue->mutex);
        while (clientQueue->active) {
            while (!clientQueue->messages.empty()) {
                const SignalingMessage& message = clientQueue->messages.front();
                if (!stream->Write(message)) {
                    clientQueue->active = false;
                    break;
                }
                clientQueue->messages.pop();
            }

            if (clientQueue->active) {
                // Wait for new messages or timeout after 30 seconds
                clientQueue->cv.wait_for(lock, std::chrono::seconds(30),
                    [&clientQueue]() {
                        return !clientQueue->messages.empty() || !clientQueue->active;
                    });
            }
        }
    }

    // Clean up when the client disconnects
    {
        std::lock_guard<std::mutex> lock(_clientsMutex);
        _clientQueues.erase(clientSsid);
    }

    // Clean up any active call sessions
    {
        std::lock_guard<std::mutex> lock(_callSessionsMutex);
        auto it = _callSessions.find(clientSsid);
        if (it != _callSessions.end()) {
            _callSessions.erase(it);
        }
    }

    if (readThread.joinable()) {
        readThread.join();
    }

    LOG_INFO("Client disconnected from signaling stream: " << clientSsid);
    return grpc::Status::OK;
}

void WebRTCSignalingServiceImpl::queueSignalingMessage(const std::string& ssid, const SignalingMessage& message)
{
    std::lock_guard<std::mutex> lock(_clientsMutex);
    auto it = _clientQueues.find(ssid);
    if (it != _clientQueues.end()) {
        std::shared_ptr<ClientSignalingQueue> queue = it->second;
        {
            std::lock_guard<std::mutex> queueLock(queue->mutex);
            queue->messages.push(message);
        }
        queue->cv.notify_one();
    } else {
        LOG_INFO("Client not connected: " << ssid);
    }
}