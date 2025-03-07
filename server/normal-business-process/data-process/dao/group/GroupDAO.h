//
// Created by FUQAQ on 2025/2/6.
//

#ifndef GROUPDAO_H
#define GROUPDAO_H

#include "../DataProcessDAO.h"

class GroupBaseInfoDAO : public IGroupBaseInfoDAO {
public:
    GroupBaseInfoDAO() {
        m_conn = ConnectionPool::getConnectPool()->getConnection();
        if (!m_conn) {
            LOG_ERROR("Failed to get MySQL connection from pool.");
        }
    }

    virtual ~GroupBaseInfoDAO() = default;

    int64_t createGroup(const GroupBaseInfoDO& group) override;

    bool update(const GroupBaseInfoDO& group) override;

    bool deleteById(const std::string& groupSsid) override;

    GroupBaseInfoDO findBySsid(const std::string& groupSsid) override;

    std::string getLastGroupSSID();

    std::vector<GroupBaseInfoDO> fuzzyMatchingByIdOrName(const std::string& ssid, const std::string& name);

private:
    std::shared_ptr<MysqlConn> m_conn;
};


class GroupAdminDAO : public IGroupAdminDAO {
public:
    GroupAdminDAO() {
        m_conn = ConnectionPool::getConnectPool()->getConnection();
        if (!m_conn) {
            LOG_ERROR("Failed to get MySQL connection from pool.");
        }
    }

    virtual ~GroupAdminDAO() = default;

    bool addAdmin(int64_t groupId, const std::string& opSsid) override;

    bool removeAdmin(int64_t groupId, const std::string& opSsid) override;

    std::vector<GroupAdminDO> getAdmins(int64_t groupId) override;
private:
    std::shared_ptr<MysqlConn> m_conn;
};

class GroupNoticeDAO : public IGroupNoticeDAO {
public:
    GroupNoticeDAO() {
        m_conn = ConnectionPool::getConnectPool()->getConnection();
        if (!m_conn) {
            LOG_ERROR("Failed to get MySQL connection from pool.");
        }
    }

    virtual ~GroupNoticeDAO() = default;

    bool deleteNotice(int64_t groupId, int64_t noticeId) override;

    int64_t addNotice(int64_t groupId, const std::string& notice) override;

    std::vector<GroupNoticeDO> listNotices(int64_t groupId, int pageSize, int pageNum) override;
private:
    std::shared_ptr<MysqlConn> m_conn;
};

class GroupMemberDAO : public IGroupMemberDAO {
private:
    std::shared_ptr<MysqlConn> m_conn;

public:
    GroupMemberDAO() {
        m_conn = ConnectionPool::getConnectPool()->getConnection();
        if (!m_conn) {
            LOG_ERROR("Failed to get MySQL connection from pool.");
        }
    }

    virtual ~GroupMemberDAO() = default;

    bool addMember(const std::string& groupSsid, const std::string& memberSsid) override;

    bool removeMember(const std::string& groupSsid, const std::string& memberSsid) override;

    std::vector<GroupMemberInfoDO> listMembers(const std::string& groupSsid) override;

    int getMemberCount(const std::string& groupSsid) override;
};

#endif //GROUPDAO_H
