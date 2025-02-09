//
// Created by FUQAQ on 2025/2/6.
//

#ifndef GROUPSERVICE_H
#define GROUPSERVICE_H

#include "../../dao/group/GroupDAO.h"
#include "../../dto/DataProcessDTO.h"

class GroupService {
public:
    // create group
    std::string createGroup(const GroupBaseInfoDTO& info, const std::vector<GroupMemberInfoDTO>& members);

    // delete group
    bool deleteGroup(const std::string& groupSsid);

    // update group base info
    bool updateGroup(const GroupBaseInfoDTO& info);

    // add notice
    bool addGroupNotice(const std::string &groupSsid, const std::string &notice);

    // get group base info
    GroupBaseInfoDTO getGroupBaseInfo(const std::string& groupSsid);

    // get group member info
    std::vector<UserBaseInfoDTO> getGroupMemberInfo(const std::string& groupSsid);

    // get group notice
    std::vector<std::string> getGroupNoticeInfo(const std::string& groupSsid,int pageSize,int pageNum);
private:
    GroupAdminDAO    groupAdminDAO;
    GroupMemberDAO   groupMemberDAO;
    GroupNoticeDAO   groupNoticeDAO;
    GroupBaseInfoDAO groupBaseInfoDAO;
};



#endif //GROUPSERVICE_H
