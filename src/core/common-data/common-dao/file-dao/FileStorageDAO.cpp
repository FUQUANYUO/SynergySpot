//
// Created by FUQAQ on 2025/2/19.
//

#include "FileStorageDAO.h"

FileStorageDAO::FileStorageDAO(LiteConn &db) : _db(db){}

FileStorageDAO::~FileStorageDAO() {}

bool FileStorageDAO::deleteFile(const QString &fileId) {
    std::string sql = "DELETE FROM file_storage WHERE file_id = ?";
    std::vector<std::string> params = {fileId.toStdString()};

    if (!_db.update(sql, params)) {
        LOG_ERROR("Failed to delete file: " << fileId.toStdString());
        return false;
    }
    return true;
}

bool FileStorageDAO::upload(const FileStorageDO &file) {
    std::string sql = "INSERT INTO file_storage (file_id, uploader_ssid, file_name, file_size, file_type, storage_path, upload_time) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?)";

    std::vector<std::string> params = {
        file.fileId.toStdString(),
        file.uploaderSsid.toStdString(),
        file.fileName.toStdString(),
        std::to_string(file.fileSize),
        file.fileType.toStdString(),
        file.storagePath.toStdString(),
        std::to_string(file.uploadTime)
    };

    return _db.update(sql, params);
}

FileStorageDO FileStorageDAO::findById(const QString &fileId) {
    std::string sql = "SELECT file_id, uploader_ssid, file_name, file_size, file_type, storage_path, upload_time "
                      "FROM file_storage WHERE file_id = ?";
    auto result = _db.query(sql, {fileId.toStdString()});

    if (!result.empty() && result[0].size() >= 7) {
        const auto& row = result[0];
        FileStorageDO file;

        file.fileId       = QString::fromStdString(row[0]);
        file.uploaderSsid = QString::fromStdString(row[1]);
        file.fileName     = QString::fromStdString(row[2]);
        file.fileSize     = std::stol(row[3]);
        file.fileType     = QString::fromStdString(row[4]);
        file.storagePath  = QString::fromStdString(row[5]);
        file.uploadTime   = std::stoll(row[6]);
        return file;
    }
    return {};
}

FileStorageDO FileStorageDAO::findByPath(const QString &path) {
    std::string sql = "SELECT file_id, uploader_ssid, file_name, file_size, file_type, storage_path, upload_time "
                  "FROM file_storage WHERE storage_path = ?";
    auto result = _db.query(sql, {path.toStdString()});

    if (!result.empty() && result[0].size() >= 7) {
        const auto& row = result[0];
        FileStorageDO file;

        file.fileId       = QString::fromStdString(row[0]);
        file.uploaderSsid = QString::fromStdString(row[1]);
        file.fileName     = QString::fromStdString(row[2]);
        file.fileSize     = std::stol(row[3]);
        file.fileType     = QString::fromStdString(row[4]);
        file.storagePath  = QString::fromStdString(row[5]);
        file.uploadTime   = std::stoll(row[6]);
        return file;
    }
    return {};
}

QList<FileStorageDO> FileStorageDAO::findByName(const QString &fileName, int pageSize, int pageNum) {
    std::string sql = "SELECT file_id, uploader_ssid, file_name, file_size, file_type, storage_path, upload_time "
                  "FROM file_storage WHERE file_name = ? LIMIT ? OFFSET ?";

    std::vector<std::string> params = {
        fileName.toStdString(),
        std::to_string(pageSize),
        std::to_string((pageNum - 1) * pageSize)
    };

    auto result = _db.query(sql, params);
    QList<FileStorageDO> files;

    for (const auto& row : result) {
        FileStorageDO file;
        file.fileId = QString::fromStdString(row[0]);
        file.uploaderSsid = QString::fromStdString(row[1]);
        file.fileName = QString::fromStdString(row[2]);
        file.fileSize = std::stoll(row[3]);
        file.fileType = QString::fromStdString(row[4]);
        file.storagePath = QString::fromStdString(row[5]);
        file.uploadTime = std::stoll(row[6]);
        files.push_back(file);
    }
    return files;
}

QList<FileStorageDO> FileStorageDAO::findBySSID(const QString &userSSID, int pageSize, int pageNum) {
    std::string sql = "SELECT file_id, uploader_ssid, file_name, file_size, file_type, storage_path, upload_time "
                  "FROM file_storage WHERE uploader_ssid = ? LIMIT ? OFFSET ?";

    std::vector<std::string> params = {
        userSSID.toStdString(),
        std::to_string(pageSize),
        std::to_string((pageNum - 1) * pageSize)
    };

    auto result = _db.query(sql, params);
    QList<FileStorageDO> files;

    for (const auto& row : result) {
        FileStorageDO file;
        file.fileId = QString::fromStdString(row[0]);
        file.uploaderSsid = QString::fromStdString(row[1]);
        file.fileName = QString::fromStdString(row[2]);
        file.fileSize = std::stoll(row[3]);
        file.fileType = QString::fromStdString(row[4]);
        file.storagePath = QString::fromStdString(row[5]);
        file.uploadTime = std::stoll(row[6]);
        files.push_back(file);
    }
    return files;
}