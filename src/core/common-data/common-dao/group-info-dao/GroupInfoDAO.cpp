//
// Created by FUQAQ on 2025/1/26.
//

#include "GroupInfoDAO.h"

GroupInfoDAO::GroupInfoDAO(LiteConn &db) : _db(db){}

GroupInfoDAO::~GroupInfoDAO() {}

qint64 GroupInfoDAO::createGroup(const GroupBaseInfoDO &group) {
    std::string sql = "INSERT INTO group_base_info (ssid_group, name, avatar, create_ssid, profile, admins ,create_time) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?);";
    std::vector<std::string> params = {
        group.ssidGroup.toStdString(),
        group.name.toStdString(),
        group.avatar.toStdString(),
        group.createSSID.toStdString(),
        group.profile.toStdString(),
        group.admins.join("/").toStdString(),
        std::to_string(group.createTime)
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

bool GroupInfoDAO::updateProfile(const QString &groupSsid, const QString &newProfile) {
    std::string sql = "UPDATE group_base_info SET profile = ? WHERE ssid_group = ?;";
    std::vector<std::string> params = {
        newProfile.toStdString(),
        groupSsid.toStdString()
    };

    return _db.update(sql, params);
}

bool GroupInfoDAO::updateAvatar(const QString &groupSsid, const QString &path) {
    std::string sql = "UPDATE group_base_info SET avatar = ? WHERE ssid_group = ?;";
    std::vector<std::string> params = {
        path.toStdString(),
        groupSsid.toStdString()
    };

    return _db.update(sql, params);
}

GroupBaseInfoDO GroupInfoDAO::findBySsid(const QString &groupSsid) {
    std::string sql = "SELECT id, ssid_group, name, avatar, create_ssid, profile, admins, create_time "
                      "FROM group_base_info WHERE ssid_group = ?;";
    std::vector<std::string> params = { groupSsid.toStdString() };

    auto result = _db.query(sql, params);
    if (!result.empty()) {
        GroupBaseInfoDO group;
        group.id = std::stoll(result[0][0]);
        group.ssidGroup = QString::fromStdString(result[0][1]);
        group.name = QString::fromStdString(result[0][2]);
        group.avatar = QString::fromStdString(result[0][3]);
        group.createSSID = QString::fromStdString(result[0][4]);
        group.profile = QString::fromStdString(result[0][5]);
        group.admins  = QString::fromStdString(result[0][6]).split("/");
        group.createTime = std::stoll(result[0][7]);
        return group;
    }
    return {-1};
}

QList<GroupBaseInfoDO> GroupInfoDAO::getAllGroupInfos(int pageSize, int pageNum) {
    std::string sql = "SELECT id, ssid_group, name, avatar, create_ssid, profile, admins, create_time "
                      "FROM group_base_info LIMIT ? OFFSET ?;";
    std::vector<std::string> params = { std::to_string(pageSize), std::to_string((pageNum - 1) * pageSize)};

    auto result = _db.query(sql, params);
    QList<GroupBaseInfoDO> infos;
    for (const auto &row : result) {
        GroupBaseInfoDO group;
        group.id = std::stoll(result[0][0]);
        group.ssidGroup = QString::fromStdString(result[0][1]);
        group.name = QString::fromStdString(result[0][2]);
        group.avatar = QString::fromStdString(result[0][3]);
        group.createSSID = QString::fromStdString(result[0][4]);
        group.profile = QString::fromStdString(result[0][5]);
        group.admins = QString::fromStdString(result[0][6]).split("/");
        group.createTime = std::stoll(result[0][7]);
        infos.append(group);
    }
    return infos;
}

int GroupInfoDAO::getGroupInfoDataCount() {
    std::string sql = "SELECT COUNT(*) FROM group_base_info";
    std::vector<std::string> params = { };

    auto result = _db.query(sql, params);

    if (!result.empty() && !result[0].empty()) {
        return std::stoi(result[0][0]);
    }else {
        return 0;
    }
}