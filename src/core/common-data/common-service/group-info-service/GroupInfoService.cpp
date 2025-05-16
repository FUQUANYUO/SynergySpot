//
// Created by FUQAQ on 2025/1/27.
//

#include "GroupInfoService.h"
#include "../../CommonData.h"

GroupInfoService::GroupInfoService(LiteConn &db) : groupInfoDAO(db){}

GroupBaseInfoDTO GroupInfoService::getGroupInfoById(const QString &groupId) {
    GroupBaseInfoDO result = groupInfoDAO.findBySsid(groupId);
    return {result.id==-1?"-1":result.ssidGroup,result.name,
    result.avatar.isEmpty()?"":(result.avatar),
        result.createSSID,result.profile,result.admins, result.createTime};
}

QList<GroupBaseInfoDTO> GroupInfoService::getGroupInfos(int pageSize, int pageNum) {
    QList<GroupBaseInfoDTO> dto;
    QList<GroupBaseInfoDO> result = groupInfoDAO.getAllGroupInfos(pageSize, pageNum);
    for (auto it : result) {
        dto.append({it.ssidGroup,it.name,
    it.avatar.isEmpty()?"":(it.avatar),
            it.createSSID,it.profile,it.admins,it.createTime});
    }
    return dto;
}

int GroupInfoService::createGroup(const QString &ssidGroup, const GroupBaseInfoDTO &groupInfo) {
    return groupInfoDAO.createGroup({-1,ssidGroup,groupInfo.groupName,groupInfo.avatarPath,groupInfo.createSSID,groupInfo.profile,groupInfo.admins,groupInfo.createTime});
}

bool GroupInfoService::setGroupInfoData(QList<GroupBaseInfoDTO> dto) {
    bool res = true;
    for (auto it : dto) {
        res = groupInfoDAO.createGroup({-1, it.ssidGroup,it.groupName, it.avatarPath,it.createSSID,it.profile,it.admins,it.createTime});
        if (!res)
            return res;
    }
    return res;
}

bool GroupInfoService::updateGroupInfo(const QString &ssidGroup, const QString &profile) {
    return groupInfoDAO.updateProfile(ssidGroup,profile);
}

bool GroupInfoService::updateAvatarPath(const QString &ssidGroup, const QString &path) {
    return groupInfoDAO.updateAvatar(ssidGroup,path);
}