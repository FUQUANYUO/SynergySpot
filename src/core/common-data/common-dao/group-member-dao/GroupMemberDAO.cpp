//
// Created by FUQAQ on 2025/1/26.
//

#include "GroupMemberDAO.h"

GroupMemberDAO::GroupMemberDAO(LiteConn &db) : _db(db) {}

GroupMemberDAO::~GroupMemberDAO() {}

bool GroupMemberDAO::addMember(const GroupMemberInfoDO &member) {
    std::string sql = "INSERT INTO group_member_info (ssidGroup, ssidMember, createTime) "
                  "VALUES (?, ?, ?);";
    std::vector<std::string> params = {
        member.ssidGroup.toStdString(),
        member.ssidMember.toStdString(),
        member.createTime.toString("yyyy-MM-dd HH:mm:ss").toStdString()
    };

    return _db.update(sql, params);
}

bool GroupMemberDAO::removeMember(const QString &groupSsid, const QString &memberSsid) {
    std::string sql = "DELETE FROM group_member_info WHERE ssidGroup = ? AND ssidMember = ?;";
    std::vector<std::string> params = {
        groupSsid.toStdString(),
        memberSsid.toStdString()
    };

    return _db.update(sql, params);
}

QList<GroupMemberInfoDO> GroupMemberDAO::listMembers(const QString &groupSsid) {
    std::string sql = "SELECT id, ssidGroup, ssidMember, createTime "
                  "FROM group_member_info WHERE ssidGroup = ?;";
    std::vector<std::string> params = { groupSsid.toStdString() };

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