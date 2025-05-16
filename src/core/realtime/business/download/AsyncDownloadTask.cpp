//
// Created by FUQAQ on 2025/3/1.
//

#include "AsyncDownloadTask.h"

#include "grpcpp/completion_queue.h"
#include "uuid/GenUUID.h"

#include <QDir>

AsyncDownloadTask::AsyncDownloadTask(
        RealtimeCommHandler *handler,
        std::unique_ptr<FileTransferService::Stub> stub,
        const FileStorageDTO &dto,
        const QString &savePath,
        const QString &type)
    : _handler(handler),
    _stub(std::move(stub)),
    _dto(dto),
    _savePath(savePath),
    _businessType(type),
    _ctx(new grpc::ClientContext)
{
    // 初始化请求参数
    _request.set_file_id(_dto.fileId.toStdString());
    _request.set_file_path(_dto.storagePath.toStdString());

    // 启动异步调用
    _reader = _stub->PrepareAsyncDownloadFile(_ctx.get(), _request, _handler->getCompletionQueue());
    _reader->StartCall(this);

    // 创建临时目录（提前创建避免多次IO）
    _tempDir = QString("%1/%2-%3").arg(
        QString::fromStdString(g_pCommonData->getDataPath(file)),
        QString::fromStdString(GetCurTime::getTimeObj()->getCurTime("%Y-%m-%d")),
        QString::fromStdString(g_pGenUUID->generateUUID("file_tmp"))
    );
    if (!QDir().mkpath(_tempDir)) {
        LOG_ERROR("create tmp dir failed!")
    }
    LOG_INFO("Download task created for file: " << _dto.fileId.toStdString());
}

AsyncDownloadTask::~AsyncDownloadTask() {
    cleanupTempFiles();
}

void AsyncDownloadTask::proceed(bool ok) {
    try {
        switch (_state) {
            case State::START_CALL:
                if (!ok) {
                    throw std::runtime_error("gRPC connection error in START_CALL");
                }
                _state = State::READING;
                _reader->Read(&_chunk, this);
                break;

            case State::READING:
                if (ok) {
                    processChunk();
                    _reader->Read(&_chunk, this);
                } else {
                    _state = State::FINISH;
                    _reader->Finish(&_status, this);
                }
                break;

            case State::FINISH:
                if (_status.ok()) {
                    mergeChunks();
                    _handler->onDownloadSuccess(this);
                } else {
                    throw std::runtime_error(_status.error_message());
                }
                delete this;
                break;
        }
    } catch (const std::exception& e) {
        cleanupTempFiles();
        _handler->onDownloadFailed(e.what());
        delete this;
    }
}


void AsyncDownloadTask::processChunk() {
    LOG_INFO("process chunk");
    // 首次接收初始化文件名
    if (_finalFileName.isEmpty()) {
        _finalFileName = QString("%1/%2").arg(_tempDir).arg(
            QString::fromStdString(_chunk.file_id())
        );
    }

    // 校验MD5
    QByteArray data(_chunk.data().data(), _chunk.data().size());
    QString receivedMd5 = QString::fromStdString(_chunk.checksum());
    QString calculatedMd5 = _handler->calculateChunkMD5(data);
    if (receivedMd5 != calculatedMd5) {
        LOG_ERROR("MD5 verify failed for chunk: " + std::to_string(_chunk.chunk_number()))
        throw std::runtime_error(
            "MD5 verify failed for chunk: " + std::to_string(_chunk.chunk_number())
        );
    }

    // 保存分块到临时文件
    QString chunkPath = QString("%1.tmp_%2").arg(_finalFileName).arg(_chunk.chunk_number());
    QFile chunkFile(chunkPath);
    if (!chunkFile.open(QIODevice::WriteOnly)) {
        throw std::runtime_error("Cannot write tmp chunk: " + chunkPath.toStdString());
    }
    chunkFile.write(data);
    chunkFile.close();
    _chunkPaths[_chunk.chunk_number()] = chunkPath;

    // 发射进度信号（跨线程安全）
    int progress = static_cast<int>(
        _chunkPaths.size() * 100.0 / _chunk.total_chunks()
    );
    LOG_INFO( _finalFileName.toStdString() + " progress: " << progress);
    QMetaObject::invokeMethod(_handler, "sigDownloadProgress",
        Qt::QueuedConnection, Q_ARG(int, progress));
}

void AsyncDownloadTask::mergeChunks() {
    QFileInfo fileInfo(_savePath);
    QDir parentDir = fileInfo.absoluteDir();
    if (!parentDir.exists() && !parentDir.mkpath(".")) {
        LOG_ERROR("Cannot create parent directory: " + parentDir.path().toStdString());
    }

    QFile finalFile(_savePath);
    if (!finalFile.open(QIODevice::WriteOnly)) {
        // 添加详细错误信息
        LOG_ERROR(
            "Cannot create final file: " + _savePath.toStdString() +
            ", Error: " + finalFile.errorString().toStdString()
        );
    }
    
    // 按顺序合并
    for (int i = 0; i < _chunkPaths.size(); ++i) {
        QFile chunkFile(_chunkPaths[i]);
        if (!chunkFile.open(QIODevice::ReadOnly)) {
            finalFile.close();
            QFile::remove(_savePath);
            throw std::runtime_error("Cannot read chunk: " + _chunkPaths[i].toStdString());
        }
        finalFile.write(chunkFile.readAll());
        chunkFile.close();
        QFile::remove(_chunkPaths[i]);
    }
    finalFile.close();
}
void AsyncDownloadTask::cleanupTempFiles() {
    for (const auto& path : _chunkPaths) {
        QFile::remove(path);
    }
}