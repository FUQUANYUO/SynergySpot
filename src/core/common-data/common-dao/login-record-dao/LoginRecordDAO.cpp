//
// Created by FUQAQ on 2025/2/18.
//

#include "LoginRecordDAO.h"

LoginRecordDAO::LoginRecordDAO(LiteConn &db) : _db(db) {}

qint64 LoginRecordDAO::insert(const LoginRecordDO &record) {
    // 构建参数列表
    std::vector<std::string> params{
        record.account.toStdString(),
        record.avatarPath.toStdString(),
        std::to_string(record.loginTime),
        record.deviceInfo.toStdString()
    };

    // 执行插入
    bool success = _db.update(
        "INSERT INTO login_record (account, avatar, login_time, device_info) "
        "VALUES (?, ?, ?, ?)",
        params
    );

    if (!success) {
        LOG_ERROR("Insert login record failed")
        return -1;
    }

    // 获取最后插入ID
    auto result = _db.query("SELECT last_insert_rowid()", {});
    if (result.empty() || result[0].empty()) {
        LOG_ERROR("Failed to get last insert ID")
        return -1;
    }

    return std::stoll(result[0][0]);
}

LoginRecordDO LoginRecordDAO::findByAccount(const QString &ssid) {
    // 构建参数列表
    std::vector<std::string> params{
        ssid.toStdString()
    };

    // 执行查询
    auto result = _db.query(
        "SELECT id, account, avatar, login_time, device_info "
        "FROM login_record "
        "WHERE account = ? ",
        params
    );

    // 解析结果
    LoginRecordDO record;
    if (result.empty() || result[0].empty()) {
        return {-1,"-1"};
    }
    for (const auto& row : result) {
        record.id = std::stoll(row[0]);
        record.account = QString::fromStdString(row[1]);
        record.avatarPath = QString::fromStdString(row[2]);
        record.loginTime = std::stoll(row[3]);
        record.deviceInfo = QString::fromStdString(row[4]);
    }

    return record;
}

QList<LoginRecordDO> LoginRecordDAO::listAllRecord(int limit) {
    QList<LoginRecordDO> records;

    // 构建参数列表
    std::vector<std::string> params{
        std::to_string(limit)
    };

    // 执行查询
    auto result = _db.query(
        "SELECT id, account, avatar, login_time, device_info "
        "FROM login_record "
        "ORDER BY login_time DESC "
        "LIMIT ?",
        params
    );

    // 解析结果
    for (const auto& row : result) {
        LoginRecordDO record;
        record.id = std::stoll(row[0]);
        record.account = QString::fromStdString(row[1]);
        record.avatarPath = QString::fromStdString(row[2]);
        record.loginTime = std::stoll(row[3]);
        record.deviceInfo = QString::fromStdString(row[4]);
        records.append(record);
    }

    return records;
}

bool LoginRecordDAO::deleteBySSID(const QString &ssid) {
    // 构建参数列表
    std::vector<std::string> params{
        ssid.toStdString()
    };

    // 执行删除
    bool success = _db.update(
        "DELETE FROM login_record WHERE account = ?",
        params
    );

    if (!success) {
        LOG_ERROR("Delete by ssid error!")
        return false;
    }
    return true;
}

bool LoginRecordDAO::deleteBefore(time_t threshold) {
    // 构建参数列表
    std::vector<std::string> params{
        std::to_string(threshold)
    };

    // 执行删除
    bool success = _db.update(
        "DELETE FROM login_record WHERE login_time < ?",
        params
    );

    if (!success) {
        LOG_ERROR("Delete old login records failed")
        return false;
    }

    return true;
}

bool LoginRecordDAO::updateTime(const QString& ssid,time_t date) {
    std::string sql = "UPDATE login_record SET login_time = ? WHERE account = ?;";
    std::vector<std::string> params = {
        ssid.toStdString(),
        std::to_string(date)
    };

    return _db.update(sql, params);
}

bool LoginRecordDAO::updateAvatar(const QString &ssid, const QString &path) {
    std::string sql = "UPDATE login_record SET avatar = ? WHERE account = ?;";
    std::vector<std::string> params = {
        ssid.toStdString(),
        path.toStdString()
    };

    return _db.update(sql, params);
}