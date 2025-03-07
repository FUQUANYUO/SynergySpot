//
// Created by FUQAQ on 2025/2/6.
//

#include "FriendshipService.h"
#include "../../dao/user/UserDAO.h"

bool FriendshipService::addFriendship(const FriendshipDTO &dto) {
    return friendshipDAO.create({-1,dto.ssid,dto.grouping,dto.remark,dto.friendSsid,dto.shipStatus,dto.friendType});
}

bool FriendshipService::removeFriendship(const FriendshipDTO &dto) {
    return friendshipDAO.deleteById(dto.ssid,dto.friendSsid);
}

bool FriendshipService::updateFriendship(const FriendshipDTO &dto) {
    return friendshipDAO.update({-1,dto.ssid,dto.grouping,dto.remark,dto.friendSsid,dto.shipStatus,dto.friendType});
}

std::vector<UserBaseInfoDTO> FriendshipService::getAllFriendshipBySSID(const std::string &ssid,std::vector<FriendshipDTO>& res) {
    std::vector<UserBaseInfoDTO> friends;
    std::vector<FriendshipDO> resFriendship = friendshipDAO.listByUser(ssid);
    UserDAO infoDAO;
    for (const auto &it : resFriendship) {
        res.push_back({it.id,it.ssid,it.grouping,it.remark,it.friendSsid,it.shipStatus,it.friendType,it.createTime});
        UserBaseInfoDO resInfo = infoDAO.findById(it.friendSsid);
        friends.push_back({resInfo.ssid,resInfo.ssname,resInfo.avatar,resInfo.sex,resInfo.personalSign,resInfo.thumbUpCount,resInfo.birthday,resInfo.region,resInfo.createTime});
    }
    return friends;
}

bool FriendshipService::isFriendship(const std::string &ssid, const std::string &otherSSID) {
    return friendshipDAO.findRelationship(ssid,otherSSID).id != -1;
}