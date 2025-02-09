//
// Created by FUQAQ on 2025/2/6.
//

#ifndef STICKERDAO_H
#define STICKERDAO_H

#include "../DataProcessDAO.h"

class BaseStickerDAO : public IBaseStickerDAO {
public:
    BaseStickerDAO() {
        m_conn = ConnectionPool::getConnectPool()->getConnection();
        if (!m_conn) {
            LOG_ERROR("Failed to get MySQL connection from pool.");
        }
    }

    virtual ~BaseStickerDAO() = default;

    int64_t insert(const BaseStickerDO& sticker) override;

    bool deleteById(int64_t stickerId) override;

    std::vector<BaseStickerDO> listBaseStickers() override;
private:
    std::shared_ptr<MysqlConn> m_conn;
};

class UserCollectedStickerDAO : public IUserCollectedStickerDAO {
public:
    UserCollectedStickerDAO() {
        m_conn = ConnectionPool::getConnectPool()->getConnection();
        if (!m_conn) {
            LOG_ERROR("Failed to get MySQL connection from pool.");
        }
    }

    virtual ~UserCollectedStickerDAO() = default;

    bool collect(const UserCollectedStickerDO& sticker) override;

    bool unCollect(const std::string& userSsid, const std::string& imageUrl) override;

    std::vector<UserCollectedStickerDO> listByUser(const std::string& userSsid, int pageSize, int pageNum) override;
private:
    std::shared_ptr<MysqlConn> m_conn;
};

#endif //STICKERDAO_H
