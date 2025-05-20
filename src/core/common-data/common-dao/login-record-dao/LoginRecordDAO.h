//
// Created by FUQAQ on 2025/2/18.
//

#ifndef LOGINRECORDDAO_H
#define LOGINRECORDDAO_H

#include "../CommonDataBaseDAO.h"
#include <QList>

class LoginRecordDAO : public ILoginRecordDAO {
public:
    explicit LoginRecordDAO(LiteConn& db);

    // 核心操作
    qint64 insert(const LoginRecordDO& record) override;
    LoginRecordDO findByAccount(const QString& ssid) override;

    QList<LoginRecordDO> listAllRecord(int limit = 10);

    bool deleteBySSID(const QString& ssid);
    bool deleteBefore(time_t threshold) override;

    bool updateTime(const QString& ssid,time_t date);
    bool updateAvatar(const QString &ssid, const QString &path);

private:
    LiteConn& _db;  // 数据库连接引用
};



#endif //LOGINRECORDDAO_H
