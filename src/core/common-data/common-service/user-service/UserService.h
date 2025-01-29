//
// Created by FUQAQ on 2025/1/27.
//

#ifndef USERSERVICE_H
#define USERSERVICE_H

#include "../../common-dto/CommonDatabaseDTO.h"
#include "../../common-dao/user-dao/UserDAO.h"

class UserService {
public:
    explicit UserService(LiteConn& db);

    UserBaseInfoDTO getUserBySSID(const QString& ssid);
    bool updateUserBySSID(const QString& ssid,const UserBaseInfoDTO& dto);
    bool updateThumbUpCount(const QString& ssid,int newCount);
    bool updateAvatarPath(const QString& ssid, const QString& newUrl);
private:
    UserDAO userDAO;
};



#endif //USERSERVICE_H
