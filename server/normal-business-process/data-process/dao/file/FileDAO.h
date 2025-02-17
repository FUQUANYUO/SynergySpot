//
// Created by FUQAQ on 2025/2/6.
//

#ifndef FILEDAO_H
#define FILEDAO_H

#include "../DataProcessDAO.h"

class FileStorageDAO : public IFileStorageDAO {
public:
    FileStorageDAO() {
        m_conn = ConnectionPool::getConnectPool()->getConnection();
        if (!m_conn) {
            LOG_ERROR("Failed to get MySQL connection from pool.");
        }
    }

    virtual ~FileStorageDAO() = default;

    bool deleteFile(const std::string& fileId) override;

    bool upload(const FileStorageDO& file) override;

    FileStorageDO findById(const std::string& fileId) override;

    std::vector<FileStorageDO> findByName(const std::string& fileName,int pageSize, int pageNum);

    std::vector<FileStorageDO> findBySSID(const std::string& userSSID,int pageSize, int pageNum);
private:
    std::shared_ptr<MysqlConn> m_conn;
};


#endif //FILEDAO_H
