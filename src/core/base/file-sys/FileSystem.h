//
// Created by FU-QAQ on 2024/6/3.
//

#ifndef SYNERGYSPOT_FILESYSTEM_H
#define SYNERGYSPOT_FILESYSTEM_H

#include "../business-listen/BusinessListen.h"
#include "help.h"


class FileSystem : public QObject{
    Q_OBJECT
public:
    FileSystem(QObject * bl = nullptr);
    ~FileSystem() = default;

    // 上传文件
    void uploadFile(const std::string& filePath, const std::string& destination);

    // 接收文件
    void downloadFile(const std::string &filename, int filesize, bool isSaveInServer);

    // 处理接收到的文件块
    void handleFileChunk(const std::string& filename, int64_t offset, const std::string& data, bool isLastChunk);

    // 请求缺少文件
    void requestMissingChunks(const std::string &filename, const std::vector<int64_t>& missingOffsets);
private:
    void sendFileChunk(const std::string& filename, int offset, const std::string& data, bool isLastChunk);
    void saveFileChunk(const std::string& filename, int64_t offset, const std::string& data, bool isLastChunk);

    BusinessListen * _bl;
};


#endif//SYNERGYSPOT_FILESYSTEM_H
