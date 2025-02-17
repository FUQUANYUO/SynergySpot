//
// Created by FUQAQ on 2025/1/27.
//

#include "GroupInfoService.h"
#include "../../CommonData.h"

GroupInfoService::GroupInfoService(LiteConn &db) : groupInfoDAO(db){}

GroupBaseInfoDTO GroupInfoService::getGroupInfoById(const QString &groupId) {
    GroupBaseInfoDO result = groupInfoDAO.findBySsid(groupId);
    return {result.id==-1?"-1":result.ssidGroup,result.name,
    QString::fromStdString(g_pCommonData->getDataPath(avatar)) + "/" + result.avatar + QString::fromStdString(g_pCommonData->getImageEx()),
        result.createSSID,result.profile,result.createTime};
}

QList<GroupBaseInfoDTO> GroupInfoService::getGroupInfos(int pageSize, int pageNum) {
    QList<GroupBaseInfoDTO> dto;
    QList<GroupBaseInfoDO> result = groupInfoDAO.getAllGroupInfos(pageSize, pageNum);
    for (auto it : result) {
        dto.append({it.ssidGroup,it.name,
    QString::fromStdString(g_pCommonData->getDataPath(avatar)) + "/" + it.avatar + QString::fromStdString(g_pCommonData->getImageEx()),
            it.createSSID,it.profile,it.createTime});
    }
    return dto;
}

int GroupInfoService::createGroup(const QString &ssidGroup, const GroupBaseInfoDTO &groupInfo) {
    return groupInfoDAO.createGroup({-1,ssidGroup,groupInfo.groupName,groupInfo.avatarPath,groupInfo.createSSID,groupInfo.profile,groupInfo.createTime});
}

bool GroupInfoService::setGroupInfoData(QList<GroupBaseInfoDTO> dto) {
    bool res = true;
    for (auto it : dto) {
        res = groupInfoDAO.createGroup({-1, it.ssidGroup,it.groupName, it.avatarPath,it.createSSID,it.profile,it.createTime});
        if (!res)
            return res;
    }
    return res;
}

bool GroupInfoService::updateGroupInfo(const QString &ssidGroup, const QString &profile) {
    return groupInfoDAO.updateProfile(ssidGroup,profile);
}