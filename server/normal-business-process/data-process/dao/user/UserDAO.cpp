//
// Created by FUQAQ on 2025/2/6.
//

#include "UserDAO.h"

bool UserDAO::insert(const UserBaseInfoDO &user) {
    std::string sql = "INSERT INTO user_base_info (ssid, ssname, avatar, sex, personal_sign, birthday, region) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?)";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramSsid;
    paramSsid.type = MysqlConn::Param::STRING;
    paramSsid.str_val = user.ssid;
    params.push_back(paramSsid);

    MysqlConn::Param paramSsname;
    paramSsname.type = MysqlConn::Param::STRING;
    paramSsname.str_val = user.ssname;
    params.push_back(paramSsname);

    MysqlConn::Param paramAvatar;
    paramAvatar.type = MysqlConn::Param::STRING;
    paramAvatar.str_val = user.avatar;
    params.push_back(paramAvatar);

    MysqlConn::Param paramSex;
    paramSex.type = MysqlConn::Param::STRING;
    paramSex.str_val = std::string(1, user.sex);
    params.push_back(paramSex);

    MysqlConn::Param paramPersonalSign;
    paramPersonalSign.type = MysqlConn::Param::STRING;
    paramPersonalSign.str_val = user.personalSign;
    params.push_back(paramPersonalSign);

    MysqlConn::Param paramBirthday;
    paramBirthday.type = MysqlConn::Param::STRING;
    paramBirthday.str_val = user.birthday ? std::to_string(user.birthday) : "";
    params.push_back(paramBirthday);

    MysqlConn::Param paramRegion;
    paramRegion.type = MysqlConn::Param::INT;
    paramRegion.int_val = user.region;
    params.push_back(paramRegion);

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to insert user: " << user.ssid);
        return false;
    }

    return true;
}

bool UserDAO::update(const UserBaseInfoDO &user) {
    std::string sql = "UPDATE user_base_info SET ";
    std::vector<std::string> setClauses;
    std::vector<MysqlConn::Param> params;

    // 检查并添加 ssname
    if (!user.ssname.empty() && user.ssname != "-1") {
        setClauses.push_back("ssname = ?");
        params.push_back({MysqlConn::Param::STRING, 0, user.ssname});
    }

    // 检查并添加 avatar
    if (!user.avatar.empty() && user.avatar != "-1") {
        setClauses.push_back("avatar = ?");
        params.push_back({MysqlConn::Param::STRING, 0, user.avatar});
    }

    // 检查并添加 sex
    if (user.sex != '\0') {
        setClauses.push_back("sex = ?");
        params.push_back({MysqlConn::Param::STRING, 0, std::string(1, user.sex)});
    }

    // 检查并添加 personalSign
    if (!user.personalSign.empty() && user.personalSign != "-1") {
        setClauses.push_back("personal_sign = ?");
        params.push_back({MysqlConn::Param::STRING, 0, user.personalSign});
    }

    // 检查并添加 birthday
    if (user.birthday != 0) {
        setClauses.push_back("birthday = ?");
        params.push_back({MysqlConn::Param::STRING, 0, std::to_string(user.birthday)});
    }

    // 检查并添加 region
    if (user.region != 0) {
        setClauses.push_back("region = ?");
        params.push_back({MysqlConn::Param::INT, user.region, ""});
    }

    // 如果没有有效的字段需要更新，直接返回
    if (setClauses.empty()) {
        LOG_WARNING("No valid fields to update for user: " << user.ssid);
        return false;
    }

    for (size_t i = 0; i < setClauses.size(); ++i) {
        sql += setClauses[i];
        if (i != setClauses.size() - 1) {
            sql += ", ";
        }
    }

    sql += " WHERE ssid = ?";

    // 添加 ssid 参数
    params.push_back({MysqlConn::Param::STRING, 0, user.ssid});

    // 执行更新操作
    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to update user: " << user.ssid);
        return false;
    }

    return true;
}

bool UserDAO::deleteById(const std::string &ssid) {
    std::string sql = "DELETE FROM user_base_info WHERE ssid = ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramSsid;
    paramSsid.type = MysqlConn::Param::STRING;
    paramSsid.str_val = ssid;
    params.push_back(paramSsid);

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to delete user with SSID: " << ssid);
        return false;
    }

    return true;
}

