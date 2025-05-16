//
// Created by FUQAQ on 2025/3/1.
//

#ifndef ASYNCDOWNLOADTASK_H
#define ASYNCDOWNLOADTASK_H

#include "../../RealtimeCommHandler.h"

class AsyncDownloadTask : public AsyncTask{
    friend class RealtimeCommHandler;
public:
    explicit AsyncDownloadTask(
        RealtimeCommHandler* handler,
        std::unique_ptr<FileTransferService::Stub> stub,
        const FileStorageDTO& dto,
        const QString& savePath,
        const QString& type
    );

    ~AsyncDownloadTask();

    void proceed(bool ok);
private:
    enum class State { START_CALL, READING, FINISH };

    void processChunk();

    void mergeChunks();

    void cleanupTempFiles();


    QString             _savePath;
    QString             _businessType;
    QString             _tempDir;
    QString             _finalFileName;
    QMap<int, QString>  _chunkPaths;

    State               _state = State::START_CALL;
    FileRequest         _request;
    FileChunk           _chunk;

    FileStorageDTO      _dto;

    grpc::Status        _status;
    std::unique_ptr<grpc::ClientAsyncReader<FileChunk>> _reader;
    std::unique_ptr<FileTransferService::Stub> _stub;
    std::unique_ptr<grpc::ClientContext> _ctx;
    RealtimeCommHandler * _handler{};
};

#endif //ASYNCDOWNLOADTASK_H
