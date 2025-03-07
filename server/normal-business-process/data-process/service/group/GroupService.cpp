//
// Created by FUQAQ on 2025/2/6.
//

#include "GroupService.h"
#include "../../dao/user/UserDAO.h"

std::string GroupService::createGroup(const GroupBaseInfoDTO &info, const std::vector<GroupMemberInfoDTO> &members) {
    std::string newGroupSSID = groupBaseInfoDAO.getLastGroupSSID();
    if (newGroupSSID != "-1") {
        newGroupSSID = newGroupSSID.substr(1,newGroupSSID.length()-1);
        newGroupSSID = "G" + std::to_string(std::stoi(newGroupSSID) + 1);
    }else {
        LOG("new GroupSSID dont find")
        return "-1";
    }
    int groupID = groupBaseInfoDAO.createGroup({-1,newGroupSSID ,info.name,info.avatar,info.createSsid,info.profile});
    for (auto member : members) {
        groupMemberDAO.addMember(newGroupSSID,member.ssidMember);
    }
    return newGroupSSID;
}

bool GroupService::deleteGroup(const std::string &groupSsid) {
    return groupBaseInfoDAO.deleteById(groupSsid);
}

bool GroupService::updateGroup(const GroupBaseInfoDTO &info) {
    return groupBaseInfoDAO.update({-1,info.ssidGroup,info.name,info.avatar,info.createSsid,info.profile});
}

bool GroupService::addGroupNotice(const std::string &groupSsid, const std::string &notice) {
    return groupNoticeDAO.addNotice(groupBaseInfoDAO.findBySsid(groupSsid).id,notice);
}

GroupBaseInfoDTO GroupService::getGroupBaseInfo(const std::string &groupSsid) {
    GroupBaseInfoDO resBase = groupBaseInfoDAO.findBySsid(groupSsid);
    std::vector<GroupAdminDO> resAdmin = groupAdminDAO.getAdmins(resBase.id);
    std::vector<std::string> admins;
    for (const auto& it : resAdmin ) {
        admins.push_back(it.opSsid);
    }
    return {resBase.id,resBase.ssidGroup,resBase.name,resBase.avatar,resBase.createSsid,resBase.profile,admins,resBase.createTime};
}

std::vector<UserBaseInfoDTO> GroupService::getGroupMemberInfo(const std::string &groupSsid) {
    std::vector<UserBaseInfoDTO> members;
    std::vector<GroupMemberInfoDO> res = groupMemberDAO.listMembers(groupSsid);
    UserDAO infoDAO;
    for (const auto& it : res) {
        UserBaseInfoDO resInfo = infoDAO.findById(it.ssidMember);
        members.push_back({resInfo.ssid,resInfo.ssname,resInfo.avatar,resInfo.sex,resInfo.personalSign,resInfo.thumbUpCount,resInfo.birthday,resInfo.region,resInfo.createTime});
    }
    return members;
}

std::vector<std::string> GroupService::getGroupNoticeInfo(const std::string &groupSsid,int pageSize,int pageNum) {
    std::vector<std::string> notices;
    std::vector<GroupNoticeDO> res = groupNoticeDAO.listNotices(groupBaseInfoDAO.findBySsid(groupSsid).id,pageSize,pageNum);
    for (const auto& it : res) {
        notices.push_back(it.notice);
    }
    return notices;
}

std::vector<GroupBaseInfoDTO> GroupService::fuzzyMatch(const std::string &ssid, const std::string &name) {
    std::vector<GroupBaseInfoDTO> userDto;
    std::vector<GroupBaseInfoDO> res = groupBaseInfoDAO.fuzzyMatchingByIdOrName(ssid,name);
    for (const auto &it : res) {
        std::vector<GroupAdminDO> resAdmin = groupAdminDAO.getAdmins(it.id);
        std::vector<std::string> admins;
        for (const auto& ad : resAdmin ) {
            admins.push_back(ad.opSsid);
        }
        userDto.push_back({it.id,it.ssidGroup,it.name,it.avatar,it.createSsid,it.profile,admins,it.createTime});
    }
    return userDto;
}
