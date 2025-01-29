//
// Created by FUQAQ on 2025/1/27.
//

#include "FriendshipService.h"

FriendshipService::FriendshipService(LiteConn &db) : friendshipDAO(db) {}

QList<FriendshipDTO> FriendshipService::getFriendshipsBySSID(const QString &ssid) {}

bool FriendshipService::addFriendship(const FriendshipDTO &friendship) {}