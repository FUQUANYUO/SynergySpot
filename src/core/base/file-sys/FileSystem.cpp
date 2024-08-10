//
// Created by FU-QAQ on 2024/6/3.
//

#include "FileSystem.h"
#include <array>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "forward_msg/ForwardMsgDTO.pb.h"

FileSystem::FileSystem(QObject * bl) : _bl(dynamic_cast<BusinessListen*>(bl)){
}

void FileSystem::uploadFile(const std::string &filePath, const std::string &destination) {
    std::ifstream file(filePath,std::ios::binary | std::ios::ate);
    if(!file.is_open()){
        LOG("Failed to load file for reading : " << filePath)
        return ;
    }
    std::streamsize fileSize = file.tellg();
    file.seekg(0,std::ios::beg);

    // 每块大小1KB
    const int chunkSize = 1024;
    int totalChunks = (static_cast<int>(fileSize) + chunkSize - 1) / chunkSize;
    std::array<char,chunkSize> buffer{};

    for(int i = 0; i < totalChunks; ++i){
        file.read(buffer.data(), chunkSize);
        // 实际读取的大小
        std::streamsize bytesRead = file.gcount();
        bool isLastChunk = (i == totalChunks - 1);
        sendFileChunk(destination, i, std::string(buffer.data(),bytesRead),isLastChunk);
    }
}

void FileSystem::downloadFile(const std::string &filename, int filesize, bool isSaveInServer) {
    SSDTO::ChatMessage chatMessage;
    auto fileTransferRequestDto = new SSDTO::FileTransferRequest_DTO();
    fileTransferRequestDto->set_filename(filename);
    fileTransferRequestDto->set_filesize(filesize);
    fileTransferRequestDto->set_is_save_in_server(isSaveInServer);
    fileTransferRequestDto->set_type(SSDTO::Business_Type::FILE_TRANSFER_REQUEST);

    chatMessage.set_allocated_file_transfer_request(fileTransferRequestDto);
    std::string out;
    chatMessage.SerializeToString(&out);
    // TODO : emit request signal
}

void FileSystem::handleFileChunk(const std::string& filename, int64_t offset, const std::string& data, bool isLastChunk) {
    saveFileChunk(filename, offset, data, isLastChunk);
    if(isLastChunk) {
        std::string directoryPath = "/tmp/" + CurSSID + "/";
        std::vector<std::string> tmpFiles;

        // 找出所有的临时文件
        for (const auto &entry: std::filesystem::directory_iterator(directoryPath)) {
            if (entry.path().filename().string().find(filename + "_") == 0) {
                tmpFiles.push_back(entry.path().string());
            }
        }

        // 根据偏移量排序，后续的检查文件是否缺失也可以在此处进行
        // TODO : check the file chunk is complete and safe verify
        std::sort(tmpFiles.begin(), tmpFiles.end());

        std::string outPath = "/downloads/" + CurSSID + "/" + filename;

        // 写入合并文件
        std::ofstream mergeFile(outPath, std::ios::binary);
        if (!mergeFile) {
            LOG("Failed to create mergeFile : " << filename)
            return;
        }

        // 按顺序合并
        for (const auto &tmpFile: tmpFiles) {
            std::ifstream chunkFile(tmpFile, std::ios::binary);
            if (!chunkFile) {
                LOG("Failed to open tmp file for reading : " << tmpFile)
                continue;
            }
            mergeFile << chunkFile.rdbuf();
            chunkFile.close();
        }
        mergeFile.close();

        // 清空tmp
        for (const auto &tmp: tmpFiles) {
            std::filesystem::remove(tmp);
        }
        LOG("File has been successfully merged: " << filename);
    }
}

void FileSystem::requestMissingChunks(const std::string &filename, const std::vector<int64_t>& missingOffsets) {
    SSDTO::ChatMessage chatMessage;
    auto missingChunkRequestDto = new SSDTO::MissingChunkRequest_DTO();

    missingChunkRequestDto->set_filename(filename);
    for(int i = 0; i < missingOffsets.size(); ++i){
        missingChunkRequestDto->set_offset(i,missingOffsets[i]);
    }
    missingChunkRequestDto->set_type(SSDTO::Business_Type::MISSING_CHUNK_REQUEST);

    chatMessage.set_allocated_missing_chunk_request(missingChunkRequestDto);

    std::string out;
    chatMessage.SerializeToString(&out);

    // TODO : emit request of getting missing file chunk
}

void FileSystem::sendFileChunk(const std::string &filename, int offset, const std::string &data, bool isLastChunk) {
    SSDTO::ChatMessage chatMessage;
    auto fileChunkDto = new SSDTO::FileChunk_DTO();

    fileChunkDto->set_filename(filename);
    fileChunkDto->set_offset(offset);
    fileChunkDto->set_data(data);
    fileChunkDto->set_is_last_chunk(isLastChunk);
    fileChunkDto->set_type(SSDTO::Business_Type::FILE_CHUNK);

    chatMessage.set_allocated_file_chunk(fileChunkDto);
    std::string out;
    chatMessage.SerializeToString(&out);

    // TODO : emit file chunk signal and achieve business part
}

void FileSystem::saveFileChunk(const std::string& filename, int64_t offset, const std::string& data, bool isLastChunk) {
    std::string tempFilePath = "/tmp/" + CurSSID + "/" + filename + "_" + std::to_string(offset);
    std::ofstream file(tempFilePath,std::ios::binary);
    if(!file){
        LOG("Failed to save file chunk : " << tempFilePath)
        return ;
    }
    file.write(data.data(),static_cast<int>(data.size()));
    file.close();

    if(isLastChunk){
        LOG("Received last chunk of file : " << filename)
    }
}
