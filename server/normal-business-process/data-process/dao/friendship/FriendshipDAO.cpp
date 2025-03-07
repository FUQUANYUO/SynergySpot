//
// Created by FUQAQ on 2025/2/6.
//

#include "FriendshipDAO.h"

int64_t FriendshipDAO::create(const FriendshipDO &friendship) {
    std::string sql = "INSERT INTO friendship (ssid, `grouping`, friend_ssid, ship_status, friend_type) "
                      "VALUES (?, ?, ?, ?, ?)";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramSsid;
    paramSsid.type = MysqlConn::Param::STRING;
    paramSsid.str_val = friendship.ssid;
    params.push_back(paramSsid);

    MysqlConn::Param paramGrouping;
    paramGrouping.type = MysqlConn::Param::STRING;
    paramGrouping.str_val = friendship.grouping;
    params.push_back(paramGrouping);

    MysqlConn::Param paramFriendSsid;
    paramFriendSsid.type = MysqlConn::Param::STRING;
    paramFriendSsid.str_val = friendship.friendSsid;
    params.push_back(paramFriendSsid);

    MysqlConn::Param paramShipStatus;
    paramShipStatus.type = MysqlConn::Param::INT;
    paramShipStatus.int_val = friendship.shipStatus;
    params.push_back(paramShipStatus);

    MysqlConn::Param paramFriendType;
    paramFriendType.type = MysqlConn::Param::INT;
    paramFriendType.int_val = friendship.friendType;
    params.push_back(paramFriendType);

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to create friendship: " << friendship.ssid << " - " << friendship.friendSsid);
        return -1;
    }

    return static_cast<int64_t>(m_conn->getLastInsertId());
}

bool FriendshipDAO::update(const FriendshipDO &friendship) {
    std::string sql = "UPDATE friendship SET ";
    std::vector<std::string> setClauses;
    std::vector<MysqlConn::Param> params;

    // 检查并添加 grouping
    if (!friendship.grouping.empty() && friendship.grouping != "-1") {
        setClauses.push_back("`grouping` = ?");
        MysqlConn::Param paramGrouping;
        paramGrouping.type = MysqlConn::Param::STRING;
        paramGrouping.str_val = friendship.grouping;
        params.push_back(paramGrouping);
    }

    // 检查并添加 remark
    if (!friendship.remark.empty() && friendship.remark != "-1") {
        setClauses.push_back("remark = ?");
        MysqlConn::Param paramRemark;
        paramRemark.type = MysqlConn::Param::STRING;
        paramRemark.str_val = friendship.remark;
        params.push_back(paramRemark);
    }

    // 检查并添加 shipStatus
    if (friendship.shipStatus != 0) { // 假设 0 是无效值
        setClauses.push_back("ship_status = ?");
        MysqlConn::Param paramShipStatus;
        paramShipStatus.type = MysqlConn::Param::INT;
        paramShipStatus.int_val = friendship.shipStatus;
        params.push_back(paramShipStatus);
    }

    // 如果没有有效的字段需要更新，直接返回
    if (setClauses.empty()) {
        LOG_WARNING("No valid fields to update for friendship between "
                    << friendship.ssid << " and " << friendship.friendSsid);
        return false;
    }

    for (size_t i = 0; i < setClauses.size(); ++i) {
        sql += setClauses[i];
        if (i != setClauses.size() - 1) {
            sql += ", ";
        }
    }

    sql += " WHERE ssid = ? AND friend_ssid = ?";

    // 添加 ssid 和 friend_ssid 参数
    MysqlConn::Param paramSsid;
    paramSsid.type = MysqlConn::Param::STRING;
    paramSsid.str_val = friendship.ssid;
    params.push_back(paramSsid);

    MysqlConn::Param paramFriendSsid;
    paramFriendSsid.type = MysqlConn::Param::STRING;
    paramFriendSsid.str_val = friendship.friendSsid;
    params.push_back(paramFriendSsid);

    // 执行更新操作
    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to update status for friendship between "
                  << friendship.ssid << " and " << friendship.friendSsid);
        return false;
    }
    return true;
}

