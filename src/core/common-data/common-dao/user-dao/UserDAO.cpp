//
// Created by FUQAQ on 2025/1/26.
//

#include "UserDAO.h"


UserDAO::UserDAO(LiteConn &db) : _db(db) {}

UserDAO::~UserDAO() {}

bool UserDAO::insert(const UserBaseInfoDO &user) {
    std::string sql = "INSERT INTO user_base_info (ssid, ssname, avatar, sex, personal_sign, thumb_up_count, birthday, create_time, region) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";
    std::vector<std::string> params = {
        user.ssid.toStdString(),
        user.ssname.toStdString(),
        user.avatar.toStdString(),
        (user.sex == UserSex::Male) ? "M" : "F", // 枚举转换为 "M" 或 "F"
        user.personalSign.toStdString(),
        std::to_string(user.thumbUpCount),
        user.birthday.toString("yyyy-MM-dd").toStdString(),
        user.createTime.toString("yyyy-MM-dd HH:mm:ss").toStdString(),
        std::to_string(user.region)
    };

    return _db.update(sql, params);
}

bool UserDAO::update(const UserBaseInfoDO &user) {
    std::string sql = "UPDATE user_base_info SET ssname = ?, avatar = ?, sex = ?, personal_sign = ?, "
                      "thumb_up_count = ?, birthday = ?, region = ? WHERE ssid = ?;";
    std::vector<std::string> params = {
        user.ssname.toStdString(),
        user.avatar.toStdString(),
        (user.sex == UserSex::Male) ? "M" : "F", // 枚举转换为 "M" 或 "F"
        user.personalSign.toStdString(),
        std::to_string(user.thumbUpCount),
        user.birthday.toString("yyyy-MM-dd").toStdString(),
        std::to_string(user.region),
        user.ssid.toStdString()
    };

    return _db.update(sql, params);
}

bool UserDAO::deleteById(const QString &ssid) {
    std::string sql = "DELETE FROM user_base_info WHERE ssid = ?;";
    std::vector<std::string> params = { ssid.toStdString() };

    return _db.update(sql, params);
}

UserBaseInfoDO UserDAO::findById(const QString &ssid) {
    std::string sql = "SELECT ssid, ssname, avatar, sex, personal_sign, thumb_up_count, birthday, create_time, region "
                      "FROM user_base_info WHERE ssid = ?;";
    std::vector<std::string> params = { ssid.toStdString() };

    auto result = _db.query(sql, params);
    if (result.size() > 0) {
        UserBaseInfoDO user;
        user.ssid = QString::fromStdString(result[0][0]);
        user.ssname = QString::fromStdString(result[0][1]);
        user.avatar = QString::fromStdString(result[0][2]);
        user.sex = (result[0][3] == "M") ? UserSex::Male : UserSex::Female; // "M" 或 "F" 转换为枚举
        user.personalSign = QString::fromStdString(result[0][4]);
        user.thumbUpCount = std::stoul(result[0][5]);
        user.birthday = QDateTime::fromString(QString::fromStdString(result[0][6]), "yyyy-MM-dd");
        user.createTime = QDateTime::fromString(QString::fromStdString(result[0][7]), "yyyy-MM-dd HH:mm:ss");
        user.region = static_cast<quint8>(std::stoi(result[0][8]));
        return user;
    }
    return UserBaseInfoDO();
}

QList<UserBaseInfoDO> UserDAO::findByRegion(quint8 region, int pageSize, int pageNum) {
    std::string sql = "SELECT ssid, ssname, avatar, sex, personal_sign, thumb_up_count, birthday, create_time, region "
                      "FROM user_base_info WHERE region = ? LIMIT ? OFFSET ?;";
    std::vector<std::string> params = { std::to_string(region) , std::to_string(pageSize), std::to_string(pageNum)};

    auto result = _db.query(sql, params);
    QList<UserBaseInfoDO> users;
    for (const auto &row : result) {
        UserBaseInfoDO user;
        user.ssid = QString::fromStdString(row[0]);
        user.ssname = QString::fromStdString(row[1]);
        user.avatar = QString::fromStdString(row[2]);
        user.sex = (row[3] == "M") ? UserSex::Male : UserSex::Female; // "M" 或 "F" 转换为枚举
        user.personalSign = QString::fromStdString(row[4]);
        user.thumbUpCount = std::stoul(row[5]);
        user.birthday = QDateTime::fromString(QString::fromStdString(row[6]), "yyyy-MM-dd");
        user.createTime = QDateTime::fromString(QString::fromStdString(row[7]), "yyyy-MM-dd HH:mm:ss");
        user.region = static_cast<quint8>(std::stoi(row[8]));
        users.append(user);
    }
    return users;
}

bool UserDAO::updateAvatar(const QString &ssid, const QString &newAvatar) {
    std::string sql = "UPDATE user_base_info SET avatar = ? WHERE ssid = ?;";
    std::vector<std::string> params = {
        newAvatar.toStdString(),
        ssid.toStdString()
    };

    return _db.update(sql, params);
}

int UserDAO::getRegionCount(quint8 region) {
    std::string sql = "SELECT COUNT(*) FROM user_base_info WHERE region = ?";
    std::vector<std::string> params = { std::to_string(region) };

    auto result = _db.query(sql, params);

    if (!result.empty() && !result[0].empty()) {
        return std::stoi(result[0][0]);
    }else {
        return 0;
    }
}

bool UserDAO::updateThumbUpCount(const QString &ssid, int newCount) {
    std::string sql = "UPDATE user_base_info SET thumb_up_count = ? WHERE ssid = ?;";
    std::vector<std::string> params = {
        std::to_string(newCount),
        ssid.toStdString()
    };
    return _db.update(sql, params);
}