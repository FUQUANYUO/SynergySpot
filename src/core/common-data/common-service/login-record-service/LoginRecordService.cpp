//
// Created by FUQAQ on 2025/2/19.
//

#include "LoginRecordService.h"

#include <CommonData.h>

#include "encrypt/EncryptString.h"

LoginRecordService::LoginRecordService(LiteConn &db) : loginRecordDAO(db) {
}

qint64 LoginRecordService::addLoginRecord(const LoginRecordDTO &dto) {
    // 转换为 DO 对象
    LoginRecordDO record;
    record.account = dto.account;
    record.loginTime = dto.loginTime;
    record.deviceInfo = dto.deviceInfo;
    record.avatarPath = dto.avatarPath;

    return loginRecordDAO.insert(record);
}

bool LoginRecordService::deleteLoginRecord(const QString& ssid) {
    return loginRecordDAO.deleteBySSID(ssid);
}

bool LoginRecordService::deleteLoginRecordsBefore(time_t threshold) {
    return loginRecordDAO.deleteBefore(threshold);
}

bool LoginRecordService::updateLoginTime(const QString& ssid,const time_t &date) {
    return loginRecordDAO.updateTime(ssid,date);
}

bool LoginRecordService::updateAvatar(const QString &ssid, const QString &path) {
    return loginRecordDAO.updateAvatar(ssid,path);
}

LoginRecordDTO LoginRecordService::getLoginRecordsBySSID(const QString &ssid) {
    auto res = loginRecordDAO.findByAccount(ssid);
    return {
        res.account,res.avatarPath, res.loginTime,res.deviceInfo
    };
}

QList<LoginRecordDTO> LoginRecordService::getLoginRecords(int limit) {
    QList<LoginRecordDTO> records;

    // 调用 DAO 查询记录
    QList<LoginRecordDO> result = loginRecordDAO.listAllRecord(limit);

    // 转换为 DTO 对象
    for (const auto& it : result) {
        LoginRecordDTO dto;
        dto.account = it.account;
        dto.loginTime = it.loginTime;
        dto.deviceInfo = it.deviceInfo;
        dto.avatarPath = it.avatarPath;
        records.append(dto);
    }

    return records;
}