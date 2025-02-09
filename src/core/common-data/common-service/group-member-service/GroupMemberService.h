//
// Created by FUQAQ on 2025/1/27.
//

#ifndef GROUPMEMBERSERVICE_H
#define GROUPMEMBERSERVICE_H

#include "../../common-dto/CommonDatabaseDTO.h"
#include "../../common-dao/group-member-dao/GroupMemberDAO.h"

class GroupMemberService {
public:
    explicit GroupMemberService(LiteConn& db);

    QList<GroupMemberInfoDTO> getAllGroupMember(const QString& ssidGroup, int pageSize = 20, int pageNum = 1);
    bool addGroupMember(QList<GroupMemberInfoDTO> dto);
    bool removeGroupMember(const QString& ssidGroup, const QString& ssidMember);

    int getGroupMemberCount(const QString& ssidGroup);
private:
    GroupMemberDAO groupMemberDAO;
};



#endif //GROUPMEMBERSERVICE_H
