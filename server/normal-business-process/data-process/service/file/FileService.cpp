//
// Created by FUQAQ on 2025/2/6.
//

#include "FileService.h"

bool FileService::addFile(const FileStorageDTO &dto) {
    return fileDAO.upload({dto.fileId,dto.uploaderSsid,dto.fileName,
        dto.fileSize,dto.fileType,dto.storagePath,dto.uploadTime});
}

bool FileService::removeFile(const FileStorageDTO &dto) {
    return fileDAO.deleteFile(dto.fileId);
}

std::vector<FileStorageDTO> FileService::getFileByUserSSID(const std::string& userSSID, int pageSize, int pageNum) {
    std::vector<FileStorageDTO> files;
    std::vector<FileStorageDO> res = fileDAO.findBySSID(userSSID,pageSize,pageNum);
    files.reserve(res.size());
    for (const FileStorageDO& it : res) {
        files.push_back({
            it.fileId,it.uploaderSsid,it.fileName,it.fileSize,it.fileType,it.storagePath,it.uploadTime
        });
    }
    return files;
}

std::vector<FileStorageDTO> FileService::getFileByFileName(const std::string& fileName, int pageSize, int pageNum) {
    std::vector<FileStorageDTO> files;
    std::vector<FileStorageDO> res = fileDAO.findBySSID(fileName,pageSize,pageNum);
    files.reserve(res.size());
    for (const FileStorageDO& it : res) {
        files.push_back({
            it.fileId,it.uploaderSsid,it.fileName,it.fileSize,it.fileType,it.storagePath,it.uploadTime
        });
    }
    return files;
}

FileStorageDTO FileService::getFileByFileID(const std::string& fileID) {
    auto res = fileDAO.findById(fileID);
    return {res.fileId,res.uploaderSsid,res.fileName,res.fileSize,res.fileType,res.storagePath,res.uploadTime};
}
