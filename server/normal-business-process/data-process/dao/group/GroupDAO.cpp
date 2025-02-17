//
// Created by FUQAQ on 2025/2/6.
//

#include "GroupDAO.h"

int64_t GroupBaseInfoDAO::createGroup(const GroupBaseInfoDO &group)  {
    std::string sql = "INSERT INTO group_base_info (ssid_group, name, avatar, create_ssid, profile) "
                      "VALUES (?, ?, ?, ?, ?)";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramSsidGroup;
    paramSsidGroup.type = MysqlConn::Param::STRING;
    paramSsidGroup.str_val = group.ssidGroup;
    params.push_back(paramSsidGroup);

    MysqlConn::Param paramName;
    paramName.type = MysqlConn::Param::STRING;
    paramName.str_val = group.name;
    params.push_back(paramName);

    MysqlConn::Param paramAvatar;
    paramAvatar.type = MysqlConn::Param::STRING;
    paramAvatar.str_val = group.avatar;
    params.push_back(paramAvatar);

    MysqlConn::Param paramCreateSsid;
    paramCreateSsid.type = MysqlConn::Param::STRING;
    paramCreateSsid.str_val = group.createSsid;
    params.push_back(paramCreateSsid);

    MysqlConn::Param paramProfile;
    paramProfile.type = MysqlConn::Param::STRING;
    paramProfile.str_val = group.profile;
    params.push_back(paramProfile);

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to create group: " << group.ssidGroup);
        return -1;
    }

    return m_conn->getLastInsertId();
}

bool GroupBaseInfoDAO::update(const GroupBaseInfoDO &group) {
    std::string sql = "UPDATE group_base_info SET ";
    std::vector<std::string> setClauses;
    std::vector<MysqlConn::Param> params;

    // 检查并添加 name
    if (!group.name.empty() && group.name != "-1") {
        setClauses.push_back("name = ?");
        MysqlConn::Param paramName;
        paramName.type = MysqlConn::Param::STRING;
        paramName.str_val = group.name;
        params.push_back(paramName);
    }

    // 检查并添加 avatar
    if (!group.avatar.empty() && group.avatar != "-1") {
        setClauses.push_back("avatar = ?");
        MysqlConn::Param paramAvatar;
        paramAvatar.type = MysqlConn::Param::STRING;
        paramAvatar.str_val = group.avatar;
        params.push_back(paramAvatar);
    }

    // 检查并添加 createSsid
    if (!group.createSsid.empty() && group.createSsid != "-1") {
        setClauses.push_back("create_ssid = ?");
        MysqlConn::Param paramCreateSsid;
        paramCreateSsid.type = MysqlConn::Param::STRING;
        paramCreateSsid.str_val = group.createSsid;
        params.push_back(paramCreateSsid);
    }

    // 检查并添加 profile
    if (!group.profile.empty() && group.profile != "-1") {
        setClauses.push_back("profile = ?");
        MysqlConn::Param paramProfile;
        paramProfile.type = MysqlConn::Param::STRING;
        paramProfile.str_val = group.profile;
        params.push_back(paramProfile);
    }

    // 如果没有有效的字段需要更新，直接返回
    if (setClauses.empty()) {
        LOG_WARNING("No valid fields to update for group: " << group.ssidGroup);
        return false;
    }

    // 拼接完整的SQL语句
    for (auto it = setClauses.begin(); it != setClauses.end(); ++it) {
        if (it + 1 != setClauses.end()) {
            sql += (*it) + ", ";
        }
    }
    sql += " WHERE ssid_group = ?";

    // 添加 ssidGroup 参数
    MysqlConn::Param paramSsidGroup;
    paramSsidGroup.type = MysqlConn::Param::STRING;
    paramSsidGroup.str_val = group.ssidGroup;
    params.push_back(paramSsidGroup);

    // 执行更新操作
    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to update base info for group: " << group.ssidGroup);
        return false;
    }

    return true;
}

bool GroupBaseInfoDAO::deleteById(const std::string &groupSsid)  {
    std::string sql = "DELETE FROM group_base_info WHERE ssid_group = ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramSsidGroup;
    paramSsidGroup.type = MysqlConn::Param::STRING;
    paramSsidGroup.str_val = groupSsid;
    params.push_back(paramSsidGroup);

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to delete group: " << groupSsid);
        return false;
    }

    return true;
}

