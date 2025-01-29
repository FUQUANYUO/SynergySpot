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

    QList<FriendshipDTO> getFriendshipsBySSID(const QString& ssid);
    bool addFriendship(const FriendshipDTO& friendship);

private:
    FriendshipDAO friendshipDAO;
};



#endif //FRIENDSHIPSERVICE_H
