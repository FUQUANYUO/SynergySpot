//
// Created by FUQAQ on 2025/1/26.
//

#include "GroupInfoDAO.h"

GroupInfoDAO::GroupInfoDAO(LiteConn &db) : _db(db){}

GroupInfoDAO::~GroupInfoDAO() {}

qint64 GroupInfoDAO::createGroup(const GroupBaseInfoDO &group) {
    std::string sql = "INSERT INTO group_base_info (ssidGroup, name, createSSID, profile, createTime) "
                  "VALUES (?, ?, ?, ?, ?);";
    std::vector<std::string> params = {
        group.ssidGroup.toStdString(),
        group.name.toStdString(),
        group.createSSID.toStdString(),
        group.profile.toStdString(),
        group.createTime.toString("yyyy-MM-dd HH:mm:ss").toStdString()
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
    std::string sql = "UPDATE group_base_info SET profile = ? WHERE ssidGroup = ?;";
    std::vector<std::string> params = {
        newProfile.toStdString(),
        groupSsid.toStdString()
    };

    return _db.update(sql, params);
}

GroupBaseInfoDO GroupInfoDAO::findBySsid(const QString &groupSsid) {
    std::string sql = "SELECT id, ssidGroup, name, createSSID, profile, createTime "
                      "FROM group_base_info WHERE ssidGroup = ?;";
    std::vector<std::string> params = { groupSsid.toStdString() };

    auto result = _db.query(sql, params);
    if (!result.empty()) {
        GroupBaseInfoDO group;
        group.id = std::stoll(result[0][0]);
        group.ssidGroup = QString::fromStdString(result[0][1]);
        group.name = QString::fromStdString(result[0][2]);
        group.createSSID = QString::fromStdString(result[0][3]);
        group.profile = QString::fromStdString(result[0][4]);
        group.createTime = QDateTime::fromString(QString::fromStdString(result[0][5]), "yyyy-MM-dd HH:mm:ss");
        return group;
    }
    return GroupBaseInfoDO();
}