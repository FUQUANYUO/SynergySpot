//
// Created by FUQAQ on 2025/1/13.
//

#ifndef REALTIMEBUSINESSPROCESS_H
#define REALTIMEBUSINESSPROCESS_H

#include <string>
#include "grpcpp/grpcpp.h"

#include "heartbeat/HeartbeatDTO.grpc.pb.h"

// --------------------------------- GRPC Func Implement --------------------------------- //
class MediaServiceImpl final : public MediaService::Service {
public:
    // heartbeat check
    grpc::Status HeartbeatHandler(grpc::ServerContext* context,const HeartbeatRequest* request, HeartbeatResponse* response) override;
};
// --------------------------------- GRPC Func Implement --------------------------------- //

class RealtimeBusinessProcess {
public:
    explicit RealtimeBusinessProcess(const std::string& address);
    ~RealtimeBusinessProcess();

    void Start();

    void Stop();

    void Wait();

private:
    std::string _serverAddr;
    std::unique_ptr<MediaServiceImpl> _mediaService;
    std::unique_ptr<grpc::Server> _grpcServer;
};



#endif //REALTIMEBUSINESSPROCESS_H
