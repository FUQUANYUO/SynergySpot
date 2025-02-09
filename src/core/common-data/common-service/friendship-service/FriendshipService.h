//
// Created by FUQAQ on 2025/1/27.
//

#ifndef FRIENDSHIPSERVICE_H
#define FRIENDSHIPSERVICE_H

#include "../../common-dto/CommonDatabaseDTO.h"
#include "../../common-dao/friendship-dao/FriendshipDAO.h"

class FriendshipService {
public:
    explicit FriendshipService(LiteConn& db);

    int createFriendship(const FriendshipDTO& dto);
    QList<FriendshipDTO> getFriendshipsBySSID(const QString& ssid);
    bool isCurUserFriend(const QString& curSSID,const QString& otherSSID);
    bool setFriendshipData(QList<FriendshipDTO> dto);
private:
    FriendshipDAO friendshipDAO;
};



#endif //FRIENDSHIPSERVICE_H
