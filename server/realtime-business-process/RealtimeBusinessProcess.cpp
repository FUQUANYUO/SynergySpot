//
// Created by FUQAQ on 2025/1/13.
//

#include "RealtimeBusinessProcess.h"
#include "help.h"

grpc::Status MediaServiceImpl::HeartbeatHandler(grpc::ServerContext *context, const HeartbeatRequest *request, HeartbeatResponse *response) {
    std::string client_id = request->client_id();
    int64_t timestamp = request->timestamp();

    LOG_INFO("Received heartbeat from client: " << client_id << ", timestamp: " << timestamp);
    // 设置响应
    response->set_alive(true);
    return grpc::Status::OK;
}

RealtimeBusinessProcess::RealtimeBusinessProcess(const std::string &address) : _serverAddr(address) {}


RealtimeBusinessProcess::~RealtimeBusinessProcess() {}

void RealtimeBusinessProcess::Start() {
    _mediaService = std::make_unique<MediaServiceImpl>();

    grpc::ServerBuilder builder;
    builder.AddListeningPort(_serverAddr, grpc::InsecureServerCredentials());
    builder.RegisterService(_mediaService.get());

    _grpcServer = builder.BuildAndStart();
    LOG_INFO("--------------------------- SynergySpot-GPRC-Server Beginning ----------------------------")
    LOG_INFO("Server listening on " << _serverAddr);
}

void RealtimeBusinessProcess::Stop() {
    if (_grpcServer != nullptr) {
        LOG_INFO("Shutting down server...");
        LOG_INFO("--------------------------- SynergySpot-GPRC-Server Ending ----------------------------")
        _grpcServer->Shutdown();
    }
}

void RealtimeBusinessProcess::Wait() {
    if (_grpcServer != nullptr) {
        _grpcServer->Wait();
    }
}