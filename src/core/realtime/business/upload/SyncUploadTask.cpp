#include "SyncUploadTask.h"

#include <grpcpp/client_context.h>

SyncUploadTask::SyncUploadTask(
    RealtimeCommHandler* handler,
    std::unique_ptr<FileTransferService::Stub> stub,
    const QString& localUrl,
    const QString& type,
    FileStorageDTO dto
) : _handler(handler), _stub(std::move(stub)),
    _localUrl(localUrl), _type(type), _dto(std::move(dto))
{
    _file.setFileName(_localUrl);
    if (!_file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("Cannot open file: " + _localUrl.toStdString());
    }
}

bool SyncUploadTask::execute() {
    try {
        initUpload();
        sendChunks();
        _handler->onUploadSuccess(this);
        return true;
    } catch (const std::exception& e) {
        cleanup();
        _handler->onUploadFailed(e.what());
        return false;
    }
}

void SyncUploadTask::initUpload() {
    grpc::ClientContext ctx;
    FileInitRequest req;
    req.set_file_name(QFileInfo(_localUrl).fileName().toStdString());
    req.set_file_size(_file.size());
    req.set_type(getBusinessType());
    req.set_ssid(_dto.uploaderSsid.toStdString());

    FileInitResponse res;
    grpc::Status status = _stub->InitUpload(&ctx, req, &res);

    if (!status.ok() || !res.success()) {
        throw std::runtime_error("Init failed: " + status.error_message());
    }

    _fileId = QString::fromStdString(res.file_id());
    _dto.fileId = _fileId;
    _dto.fileSize = _file.size();
    _dto.fileName = QFileInfo(_localUrl).fileName();
    _dto.fileType = _type;
    _dto.uploadTime = GetCurTime::getTimeObj()->getCurTimeStamp();

    _totalChunks = (_file.size() + RealtimeCommHandler::CHUNK_SIZE - 1) / RealtimeCommHandler::CHUNK_SIZE;
}

void SyncUploadTask::sendChunks() {
    grpc::ClientContext ctx;
    FileResponse response;
    std::unique_ptr<grpc::ClientWriter<FileChunk>> writer(
        _stub->UploadFile(&ctx, &response)
    );

    while (!_file.atEnd()) {
        QByteArray chunkData = _file.read(RealtimeCommHandler::CHUNK_SIZE);
        if (chunkData.isEmpty()) {
            throw std::runtime_error("Failed to read chunk " + std::to_string(_currentChunk));
        }

        FileChunk chunk;
        chunk.set_file_id(_fileId.toStdString());
        chunk.set_chunk_number(_currentChunk);
        chunk.set_data(chunkData.constData(), chunkData.size());
        chunk.set_checksum(_handler->calculateChunkMD5(chunkData).toStdString());

        if (!writer->Write(chunk)) {
            throw std::runtime_error("Chunk write failed at " + std::to_string(_currentChunk));
        }

        _currentChunk++;
        int progress = static_cast<int>(_currentChunk * 100.0 / _totalChunks);
        QMetaObject::invokeMethod(_handler, "sigUploadProgress",
                                Qt::QueuedConnection, Q_ARG(int, progress));
    }

    writer->WritesDone();
    grpc::Status status = writer->Finish();
    if (!status.ok() || !response.success()) {
        throw std::runtime_error("Upload failed: " + status.error_message());
    }
}

FileBusinessType SyncUploadTask::getBusinessType() {
    if (_type == "avatar") return FileBusinessType::AVATAR;
    if (_type == "file_storage") return FileBusinessType::FILE_STORAGE;
    if (_type == "msg_pic") return FileBusinessType::MSG_PIC;
    throw std::invalid_argument("Invalid type: " + _type.toStdString());
}