bool FriendshipDAO::deleteById(const std::string &ssid, const std::string &friendSsid){
    std::string sql = "DELETE FROM friendship WHERE ssid = ? AND friend_ssid = ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramSsid;
    paramSsid.type = MysqlConn::Param::STRING;
    paramSsid.str_val = ssid;
    params.push_back(paramSsid);

    MysqlConn::Param paramFriendSsid;
    paramFriendSsid.type = MysqlConn::Param::STRING;
    paramFriendSsid.str_val = friendSsid;
    params.push_back(paramFriendSsid);

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to delete friendship: " << ssid << " - " << friendSsid);
        return false;
    }

    return true;
}

FriendshipDO FriendshipDAO::findRelationship(const std::string &ssid, const std::string &friendSsid)  {
    std::string sql = "SELECT id, ssid, `grouping`, remark ,friend_ssid, ship_status, friend_type, UNIX_TIMESTAMP(create_time) "
                      "FROM friendship WHERE ssid = ? AND friend_ssid = ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramSsid;
    paramSsid.type = MysqlConn::Param::STRING;
    paramSsid.str_val = ssid;
    params.push_back(paramSsid);

    MysqlConn::Param paramFriendSsid;
    paramFriendSsid.type = MysqlConn::Param::STRING;
    paramFriendSsid.str_val = friendSsid;
    params.push_back(paramFriendSsid);

    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG_ERROR("Failed to find friendship: " << ssid << " - " << friendSsid);
        return {-1};
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        mysql_free_result(result);
        return {-1};
    }

    FriendshipDO friendship;
    friendship.id = std::stoll(row[0]);
    friendship.ssid = row[1];
    friendship.grouping = row[2];
    friendship.remark = row[3];
    friendship.friendSsid = row[4];
    friendship.shipStatus = static_cast<uint8_t>(std::stoul(row[5]));
    friendship.friendType = static_cast<uint8_t>(std::stoul(row[6]));
    friendship.createTime = row[6] ? std::stoll(row[6]) : 0;

    mysql_free_result(result);
    return friendship;
}

std::vector<FriendshipDO> FriendshipDAO::listByUser(const std::string &ssid)  {
    std::string sql = "SELECT id, ssid, `grouping`, remark, friend_ssid, ship_status, friend_type, UNIX_TIMESTAMP(create_time) "
                      "FROM friendship WHERE ssid = ? ORDER BY UNIX_TIMESTAMP(create_time) DESC";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramSsid;
    paramSsid.type = MysqlConn::Param::STRING;
    paramSsid.str_val = ssid;
    params.push_back(paramSsid);

    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG_ERROR("Failed to list friendships for user: " << ssid);
        return {};
    }

    std::vector<FriendshipDO> friendships;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        FriendshipDO friendship;
        friendship.id = std::stoll(row[0]);
        friendship.ssid = row[1];
        friendship.grouping = row[2];
        friendship.remark = row[3];
        friendship.friendSsid = row[4];
        friendship.shipStatus = static_cast<uint8_t>(std::stoul(row[5]));
        friendship.friendType = static_cast<uint8_t>(std::stoul(row[6]));
        friendship.createTime = row[7] ? std::stoul(row[7]) : 0;
        friendships.push_back(friendship);
    }

    mysql_free_result(result);
    return friendships;
}

int FriendshipDAO::getFriendshipCount(const std::string &ssid)  {
    std::string sql = "SELECT COUNT(*) FROM friendship WHERE ssid = ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramSsid;
    paramSsid.type = MysqlConn::Param::STRING;
    paramSsid.str_val = ssid;
    params.push_back(paramSsid);

    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG_ERROR("Failed to get friendship count for user: " << ssid);
        return 0;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    int count = 0;
    if (row && row[0]) {
        count = std::stoi(row[0]);
    }

    mysql_free_result(result);
    return count;
}