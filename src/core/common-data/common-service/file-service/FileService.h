//
// Created by FUQAQ on 2025/2/20.
//

#ifndef FILESERVICE_H
#define FILESERVICE_H

#include "../../common-dao/file-dao/FileStorageDAO.h"
#include "../../common-dto/CommonDatabaseDTO.h"

class FileService {
public:
    explicit FileService(LiteConn& db);

    // add file
    bool addFile(const FileStorageDTO& dto);

    // remove file
    bool removeFile(const QString &ssid);

    // get file
    QList<FileStorageDTO> getFileByUserSSID(const QString& userSSID,int pageSize, int pageNum);

    QList<FileStorageDTO> getFileByFileName(const QString& fileName,int pageSize, int pageNum);

    FileStorageDTO getFileByFileID(const QString& fileID);

    FileStorageDTO getFileByFilePath(const QString& filePath);

private:
    FileStorageDAO fileDAO;
};



#endif //FILESERVICE_H
