//
// Created by FUQAQ on 2025/1/27.
//

#include "FriendshipService.h"

FriendshipService::FriendshipService(LiteConn &db) : friendshipDAO(db) {}

int FriendshipService::createFriendship(const FriendshipDTO &dto) {
    return friendshipDAO.create({-1,dto.ssid,dto.groupingName,dto.friendSSID,dto.shipStatus,dto.friendType,dto.createTime});
}

QList<FriendshipDTO> FriendshipService::getFriendshipsBySSID(const QString &ssid) {
    QList<FriendshipDTO> friendships;
    QList<FriendshipDO> result = friendshipDAO.listByUser(ssid);
    for (const auto &it : result) {
        friendships.append({it.ssid,it.grouping,it.friendSSID,it.shipStatus,it.friendType,it.createTime});
    }
    return friendships;
}

bool FriendshipService::isCurUserFriend(const QString &curSSID, const QString &otherSSID) {
    return !(friendshipDAO.findRelationship(curSSID,otherSSID).isEmpty());
}

bool FriendshipService::setFriendshipData(QList<FriendshipDTO> dto) {
    bool res = true;
    for (auto it : dto) {
        res = friendshipDAO.create({-1,it.ssid,it.groupingName,it.friendSSID,it.shipStatus,it.friendType,it.createTime});
        if (!res)
            return res;
    }
    return res;
}
