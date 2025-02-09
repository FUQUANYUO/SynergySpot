//
// Created by FUQAQ on 2025/2/6.
//

#ifndef USERDAO_H
#define USERDAO_H

#include "../DataProcessDAO.h"

class UserDAO : public IUserDAO {
public:
    UserDAO() {
        m_conn = ConnectionPool::getConnectPool()->getConnection();
        if (!m_conn) {
            LOG_ERROR("Failed to get MySQL connection from pool.");
        }
    }

    virtual ~UserDAO() = default;

    bool insert(const UserBaseInfoDO& user) override;

    bool update(const UserBaseInfoDO& user) override;

    bool deleteById(const std::string& ssid) override;

    UserBaseInfoDO findById(const std::string& ssid) override;

    std::vector<UserBaseInfoDO> fuzzyMatchingByIdOrName(const std::string& ssid, const std::string& name);

    bool updateThumbUpCount(const std::string& ssid, int newCount) override;

    std::vector<UserBaseInfoDO> findByRegion(uint8_t region, int pageSize, int pageNum) override;

    int getRegionCount(uint8_t region) override;

    std::string getLastUserSSID();
private:
    std::shared_ptr<MysqlConn> m_conn;
};

class UserPrivateDAO : public IUserPrivateDAO {
public:
    UserPrivateDAO() {
        m_conn = ConnectionPool::getConnectPool()->getConnection();
        if (!m_conn) {
            LOG_ERROR("Failed to get MySQL connection from pool.");
        }
    }

    virtual ~UserPrivateDAO() = default;

    bool insert(const UserPrivateInfoDO& user) override;

    bool update(const UserPrivateInfoDO& user) override;

    bool deleteById(const std::string& ssid) override;

    UserPrivateInfoDO findById(const std::string& ssid) override;
private:
    std::shared_ptr<MysqlConn> m_conn;
};


#endif //USERDAO_H
