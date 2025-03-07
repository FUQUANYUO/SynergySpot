//
// Created by FUQAQ on 2025/3/1.
//

#ifndef SYNCUPLOADTASK_H
#define SYNCUPLOADTASK_H

#include "../../RealtimeCommHandler.h"
#include <QFileInfo>

class SyncUploadTask {
    friend class RealtimeCommHandler;
public:
    SyncUploadTask(
        RealtimeCommHandler* handler,
        std::unique_ptr<FileTransferService::Stub> stub,
        const QString& localUrl,
        const QString& type,
        FileStorageDTO dto
    );

    bool execute();

private:
    void initUpload();
    void sendChunks();
    void cleanup() { if (_file.isOpen()) _file.close(); }
    FileBusinessType getBusinessType();

    RealtimeCommHandler* _handler;
    std::unique_ptr<FileTransferService::Stub> _stub;

    FileStorageDTO _dto;
    QString _localUrl;
    QString _type;
    QString _fileId;
    QFile _file;
    int _currentChunk = 0, _totalChunks = 0;
};

#endif //SYNCUPLOADTASK_H
