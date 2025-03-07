//
// Created by FUQAQ on 2025/2/20.
//

#include "FileService.h"

FileService::FileService(LiteConn &db) : fileDAO(db){}

bool FileService::addFile(const FileStorageDTO &dto) {
    return fileDAO.upload({dto.fileId,dto.uploaderSsid,dto.fileName,dto.fileSize,dto.fileType,dto.storagePath,dto.uploadTime});
}

bool FileService::removeFile(const QString &ssid) {
    return fileDAO.deleteFile(ssid);
}

QList<FileStorageDTO> FileService::getFileByUserSSID(const QString &userSSID, int pageSize, int pageNum) {
    QList<FileStorageDTO> files;
    QList<FileStorageDO> res = fileDAO.findBySSID(userSSID,pageSize,pageNum);
    files.reserve(res.size());
    for (const FileStorageDO& it : res) {
        files.push_back({
            it.fileId,it.uploaderSsid,it.fileName,it.fileSize,it.fileType,it.storagePath,it.uploadTime
        });
    }
    return files;
}

QList<FileStorageDTO> FileService::getFileByFileName(const QString &fileName, int pageSize, int pageNum) {
    QList<FileStorageDTO> files;
    QList<FileStorageDO> res = fileDAO.findByName(fileName,pageSize,pageNum);
    files.reserve(res.size());
    for (const FileStorageDO& it : res) {
        files.push_back({
            it.fileId,it.uploaderSsid,it.fileName,it.fileSize,it.fileType,it.storagePath,it.uploadTime
        });
    }
    return files;
}

FileStorageDTO FileService::getFileByFileID(const QString &fileID) {
    auto res = fileDAO.findById(fileID);
    return {res.fileId,res.uploaderSsid,res.fileName,res.fileSize,res.fileType,res.storagePath,res.uploadTime};
}

FileStorageDTO FileService::getFileByFilePath(const QString &filePath) {
    auto res = fileDAO.findByPath(filePath);
    return {res.fileId,res.uploaderSsid,res.fileName,res.fileSize,res.fileType,res.storagePath,res.uploadTime};
}