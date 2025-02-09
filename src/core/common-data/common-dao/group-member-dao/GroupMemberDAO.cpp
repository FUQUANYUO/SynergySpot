//
// Created by FUQAQ on 2025/1/26.
//

#include "GroupMemberDAO.h"

GroupMemberDAO::GroupMemberDAO(LiteConn &db) : _db(db) {}

GroupMemberDAO::~GroupMemberDAO() {}

bool GroupMemberDAO::addMember(const GroupMemberInfoDO &member) {
    std::string sql = "INSERT INTO group_member_info (ssid_group, ssid_member, create_time) "
                  "VALUES (?, ?, ?);";
    std::vector<std::string> params = {
        member.ssidGroup.toStdString(),
        member.ssidMember.toStdString(),
        member.createTime.toString("yyyy-MM-dd HH:mm:ss").toStdString()
    };

    return _db.update(sql, params);
}

bool GroupMemberDAO::removeMember(const QString &groupSsid, const QString &memberSsid) {
    std::string sql = "DELETE FROM group_member_info WHERE ssid_group = ? AND ssid_member = ?;";
    std::vector<std::string> params = {
        groupSsid.toStdString(),
        memberSsid.toStdString()
    };

    return _db.update(sql, params);
}

QList<GroupMemberInfoDO> GroupMemberDAO::listMembers(const QString &groupSsid, int pageSize, int pageNum) {
    std::string sql = "SELECT id, ssid_group, ssid_member, create_time "
                  "FROM group_member_info WHERE ssid_group = ? LIMIT ? OFFSET ?;";
    std::vector<std::string> params = { groupSsid.toStdString(), std::to_string(pageSize), std::to_string(pageNum)};

    auto result = _db.query(sql, params);
    QList<GroupMemberInfoDO> members;
    for (const auto &row : result) {
        GroupMemberInfoDO member;
        member.id = std::stoll(row[0]);
        member.ssidGroup = QString::fromStdString(row[1]);
        member.ssidMember = QString::fromStdString(row[2]);
        member.createTime = QDateTime::fromString(QString::fromStdString(row[3]), "yyyy-MM-dd HH:mm:ss");
        members.append(member);
    }
    return members;
}

int GroupMemberDAO::getMemberCount(const QString &ssid) {
    std::string sql = "SELECT COUNT(*) FROM group_member_info WHERE ssid_group = ?";
    std::vector<std::string> params = { ssid.toStdString() };

    auto result = _db.query(sql, params);

    if (!result.empty() && !result[0].empty()) {
        return std::stoi(result[0][0]);
    }else {
        return 0;
    }
}