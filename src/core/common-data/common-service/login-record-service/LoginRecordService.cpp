//
// Created by FUQAQ on 2025/2/19.
//

#include "LoginRecordService.h"

#include <CommonData.h>

#include "encrypt/EncryptString.h"

LoginRecordService::LoginRecordService(LiteConn &db) : loginRecordDAO(db) {
}

QString LoginRecordService::encryptPassword(const QString &plainPassword) {
    std::string encryptionKey = g_pCommonData->getYamlNode()["sqlite-info"]["accessKey"].as<std::string>();
    return QString::fromStdString(
        EncryptString::getEncryptObj()->encryptStringByAES(plainPassword.toStdString(),encryptionKey)
    );
}

QString LoginRecordService::decryptPassword(const QString &encryptedPassword) {
    std::string encryptionKey = g_pCommonData->getYamlNode()["sqlite-info"]["accessKey"].as<std::string>();
    return QString::fromStdString(
        EncryptString::getEncryptObj()->decryptStringByAES(encryptedPassword.toStdString(),encryptionKey)
    );
}

qint64 LoginRecordService::addLoginRecord(const LoginRecordDTO &dto) {
    // 加密密码
    QString encryptedPassword = encryptPassword(dto.plainPassword);

    // 转换为 DO 对象
    LoginRecordDO record;
    record.account = dto.account;
    record.encryptedPassword = encryptedPassword;
    record.loginTime = dto.loginTime;
    record.deviceInfo = dto.deviceInfo;

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

LoginRecordDTO LoginRecordService::getLoginRecordsBySSID(const QString &ssid) {
    auto res = loginRecordDAO.findByAccount(ssid);
    return {
        res.account,res.encryptedPassword,res.loginTime,res.deviceInfo
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
        dto.plainPassword = decryptPassword(it.encryptedPassword);
        dto.loginTime = it.loginTime;
        dto.deviceInfo = it.deviceInfo;
        records.append(dto);
    }

    return records;
}