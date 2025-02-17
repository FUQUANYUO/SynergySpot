//
// Created by FUQAQ on 2025/1/13.
//

#include "RealtimeBusinessProcess.h"
#include "help.h"

#include <openssl/md5.h>
#include <yaml-cpp/yaml.h>

#include <fstream>
#include <random>
#include <filesystem>
#include <atomic>

#include "DTO.pb.h"

extern std::string yamlPath;
extern TcpSocket toNormalSocket;

grpc::Status MediaServiceImpl::HeartbeatHandler(grpc::ServerContext *context, const HeartbeatRequest *request, HeartbeatResponse *response) {
    std::string client_id = request->client_id();
    int64_t timestamp = request->timestamp();

    LOG_INFO("Received heartbeat from client: " << client_id << ", timestamp: " << timestamp);
    // 设置响应
    response->set_alive(true);
    return grpc::Status::OK;
}

grpc::Status FileTransferServiceImpl::InitUpload(
    grpc::ServerContext *context,
    const FileInitRequest *request,
    FileInitResponse *response)
{
    YAML::Node config = YAML::LoadFile(yamlPath);
    std::lock_guard<std::mutex> lock(session_mutex_);
    fileSavePath = config["realtime-info"]["fileSavePath"].as<std::string>();
    // 生成唯一ID并创建临时目录
    std::string file_id = GenerateFileID();
    std::string temp_dir = fileSavePath + "/tmp/" + file_id + "/";

    if (!std::filesystem::create_directories(temp_dir)) {
        response->set_success(false);
        return grpc::Status::OK;
    }

    // TODO: check token of user

    UploadSession newSession(
        request->ssid(),
        request->file_name(),
        (request->file_size() + CHUNK_SIZE - 1) / CHUNK_SIZE,
        temp_dir,
        0
    );

    // 记录上传会话
    active_sessions_[file_id] = std::move(newSession);

    response->set_success(true);
    response->set_file_id(file_id);
    return grpc::Status::OK;
}

grpc::Status FileTransferServiceImpl::UploadFile(
    grpc::ServerContext *context,
    grpc::ServerReader<FileChunk> *reader,
    FileResponse *response)
{
    FileChunk chunk;
    std::string current_file_id;
    UploadSession* session = nullptr;

    while (reader->Read(&chunk)) {
        // 首次接收初始化会话
        if (current_file_id.empty()) {
            current_file_id = chunk.file_id();
            std::lock_guard<std::mutex> lock(session_mutex_);
            auto it = active_sessions_.find(current_file_id);
            if (it == active_sessions_.end()) {
                response->set_success(false);
                response->set_message("valid session id");
                return grpc::Status::OK;
            }
            session = &it->second;
        }

        // 校验分块数据
        if (calculateChunkMD5(chunk.data()) != chunk.checksum()) {
            response->set_success(false);
            response->set_message("chunk check failed");
            return grpc::Status::OK;
        }

        // 写入分块文件
        std::string chunk_path = session->temp_dir
                               + std::to_string(chunk.chunk_number());
        std::ofstream file(chunk_path, std::ios::binary);
        file.write(chunk.data().data(), chunk.data().size());
        file.close();

        // 更新进度
        session->received_chunks++;
    }

    // 验证完整性并合并文件
    if (session->received_chunks == session->total_chunks) {
        std::string prefixPath = fileSavePath + "/" + GetCurTime::getTimeObj()->getCurTime("%Y-%m-%d");
        std::string suffix = session->file_name.substr(session->file_name.find_last_of('.') + 1);
        std::string final_path = prefixPath +  "/" + current_file_id + "." + suffix;

        if ((!std::filesystem::exists(prefixPath)) && (!std::filesystem::create_directories(prefixPath)))
            LOG_ERROR("Failed to create directory: " << prefixPath);
        if (MergeChunks(current_file_id, final_path)) {
            response->set_success(true);
            response->set_message("file merge success !");

            // record file info
            SSDTO::FileStorageDTO fdto;
            fdto.set_file_id(current_file_id);
            fdto.set_uploader_ssid(session->ssid);
            fdto.set_file_name(session->file_name);
            fdto.set_file_size(session->total_chunks * CHUNK_SIZE);
            fdto.set_file_type(suffix);
            fdto.set_storage_path(final_path);

            std::string resDTO;
            fdto.SerializeToString(&resDTO);
            toNormalSocket.sendMsg(resDTO, SSDTO::C_FILE);
        } else {
            response->set_success(false);
            response->set_message("file merge failed !");
        }
    } else {
        response->set_success(false);
        response->set_message("chunk num not enough! file occur error! ");
    }

    // 清理会话
    std::lock_guard<std::mutex> lock(session_mutex_);
    active_sessions_.erase(current_file_id);
    return grpc::Status::OK;
}

