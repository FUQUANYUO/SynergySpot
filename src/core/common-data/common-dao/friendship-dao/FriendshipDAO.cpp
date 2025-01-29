//
// Created by FUQAQ on 2025/1/26.
//

#include "FriendshipDAO.h"


FriendshipDAO::FriendshipDAO(LiteConn &db) : _db(db) {}

FriendshipDAO::~FriendshipDAO() {}

qint64 FriendshipDAO::create(const FriendshipDO &friendship) {
    std::string sql = "INSERT INTO friendship (ssid, friendSSID, shipStatus, friendType, createTime) "
                      "VALUES (?, ?, ?, ?, ?);";
    std::vector<std::string> params = {
        friendship.ssid.toStdString(),
        friendship.friendSSID.toStdString(),
        std::to_string(friendship.shipStatus),
        std::to_string(friendship.friendType),
        friendship.createTime.toString("yyyy-MM-dd HH:mm:ss").toStdString()
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
    std::string sql = "UPDATE friendship SET shipStatus = ? WHERE id = ?;";
    std::vector<std::string> params = {
        std::to_string(newStatus),
        std::to_string(id)
    };

    return _db.update(sql, params);
}

FriendshipDO FriendshipDAO::findRelationship(const QString &ssid, const QString &friendSsid) {
    std::string sql = "SELECT id, ssid, friendSSID, shipStatus, friendType, createTime "
                     "FROM friendship WHERE ssid = ? AND friendSSID = ?;";
    std::vector<std::string> params = {
        ssid.toStdString(),
        friendSsid.toStdString()
    };

    auto result = _db.query(sql, params);
    if (!result.empty()) {
        FriendshipDO friendship;
        friendship.id = std::stoll(result[0][0]);
        friendship.ssid = QString::fromStdString(result[0][1]);
        friendship.friendSSID = QString::fromStdString(result[0][2]);
        friendship.shipStatus = std::stoi(result[0][3]);
        friendship.friendType = std::stoi(result[0][4]);
        friendship.createTime = QDateTime::fromString(QString::fromStdString(result[0][5]), "yyyy-MM-dd HH:mm:ss");
        return friendship;
    }
    return FriendshipDO();
}

QList<FriendshipDO> FriendshipDAO::listByUser(const QString &ssid) {
    std::string sql = "SELECT id, ssid, friendSSID, shipStatus, friendType, createTime "
                      "FROM friendship WHERE ssid = ?;";
    std::vector<std::string> params = { ssid.toStdString() };

    auto result = _db.query(sql, params);
    QList<FriendshipDO> friendships;
    for (const auto &row : result) {
        FriendshipDO friendship;
        friendship.id = std::stoll(row[0]);
        friendship.ssid = QString::fromStdString(row[1]);
        friendship.friendSSID = QString::fromStdString(row[2]);
        friendship.shipStatus = std::stoi(row[3]);
        friendship.friendType = std::stoi(row[4]);
        friendship.createTime = QDateTime::fromString(QString::fromStdString(row[5]), "yyyy-MM-dd HH:mm:ss");
        friendships.append(friendship);
    }
    return friendships;
}