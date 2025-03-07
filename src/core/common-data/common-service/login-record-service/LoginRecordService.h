//
// Created by FUQAQ on 2025/2/19.
//

#ifndef LOGINRECORDSERVICE_H
#define LOGINRECORDSERVICE_H

#include "../../common-dto/CommonDatabaseDTO.h"
#include "../../common-dao/login-record-dao/LoginRecordDAO.h"

class LoginRecordService {
public:
    explicit LoginRecordService(LiteConn& db);

    // 添加记录
    qint64 addLoginRecord(const LoginRecordDTO& dto);

    // 删除记录
    bool deleteLoginRecord(const QString& ssid);
    bool deleteLoginRecordsBefore(time_t threshold);

    // 更新时间
    bool updateLoginTime(const QString& ssid,const time_t& date);

    // 查询记录
    LoginRecordDTO        getLoginRecordsBySSID(const QString& ssid);
    QList<LoginRecordDTO> getLoginRecords(int limit = 5);

private:
    // 加密和解密
    QString encryptPassword(const QString& plainPassword);
    QString decryptPassword(const QString& encryptedPassword);
private:
    LoginRecordDAO loginRecordDAO;
};


#endif //LOGINRECORDSERVICE_H
