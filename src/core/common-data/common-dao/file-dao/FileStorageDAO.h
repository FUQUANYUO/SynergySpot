//
// Created by FUQAQ on 2025/2/19.
//

#ifndef FILESTORAGEDAO_H
#define FILESTORAGEDAO_H

#include "../CommonDataBaseDAO.h"

class FileStorageDAO : public IFileStorageDAO {
public:
    FileStorageDAO(LiteConn& db);

    virtual ~FileStorageDAO();

    bool deleteFile(const QString& fileId) override;

    bool upload(const FileStorageDO& file) override;

    FileStorageDO findById(const QString& fileId) override;

    FileStorageDO findByPath(const QString& path);

    QList<FileStorageDO> findByName(const QString& fileName,int pageSize, int pageNum);

    QList<FileStorageDO> findBySSID(const QString& userSSID,int pageSize, int pageNum);
private:
    LiteConn& _db;
};

#endif //FILESTORAGEDAO_H
