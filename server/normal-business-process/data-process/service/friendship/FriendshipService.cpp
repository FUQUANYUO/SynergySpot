//
// Created by FUQAQ on 2025/2/6.
//

#include "FriendshipService.h"

#include <data-process/dao/group/GroupDAO.h>

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

void FriendshipService::getAllFriendshipBySSID(
    const std::string &ssid,
    std::vector<FriendshipDTO> &res,
    std::vector<UserBaseInfoDTO> &users,
    std::vector<GroupBaseInfoDTO> &groups,
    std::vector<UserBaseInfoDTO> &members)
{
    std::vector<FriendshipDO> resFriendship = friendshipDAO.listByUser(ssid);
    UserDAO          uDAO;
    GroupBaseInfoDAO gDAO;
    GroupMemberDAO   gMemDAO;
    GroupAdminDAO    adDAO;
    for (const auto &it : resFriendship) {
        res.push_back({it.id,it.ssid,it.grouping,it.remark,it.friendSsid,it.shipStatus,it.friendType,it.createTime});
        if (it.friendType == 1) {
            UserBaseInfoDO resInfo = uDAO.findById(it.friendSsid);
            users.push_back({
                resInfo.ssid,
                resInfo.ssname,
                resInfo.avatar,
                resInfo.sex,
                resInfo.personalSign,
                resInfo.thumbUpCount,
                resInfo.birthday,
                resInfo.region,
                resInfo.createTime});
        }
        else if (it.friendType == 2) {
            std::vector<std::string> adminsSSID;
            GroupBaseInfoDO resInfo = gDAO.findBySsid(it.friendSsid);
            std::vector<GroupAdminDO> resAdminInfo = adDAO.getAdmins(resInfo.id);
            std::vector<GroupMemberInfoDO> resMemberInfo = gMemDAO.listMembers(it.friendSsid);
            for (auto memberIt : resMemberInfo) {
                UserBaseInfoDO memberUserBaseInfo = uDAO.findById(memberIt.ssidMember);
                members.push_back({
                    memberUserBaseInfo.ssid,
                    memberUserBaseInfo.ssname,
                    memberUserBaseInfo.avatar,
                    memberUserBaseInfo.sex,
                    memberUserBaseInfo.personalSign,
                    memberUserBaseInfo.thumbUpCount,
                    memberUserBaseInfo.birthday,
                    memberUserBaseInfo.region,
                    memberUserBaseInfo.createTime
                });
            }
            for (auto adIt : resAdminInfo) {
                adminsSSID.push_back(adIt.opSsid);
            }
            groups.push_back({
                resInfo.id,
                resInfo.ssidGroup,
                resInfo.name,
                resInfo.avatar,
                resInfo.createSsid,
                resInfo.profile,
                adminsSSID,
                resInfo.createTime
            });
        }
    }
}

bool FriendshipService::isFriendship(const std::string &ssid, const std::string &otherSSID) {
    return friendshipDAO.findRelationship(ssid,otherSSID).id != -1;
}