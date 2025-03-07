//
// Created by FUQAQ on 2025/1/26.
//

#include "FriendshipDAO.h"


FriendshipDAO::FriendshipDAO(LiteConn &db) : _db(db) {}

FriendshipDAO::~FriendshipDAO() {}

qint64 FriendshipDAO::create(const FriendshipDO &friendship) {
    std::string sql = "INSERT INTO friendship (ssid, grouping, friend_ssid, ship_status, friend_type, create_time) "
                      "VALUES (?, ?, ?, ?, ?, ?);";
    std::vector<std::string> params = {
        friendship.ssid.toStdString(),
        friendship.grouping.toStdString(),
        friendship.friendSSID.toStdString(),
        std::to_string(friendship.shipStatus),
        std::to_string(friendship.friendType),
        std::to_string(friendship.createTime)
    };

    if (_db.update(sql, params)) {
        // 获取最后插入的ID
        std::string lastIdSql = "SELECT last_insert_rowid();";
        auto result = _db.query(lastIdSql, {});
        if (!result.empty() && !result[0].empty()) {
            return std::stoll(result[0][0]);
        }
    }
    return -1;
}

bool FriendshipDAO::updateStatus(qint64 id, qint32 newStatus) {
    std::string sql = "UPDATE friendship SET ship_status = ? WHERE id = ?;";
    std::vector<std::string> params = {
        std::to_string(newStatus),
        std::to_string(id)
    };

    return _db.update(sql, params);
}

FriendshipDO FriendshipDAO::findRelationship(const QString &ssid, const QString &friendSsid) {
    std::string sql = "SELECT id, ssid, grouping, friend_ssid, ship_status, friend_type, create_time "
                     "FROM friendship WHERE ssid = ? AND friend_ssid = ?;";
    std::vector<std::string> params = {
        ssid.toStdString(),
        friendSsid.toStdString()
    };

    auto result = _db.query(sql, params);
    if (!result.empty()) {
        FriendshipDO friendship;
        friendship.id = std::stoll(result[0][0]);
        friendship.ssid = QString::fromStdString(result[0][1]);
        friendship.grouping = QString::fromStdString(result[0][2]);
        friendship.friendSSID = QString::fromStdString(result[0][3]);
        friendship.shipStatus = std::stoi(result[0][4]);
        friendship.friendType = std::stoi(result[0][5]);
        friendship.createTime = std::stoll(result[0][6]);
        return friendship;
    }
    return {-1};
}

QList<FriendshipDO> FriendshipDAO::listByUser(const QString &ssid) {
    std::string sql = "SELECT id, ssid, grouping, friend_ssid, ship_status, friend_type, create_time "
                      "FROM friendship WHERE ssid = ?;";
    std::vector<std::string> params = { ssid.toStdString()};

    auto result = _db.query(sql, params);
    QList<FriendshipDO> friendships;
    for (int i = 0;i < result.size(); ++i) {
        FriendshipDO friendship;
        friendship.id = std::stoll(result[i][0]);
        friendship.ssid = QString::fromStdString(result[i][1]);
        friendship.grouping = QString::fromStdString(result[i][2]);
        friendship.friendSSID = QString::fromStdString(result[i][3]);
        friendship.shipStatus = std::stoi(result[i][4]);
        friendship.friendType = std::stoi(result[i][5]);
        friendship.createTime = std::stoll(result[0][6]);
        friendships.append(friendship);
    }
    return friendships;
}

int FriendshipDAO::getFriendshipCount(const QString &ssid) {
    std::string sql = "SELECT COUNT(*) FROM friendship WHERE ssid = ?";
    std::vector<std::string> params = { ssid.toStdString() };

    auto result = _db.query(sql, params);

    if (!result.empty() && !result[0].empty()) {
        return std::stoi(result[0][0]);
    }else {
        return 0;
    }
}