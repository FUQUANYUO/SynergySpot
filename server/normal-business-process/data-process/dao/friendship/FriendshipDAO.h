//
// Created by FUQAQ on 2025/2/6.
//

#ifndef FRIENDSHIPDAO_H
#define FRIENDSHIPDAO_H

#include "../DataProcessDAO.h"

class FriendshipDAO : public IFriendshipDAO {
public:
    FriendshipDAO() {
        m_conn = ConnectionPool::getConnectPool()->getConnection();
        if (!m_conn) {
            LOG_ERROR("Failed to get MySQL connection from pool.");
        }
    }

    virtual ~FriendshipDAO() = default;

    int64_t create(const FriendshipDO& friendship) override;

    bool update(const FriendshipDO& friendship) override;

    bool deleteById(const std::string& ssid, const std::string& friendSsid) override;

    FriendshipDO findRelationship(const std::string& ssid, const std::string& friendSsid) override;

    std::vector<FriendshipDO> listByUser(const std::string& ssid) override;

    int getFriendshipCount(const std::string& ssid) override;
private:
    std::shared_ptr<MysqlConn> m_conn;
};



#endif //FRIENDSHIPDAO_H
