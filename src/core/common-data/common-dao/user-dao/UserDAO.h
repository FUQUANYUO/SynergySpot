//
// Created by FUQAQ on 2025/1/26.
//

#ifndef USERDAO_H
#define USERDAO_H

#include "../CommonDataBaseDAO.h"

class UserDAO : public IUserDAO{
public:
    UserDAO(LiteConn& db);
    ~UserDAO() override;

    // 核心方法
    bool insert(const UserBaseInfoDO& user) override;
    bool update(const UserBaseInfoDO& user) override;
    bool deleteById(const QString& ssid) override;
    UserBaseInfoDO findById(const QString& ssid) override;

    // 扩展方法
    QList<UserBaseInfoDO> findByRegion(quint8 region,int pageSize = 20, int pageNum = 1) override;
    bool updateThumbUpCount(const QString& ssid, int newCount) override;
    bool updateAvatar(const QString& ssid, const QString& newAvatar);

    int getRegionCount(quint8 region);

private:
    LiteConn& _db;
};



#endif //USERDAO_H