UserBaseInfoDO UserDAO::findById(const std::string &ssid) {
    std::string sql = "SELECT ssid, ssname, avatar, sex, personal_sign, UNIX_TIMESTAMP(birthday), region, UNIX_TIMESTAMP(create_time) "
                      "FROM user_base_info WHERE ssid = ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramSsid;
    paramSsid.type = MysqlConn::Param::STRING;
    paramSsid.str_val = ssid;
    params.push_back(paramSsid);

    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG_ERROR("Failed to find user by SSID: " << ssid);
        return {};
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        mysql_free_result(result);
        return {};
    }

    UserBaseInfoDO user;
    user.ssid = row[0];
    user.ssname = row[1];
    user.avatar = (row[2] != nullptr) ? row[2] : "";
    user.sex = row[3][0];
    user.personalSign = row[4];
    user.birthday = row[5] ? std::stoll(row[5]) : 0;
    user.region = static_cast<uint8_t>(std::stoul(row[6]));
    user.createTime = row[7] ? std::stoll(row[7]) : 0;

    mysql_free_result(result);
    return user;
}

std::vector<UserBaseInfoDO> UserDAO::fuzzyMatchingByIdOrName(const std::string &ssid, const std::string &name) {
    std::vector<UserBaseInfoDO> users;
    std::vector<std::string> conditions;
    std::vector<MysqlConn::Param> params;

    // 动态构建SQL条件
    if (!ssid.empty()) {
        conditions.emplace_back("ssid LIKE ?");
        MysqlConn::Param param;
        param.type = MysqlConn::Param::STRING;
        param.str_val = "%" + ssid + "%";
        params.push_back(param);
    }
    if (!name.empty()) {
        conditions.emplace_back("ssname LIKE ?");
        MysqlConn::Param param;
        param.type = MysqlConn::Param::STRING;
        param.str_val = "%" + name + "%";
        params.push_back(param);
    }

    // 无有效条件时直接返回空结果
    if (conditions.empty()) {
        LOG_ERROR("Both ssid and name are empty");
        return users;
    }

    // 拼接SQL语句
    std::string sql = "SELECT ssid, ssname, avatar, sex, personal_sign, UNIX_TIMESTAMP(birthday), region, UNIX_TIMESTAMP(create_time) "
                      "FROM user_base_info WHERE ";
    for (size_t i = 0; i < conditions.size(); ++i) {
        if (i > 0) sql += " OR ";
        sql += conditions[i];
    }

    // 执行查询
    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG_ERROR("Fuzzy search failed. SQL: " << sql);
        return users;
    }

    // 遍历结果集
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        UserBaseInfoDO user;
        user.ssid = row[0] ? row[0] : "";
        user.ssname = row[1] ? row[1] : "";
        user.avatar = row[2] ? row[2] : "";
        user.sex = row[3] ? row[3][0] : '\0';
        user.personalSign = row[4] ? row[4] : "";
        user.birthday = row[5] ? std::stoll(row[5]) : 0;
        user.region = row[6] ? static_cast<uint8_t>(std::stoul(row[6])) : 0;
        user.createTime = row[7] ? std::stoll(row[7]) : 0;
        users.push_back(user);
    }

    mysql_free_result(result);
    return users;
}

bool UserDAO::updateThumbUpCount(const std::string &ssid, int newCount) {
    std::string sql = "UPDATE user_base_info SET thumb_up_count = ? WHERE ssid = ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramCount;
    paramCount.type = MysqlConn::Param::INT;
    paramCount.int_val = newCount;
    params.push_back(paramCount);

    MysqlConn::Param paramSsid;
    paramSsid.type = MysqlConn::Param::STRING;
    paramSsid.str_val = ssid;
    params.push_back(paramSsid);

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to update thumb up count for user: " << ssid);
        return false;
    }

    return true;
}

std::vector<UserBaseInfoDO> UserDAO::findByRegion(uint8_t region, int pageSize, int pageNum) {
    std::string sql = "SELECT ssid, ssname, avatar, sex, personal_sign, UNIX_TIMESTAMP(birthday), region, UNIX_TIMESTAMP(create_time) "
                      "FROM user_base_info WHERE region = ? "
                      "LIMIT ? OFFSET ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramRegion;
    paramRegion.type = MysqlConn::Param::INT;
    paramRegion.int_val = region;
    params.push_back(paramRegion);

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
        LOG_ERROR("Failed to find users by region: " << static_cast<int>(region));
        return {};
    }

    std::vector<UserBaseInfoDO> users;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        UserBaseInfoDO user;
        user.ssid = row[0];
        user.ssname = row[1];
        user.avatar = row[2];
        user.sex = row[3][0];
        user.personalSign = row[4];
        user.birthday = row[5] ? std::stoul(row[5]) : 0;
        user.region = static_cast<uint8_t>(std::stoul(row[6]));
        user.createTime = row[7] ? std::stoul(row[7]) : 0;
        users.push_back(user);
    }

    mysql_free_result(result);
    return users;
}

