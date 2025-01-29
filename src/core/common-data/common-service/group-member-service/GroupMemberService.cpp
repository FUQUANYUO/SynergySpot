//
// Created by FUQAQ on 2025/1/27.
//

#include "GroupMemberService.h"

GroupMemberService::GroupMemberService(LiteConn &db) : groupMemberDAO(db) {}

QList<GroupMemberInfoDTO> GroupMemberService::getAllGroupMember() {}

bool GroupMemberService::addGroupMember(QList<GroupMemberInfoDTO> dto) {}