GroupBaseInfoDO GroupBaseInfoDAO::findBySsid(const std::string &groupSsid) {
    std::string sql = "SELECT id, ssid_group, name, avatar, create_ssid, profile, create_time "
                      "FROM group_base_info WHERE ssid_group = ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramSsidGroup;
    paramSsidGroup.type = MysqlConn::Param::STRING;
    paramSsidGroup.str_val = groupSsid;
    params.push_back(paramSsidGroup);

    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG_ERROR("Failed to find group by ssid: " << groupSsid);
        return {};
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        mysql_free_result(result);
        return {};
    }

    GroupBaseInfoDO group;
    group.id = std::stoll(row[0]);
    group.ssidGroup = row[1];
    group.name = row[2];
    group.avatar = row[3];
    group.createSsid = row[4];
    group.profile = row[5];
    group.createTime = row[6] ? std::stoul(row[6]) : 0;

    mysql_free_result(result);
    return group;
}

std::string GroupBaseInfoDAO::getLastGroupSSID() {
    std::string sql = "SELECT ssid_group FROM group_base_info ORDER BY create_time DESC LIMIT 1";
    MYSQL_RES* result = m_conn->query(sql, {});
    if (!result) {
        LOG_ERROR("Failed find last group ssid ");
        return "-1";
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        mysql_free_result(result);
        return "-1";
    }
    return row[0];
}

bool GroupAdminDAO::addAdmin(int64_t groupId, const std::string &opSsid)  {
    std::string sql = "INSERT INTO group_admin (group_id, op_ssid) VALUES (?, ?)";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramGroupId;
    paramGroupId.type = MysqlConn::Param::INT;
    paramGroupId.int_val = groupId;
    params.push_back(paramGroupId);

    MysqlConn::Param paramOpSsid;
    paramOpSsid.type = MysqlConn::Param::STRING;
    paramOpSsid.str_val = opSsid;
    params.push_back(paramOpSsid);

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to add admin to group ID: " << groupId << " , Ssid: " << opSsid);
        return false;
    }

    return true;
}

bool GroupAdminDAO::removeAdmin(int64_t groupId, const std::string &opSsid) {
    std::string sql = "DELETE FROM group_admin WHERE group_id = ? AND op_ssid = ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramGroupId;
    paramGroupId.type = MysqlConn::Param::INT;
    paramGroupId.int_val = groupId;
    params.push_back(paramGroupId);

    MysqlConn::Param paramOpSsid;
    paramOpSsid.type = MysqlConn::Param::STRING;
    paramOpSsid.str_val = opSsid;
    params.push_back(paramOpSsid);

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to remove admin from group ID: " << groupId << " , Ssid: " << opSsid);
        return false;
    }

    return true;
}

std::vector<GroupAdminDO> GroupAdminDAO::getAdmins(int64_t groupId) {
    std::string sql = "SELECT id, group_id, op_ssid FROM group_admin WHERE group_id = ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramGroupId;
    paramGroupId.type = MysqlConn::Param::INT;
    paramGroupId.int_val = groupId;
    params.push_back(paramGroupId);

    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG_ERROR("Failed to get admins for group ID: " << groupId);
        return {};
    }

    std::vector<GroupAdminDO> admins;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        GroupAdminDO admin;
        admin.id = std::stoll(row[0]);
        admin.groupId = std::stoll(row[1]);
        admin.opSsid = row[2];
        admins.push_back(admin);
    }

    mysql_free_result(result);
    return admins;
}

bool GroupNoticeDAO::deleteNotice(int64_t groupId, int64_t noticeId)  {
    std::string sql = "DELETE FROM group_notice WHERE group_id = ? AND id = ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramGroupId;
    paramGroupId.type = MysqlConn::Param::INT;
    paramGroupId.int_val = groupId;
    params.push_back(paramGroupId);

    MysqlConn::Param paramNoticeId;
    paramNoticeId.type = MysqlConn::Param::INT;
    paramNoticeId.int_val = noticeId;
    params.push_back(paramNoticeId);

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to delete notice with ID: " << noticeId <<
                  " for group ID: " << groupId);
        return false;
    }

    return true;
}

int64_t GroupNoticeDAO::addNotice(int64_t groupId, const std::string &notice)  {
    std::string sql = "INSERT INTO group_notice (group_id, notice) VALUES (?, ?)";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramGroupId;
    paramGroupId.type = MysqlConn::Param::INT;
    paramGroupId.int_val = groupId;
    params.push_back(paramGroupId);

    MysqlConn::Param paramNotice;
    paramNotice.type = MysqlConn::Param::STRING;
    paramNotice.str_val = notice;
    params.push_back(paramNotice);

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to add notice for group ID: " << groupId);
        return -1;
    }

    return m_conn->getLastInsertId();
}

