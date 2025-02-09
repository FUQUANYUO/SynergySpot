//
// Created by FUQAQ on 2025/1/27.
//

#include "GroupMemberService.h"

GroupMemberService::GroupMemberService(LiteConn &db) : groupMemberDAO(db) {}

QList<GroupMemberInfoDTO> GroupMemberService::getAllGroupMember(const QString& ssidGroup,int pageSize, int pageNum) {
    QList<GroupMemberInfoDTO> member;
    QList<GroupMemberInfoDO> result = groupMemberDAO.listMembers(ssidGroup, pageSize,pageNum);
    for (auto it : result) {
        member.append({it.ssidGroup,it.ssidMember,it.createTime});
    }
    return member;
}

bool GroupMemberService::addGroupMember(QList<GroupMemberInfoDTO> dto) {
    bool result = true;
    for (auto it : dto) {
        result = groupMemberDAO.addMember({-1,it.ssidGroup,it.ssidMember});
        if (!result) {
            return result;
        }
    }
    return result;
}

bool GroupMemberService::removeGroupMember(const QString &ssidGroup, const QString &ssidMember) {
    return groupMemberDAO.removeMember(ssidGroup,ssidMember);
}

int GroupMemberService::getGroupMemberCount(const QString &ssidGroup) {
    return groupMemberDAO.getMemberCount(ssidGroup);
}