int UserDAO::getRegionCount(uint8_t region) {
    std::string sql = "SELECT COUNT(*) FROM user_base_info WHERE region = ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramRegion;
    paramRegion.type = MysqlConn::Param::INT;
    paramRegion.int_val = region;
    params.push_back(paramRegion);

    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG_ERROR("Failed to get user count for region: " << static_cast<int>(region));
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

std::string UserDAO::getLastUserSSID() {
    std::string sql = "SELECT ssid FROM user_base_info ORDER BY create_time DESC LIMIT 1";
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

bool UserPrivateDAO::insert(const UserPrivateInfoDO &user) {
    std::string sql = "INSERT INTO user_private_info (ssid, email, password, password_salt) "
                      "VALUES (?, ?, ?, ?)";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramSsid;
    paramSsid.type = MysqlConn::Param::STRING;
    paramSsid.str_val = user.ssid;
    params.push_back(paramSsid);

    MysqlConn::Param paramEmail;
    paramEmail.type = MysqlConn::Param::STRING;
    paramEmail.str_val = user.email;
    params.push_back(paramEmail);

    MysqlConn::Param paramPassword;
    paramPassword.type = MysqlConn::Param::STRING;
    paramPassword.str_val = user.password;
    params.push_back(paramPassword);

    MysqlConn::Param paramSalt;
    paramSalt.type = MysqlConn::Param::STRING;
    paramSalt.str_val = user.passwordSalt;
    params.push_back(paramSalt);

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to insert user private info: " << user.ssid);
        return false;
    }

    return true;
}

bool UserPrivateDAO::update(const UserPrivateInfoDO &user) {
    std::string sql = "UPDATE user_private_info SET";
    std::vector<MysqlConn::Param> params;
    std::vector<std::string> dynamicSQL;

    if (user.email != "-1" && !user.email.empty()) {
        dynamicSQL.emplace_back(" email = ?");
        MysqlConn::Param paramEmail;
        paramEmail.type = MysqlConn::Param::STRING;
        paramEmail.str_val = user.email;
        params.push_back(paramEmail);
    }

    if (user.password != "-1" && !user.password.empty()) {
        dynamicSQL.emplace_back(" password = ?");
        MysqlConn::Param paramPassword;
        paramPassword.type = MysqlConn::Param::STRING;
        paramPassword.str_val = user.password;
        params.push_back(paramPassword);
    }

    if (user.passwordSalt != "-1" && !user.passwordSalt.empty()) {
        dynamicSQL.emplace_back(" password_salt = ?");
        MysqlConn::Param paramSalt;
        paramSalt.type = MysqlConn::Param::STRING;
        paramSalt.str_val = user.passwordSalt;
        params.push_back(paramSalt);
    }

    for (int i = 0; i < dynamicSQL.size(); i++) {
        if (i < dynamicSQL.size() - 1) {
            sql += dynamicSQL[i] + ",";
        }else {
            sql += dynamicSQL[i];
        }
    }
    sql += " WHERE ssid = ?";

    MysqlConn::Param paramSsid;
    paramSsid.type = MysqlConn::Param::STRING;
    paramSsid.str_val = user.ssid;
    params.push_back(paramSsid);

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to update user private info: " << user.ssid);
        return false;
    }

    return true;
}

bool UserPrivateDAO::deleteById(const std::string &ssid) {
    std::string sql = "DELETE FROM user_private_info WHERE ssid = ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramSsid;
    paramSsid.type = MysqlConn::Param::STRING;
    paramSsid.str_val = ssid;
    params.push_back(paramSsid);

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to delete user private info with SSID: " << ssid);
        return false;
    }

    return true;
}

UserPrivateInfoDO UserPrivateDAO::findById(const std::string &ssid) {
    std::string sql = "SELECT ssid, email, password, password_salt, account_status "
                      "FROM user_private_info WHERE ssid = ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramSsid;
    paramSsid.type = MysqlConn::Param::STRING;
    paramSsid.str_val = ssid;
    params.push_back(paramSsid);

    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG_ERROR("Failed to find user private info by SSID: " << ssid);
        return {};
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (!row) {
        mysql_free_result(result);
        return {};
    }

    UserPrivateInfoDO user;
    user.ssid = row[0];
    user.email = row[1];
    user.password = row[2];
    user.passwordSalt = row[3];
    user.accountStatus = static_cast<uint8_t>(std::stoul(row[4]));

    mysql_free_result(result);
    return user;
}