std::vector<GroupNoticeDO> GroupNoticeDAO::listNotices(int64_t groupId, int pageSize, int pageNum)  {
    std::string sql = "SELECT id, group_id, notice, create_time "
                      "FROM group_notice WHERE group_id = ? "
                      "ORDER BY create_time DESC LIMIT ? OFFSET ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramGroupId;
    paramGroupId.type = MysqlConn::Param::INT;
    paramGroupId.int_val = groupId;
    params.push_back(paramGroupId);

    MysqlConn::Param paramPageSize;
    paramPageSize.type = MysqlConn::Param::INT;
    paramPageSize.int_val = pageSize;
    params.push_back(paramPageSize);

    MysqlConn::Param paramOffset;
    paramOffset.type = MysqlConn::Param::INT;
    paramOffset.int_val = (pageNum - 1) * pageSize;
    params.push_back(paramOffset);

    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG_ERROR("Failed to list notices for group ID: " << groupId);
        return {};
    }

    std::vector<GroupNoticeDO> notices;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        GroupNoticeDO notice;
        notice.id = std::stoll(row[0]);
        notice.groupId = std::stoll(row[1]);
        notice.notice = row[2];
        notice.createTime = row[3] ? std::stoul(row[3]) : 0;
        notices.push_back(notice);
    }

    mysql_free_result(result);
    return notices;
}

bool GroupMemberDAO::addMember(const std::string &groupSsid, const std::string &memberSsid)  {
    std::string sql = "INSERT INTO group_member_info (ssid_group, ssid_member) VALUES (?, ?)";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramGroupSsid;
    paramGroupSsid.type = MysqlConn::Param::STRING;
    paramGroupSsid.str_val = groupSsid;
    params.push_back(paramGroupSsid);

    MysqlConn::Param paramMemberSsid;
    paramMemberSsid.type = MysqlConn::Param::STRING;
    paramMemberSsid.str_val = memberSsid;
    params.push_back(paramMemberSsid);

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to add member " << memberSsid <<
                  " to group " << groupSsid);
        return false;
    }

    return true;
}

bool GroupMemberDAO::removeMember(const std::string &groupSsid, const std::string &memberSsid)  {
    std::string sql = "DELETE FROM group_member_info WHERE ssid_group = ? AND ssid_member = ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramGroupSsid;
    paramGroupSsid.type = MysqlConn::Param::STRING;
    paramGroupSsid.str_val = groupSsid;
    params.push_back(paramGroupSsid);

    MysqlConn::Param paramMemberSsid;
    paramMemberSsid.type = MysqlConn::Param::STRING;
    paramMemberSsid.str_val = memberSsid;
    params.push_back(paramMemberSsid);

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to remove member " << memberSsid <<
                  " from group " << groupSsid);
        return false;
    }

    return true;
}

std::vector<GroupMemberInfoDO> GroupMemberDAO::listMembers(const std::string &groupSsid)  {
    std::string sql = "SELECT id, ssid_group, ssid_member, create_time "
                      "FROM group_member_info WHERE ssid_group = ? "
                      "ORDER BY create_time DESC";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramGroupSsid;
    paramGroupSsid.type = MysqlConn::Param::STRING;
    paramGroupSsid.str_val = groupSsid;
    params.push_back(paramGroupSsid);

    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG_ERROR("Failed to list members for group " << groupSsid);
        return {};
    }

    std::vector<GroupMemberInfoDO> members;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        GroupMemberInfoDO member;
        member.id = std::stoll(row[0]);
        member.ssidGroup = row[1];
        member.ssidMember = row[2];
        member.createTime = row[3] ? std::stoul(row[3]) : 0;
        members.push_back(member);
    }

    mysql_free_result(result);
    return members;
}

int GroupMemberDAO::getMemberCount(const std::string &groupSsid)  {
    std::string sql = "SELECT COUNT(*) FROM group_member_info WHERE ssid_group = ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramGroupSsid;
    paramGroupSsid.type = MysqlConn::Param::STRING;
    paramGroupSsid.str_val = groupSsid;
    params.push_back(paramGroupSsid);

    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG_ERROR("Failed to get member count for group " << groupSsid);
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