grpc::Status FileTransferServiceImpl::DownloadFile(
    grpc::ServerContext *context,
    const FileRequest *request,
    grpc::ServerWriter<FileChunk> *writer)
{
    std::string filePath = request->file_path();
    if (!std::filesystem::exists(filePath)) {
        return grpc::Status(grpc::NOT_FOUND, "文件不存在");
    }

    // 读取并分块发送
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    const int64_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    const int total_chunks = (file_size + CHUNK_SIZE - 1) / CHUNK_SIZE;
    char buffer[CHUNK_SIZE];

    for (int i = 0; i < total_chunks; ++i) {
        file.read(buffer, CHUNK_SIZE);
        const size_t bytes_read = file.gcount();

        FileChunk chunk;
        chunk.set_file_id(request->file_id());
        chunk.set_chunk_number(i);
        chunk.set_total_chunks(total_chunks);
        chunk.set_data(buffer, bytes_read);
        chunk.set_checksum(calculateChunkMD5(std::string(buffer, bytes_read)));

        if (!writer->Write(chunk)) {
            break;
        }
    }
    return grpc::Status::OK;
}

std::string FileTransferServiceImpl::GenerateFileID() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);

    const char* hex_chars = "0123456789abcdef";
    std::string uuid(18, '-');

    for (size_t i=0; i<18; ++i) {
        switch(i) {
            case 5: case 12:
                break;
            default:
                uuid[i] = hex_chars[dis(gen) % 16];
        }
    }
    return "file_" + uuid + "_" + GetCurTime::getTimeObj()->getCurTime("%Y%m%d%H%M%S");
}

std::string FileTransferServiceImpl::calculateChunkMD5(const std::string &data) {
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5((const unsigned char*)data.c_str(), data.size(), digest);

    char mdStr[33];
    for(int i=0; i<16; i++)
        snprintf(&mdStr[i*2], 3, "%02x", (unsigned int)digest[i]);

    return mdStr;
}

bool FileTransferServiceImpl::MergeChunks(const std::string &file_id, const std::string &final_path) {
    std::lock_guard<std::mutex> lock(session_mutex_);
    auto& session = active_sessions_[file_id];

    std::ofstream final_file(final_path, std::ios::binary);
    if (!final_file) return false;

    for (int i = 0; i < session.total_chunks; ++i) {
        std::string chunk_path = session.temp_dir + std::to_string(i);
        std::ifstream chunk_file(chunk_path, std::ios::binary);
        if (!chunk_file) return false;

        final_file << chunk_file.rdbuf();
        chunk_file.close();
        std::filesystem::remove(chunk_path);
    }

    final_file.close();
    std::filesystem::remove_all(session.temp_dir);
    return true;
}

RealtimeBusinessProcess::RealtimeBusinessProcess(const std::string &address) : _serverAddr(address) {}


RealtimeBusinessProcess::~RealtimeBusinessProcess() {}

void RealtimeBusinessProcess::Start() {
    _mediaService = std::make_unique<MediaServiceImpl>();
    _fileTransferService = std::make_unique<FileTransferServiceImpl>();

    grpc::ServerBuilder builder;
    builder.AddListeningPort(_serverAddr, grpc::InsecureServerCredentials());

    builder.RegisterService(_mediaService.get());
    builder.RegisterService(_fileTransferService.get());

    builder.SetMaxReceiveMessageSize(INT_MAX);
    builder.SetMaxSendMessageSize(INT_MAX);

    _grpcServer = builder.BuildAndStart();

    YAML::Node config = YAML::LoadFile(yamlPath);
    std::string fileSavePath = config["realtime-info"]["fileSavePath"].as<std::string>();

    std::filesystem::create_directories(fileSavePath + "/tmp");
    std::filesystem::create_directories(fileSavePath);
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