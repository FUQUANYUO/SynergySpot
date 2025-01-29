//
// Created by FUQAQ on 2025/1/27.
//

#include "GroupInfoService.h"

GroupInfoService::GroupInfoService(LiteConn &db) : groupInfoDAO(db){}

GroupBaseInfoDTO GroupInfoService::getGroupInfoById(const QString &groupId) {}

bool GroupInfoService::updateGroupInfo(const GroupBaseInfoDTO &groupInfo) {}