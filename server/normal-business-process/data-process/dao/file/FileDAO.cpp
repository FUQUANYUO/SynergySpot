//
// Created by FUQAQ on 2025/2/6.
//

#include "FileDAO.h"

bool FileStorageDAO::deleteFile(const std::string &fileId) {
    std::string sql = "DELETE FROM file_storage WHERE file_id = ?";
    std::vector<MysqlConn::Param> params;
    MysqlConn::Param param;
    param.type = MysqlConn::Param::STRING;
    param.str_val = fileId;
    params.push_back(param);

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to delete file: " << fileId);
        return false;
    }
    return true;
}

bool FileStorageDAO::upload(const FileStorageDO &file) {

    std::string sql = "INSERT INTO file_storage (file_id, uploader_ssid, file_name, file_size, file_type, storage_path) "
                      "VALUES (?, ?, ?, ?, ?, ?)";
    std::vector<MysqlConn::Param> params;

    // Bind parameters
    MysqlConn::Param paramFileId;
    paramFileId.type = MysqlConn::Param::STRING;
    paramFileId.str_val = file.fileId;
    params.push_back(paramFileId);

    MysqlConn::Param paramUploader;
    paramUploader.type = MysqlConn::Param::STRING;
    paramUploader.str_val = file.uploaderSsid;
    params.push_back(paramUploader);

    MysqlConn::Param paramFileName;
    paramFileName.type = MysqlConn::Param::STRING;
    paramFileName.str_val = file.fileName;
    params.push_back(paramFileName);

    MysqlConn::Param paramFileSize;
    paramFileSize.type = MysqlConn::Param::INT;
    paramFileSize.int_val = file.fileSize;
    params.push_back(paramFileSize);

    MysqlConn::Param paramFileType;
    paramFileType.type = MysqlConn::Param::STRING;
    paramFileType.str_val = file.fileType;
    params.push_back(paramFileType);

    MysqlConn::Param paramStoragePath;
    paramStoragePath.type = MysqlConn::Param::STRING;
    paramStoragePath.str_val = file.storagePath;
    params.push_back(paramStoragePath);

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to upload file: " << file.fileId);
        return false;
    }
    return true;
}

FileStorageDO FileStorageDAO::findById(const std::string &fileId) {
    std::string sql = "SELECT file_id, uploader_ssid, file_name, file_size, file_type, storage_path, UNIX_TIMESTAMP(upload_time) FROM file_storage WHERE file_id = ?";
    std::vector<MysqlConn::Param> params;
    MysqlConn::Param param;
    param.type = MysqlConn::Param::STRING;
    param.str_val = fileId;
    params.push_back(param);

    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG_ERROR("Failed to find file by id: " << fileId);
        return {};
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        mysql_free_result(result);
        LOG_WARNING("File not found: " << fileId);
        return {};
    }

    FileStorageDO file;
    file.fileId = row[0];
    file.uploaderSsid = row[1];
    file.fileName = row[2];
    file.fileSize = std::stoll(row[3]);
    file.fileType = row[4];
    file.storagePath = row[5];
    file.uploadTime = std::stoll(row[6]);

    mysql_free_result(result);
    return file;
}

FileStorageDO FileStorageDAO::findByPath(const std::string &path) {
    std::string sql = "SELECT file_id, uploader_ssid, file_name, file_size, file_type, storage_path, UNIX_TIMESTAMP(upload_time) FROM file_storage WHERE storage_path = ?";
    std::vector<MysqlConn::Param> params;
    MysqlConn::Param param;
    param.type = MysqlConn::Param::STRING;
    param.str_val = path;
    params.push_back(param);

    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG_ERROR("Failed to find file by path: " << path);
        return {};
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        mysql_free_result(result);
        LOG_WARNING("File not found: " << path);
        return {};
    }

    FileStorageDO file;
    file.fileId = row[0];
    file.uploaderSsid = row[1];
    file.fileName = row[2];
    file.fileSize = std::stoll(row[3]);
    file.fileType = row[4];
    file.storagePath = row[5];
    file.uploadTime = std::stoll(row[6]);

    mysql_free_result(result);
    return file;
}

std::vector<FileStorageDO> FileStorageDAO::findByName(const std::string &fileName,int pageSize, int pageNum) {
    std::string sql = "SELECT file_id, uploader_ssid, file_name, file_size, file_type, storage_path, UNIX_TIMESTAMP(upload_time) FROM file_storage "
                      "WHERE file_name = ? "
                      "LIMIT ? OFFSET ?";
    std::vector<MysqlConn::Param> params;
    MysqlConn::Param paramFileName;
    paramFileName.type = MysqlConn::Param::STRING;
    paramFileName.str_val = fileName;
    params.push_back(paramFileName);

    MysqlConn::Param paramPageSize;
    paramPageSize.type = MysqlConn::Param::INT;
    paramPageSize.int_val = pageSize;
    params.push_back(paramPageSize);

    MysqlConn::Param paramOffset;
    paramOffset.type = MysqlConn::Param::INT;
    paramOffset.int_val = (pageNum - 1) * pageSize;
    params.push_back(paramOffset);

    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG_ERROR("Failed to find file by name: " << fileName);
        return {};
    }

    std::vector<FileStorageDO> files;
    MYSQL_ROW row;
    while (row = mysql_fetch_row(result)) {
        FileStorageDO file;
        file.fileId = row[0];
        file.uploaderSsid = row[1];
        file.fileName = row[2];
        file.fileSize = std::stoll(row[3]);
        file.fileType = row[4];
        file.storagePath = row[5];
        file.uploadTime = std::stoll(row[6]);
        files.push_back(file);
    }
    mysql_free_result(result);
    return files;
}

std::vector<FileStorageDO> FileStorageDAO::findBySSID(const std::string &userSSID,int pageSize, int pageNum) {
    std::string sql = "SELECT file_id, uploader_ssid, file_name, file_size, file_type, storage_path, UNIX_TIMESTAMP(upload_time) FROM file_storage "
                      "WHERE uploader_ssid = ? "
                      "LIMIT ? OFFSET ?";
    std::vector<MysqlConn::Param> params;
    MysqlConn::Param paramSSID;
    paramSSID.type = MysqlConn::Param::STRING;
    paramSSID.str_val = userSSID;
    params.push_back(paramSSID);

    MysqlConn::Param paramPageSize;
    paramPageSize.type = MysqlConn::Param::INT;
    paramPageSize.int_val = pageSize;
    params.push_back(paramPageSize);

    MysqlConn::Param paramOffset;
    paramOffset.type = MysqlConn::Param::INT;
    paramOffset.int_val = (pageNum - 1) * pageSize;
    params.push_back(paramOffset);

    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG_ERROR("Failed to find file by name: " << userSSID);
        return {};
    }

    std::vector<FileStorageDO> files;
    MYSQL_ROW row;
    while (row = mysql_fetch_row(result)) {
        FileStorageDO file;
        file.fileId = row[0];
        file.uploaderSsid = row[1];
        file.fileName = row[2];
        file.fileSize = std::stoll(row[3]);
        file.fileType = row[4];
        file.storagePath = row[5];
        file.uploadTime = std::stoll(row[6]);
        files.push_back(file);
    }
    mysql_free_result(result);
    return files;
}