//
// Created by FUQAQ on 2025/1/13.
//

#ifndef REALTIMEBUSINESSPROCESS_H
#define REALTIMEBUSINESSPROCESS_H

#include <string>
#include <unordered_map>

#include "../normal-business-process/tcp-socket/TcpSocket.h"

#include "grpcpp/grpcpp.h"

#include "heartbeat/HeartbeatDTO.grpc.pb.h"
#include "file-transfer/FileTransferDTO.grpc.pb.h"

struct UploadSession {
    std::string ssid;
    std::string file_name;
    int64_t total_chunks{};
    std::string temp_dir;
    std::atomic<int> received_chunks{0};
    UploadSession() = default;
    UploadSession(std::string ssid,std::string file_name, int64_t total_chunks, std::string temp_dir, int received_chunks) {
        this->ssid = ssid;
        this->file_name = file_name;
        this->total_chunks = total_chunks;
        this->temp_dir = temp_dir;
        this->received_chunks.store(received_chunks);
    }
    UploadSession& operator=(UploadSession&& other) noexcept {
        if (this != &other) {
            ssid = other.ssid;
            file_name = std::move(other.file_name);
            total_chunks = other.total_chunks;
            temp_dir = std::move(other.temp_dir);
            received_chunks.store(other.received_chunks.load());
        }
        return *this;
    }
};

// --------------------------------- GRPC Func Implement --------------------------------- //
class MediaServiceImpl final : public MediaService::Service {
public:
    // heartbeat check
    grpc::Status HeartbeatHandler(grpc::ServerContext* context,const HeartbeatRequest* request, HeartbeatResponse* response) override;
};

class FileTransferServiceImpl final : public FileTransferService::Service {
public:
    grpc::Status InitUpload(grpc::ServerContext* context,
                           const FileInitRequest* request,
                           FileInitResponse* response) override;

    grpc::Status UploadFile(grpc::ServerContext* context,
                           grpc::ServerReader<FileChunk>* reader,
                           FileResponse* response) override;

    grpc::Status DownloadFile(grpc::ServerContext* context,
                             const FileRequest* request,
                             grpc::ServerWriter<FileChunk>* writer) override;

private:
    std::string GenerateFileID();
    std::string calculateChunkMD5(const std::string& data);
    bool MergeChunks(const std::string& file_id, const std::string& final_path);

    std::mutex session_mutex_;
    std::unordered_map<std::string, UploadSession> active_sessions_;
    std::string fileSavePath = "";

    static constexpr int CHUNK_SIZE = 4 * 1024 * 1024;
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
    std::unique_ptr<FileTransferServiceImpl> _fileTransferService;
    std::unique_ptr<grpc::Server> _grpcServer;
};



#endif //REALTIMEBUSINESSPROCESS_H
