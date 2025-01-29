//
// Created by FUQAQ on 2025/1/27.
//

#ifndef GROUPINFOSERVICE_H
#define GROUPINFOSERVICE_H

#include "../../common-dto/CommonDatabaseDTO.h"
#include "../../common-dao/group-info-dao/GroupInfoDAO.h"

class GroupInfoService {
public:
    explicit GroupInfoService(LiteConn& db);

    GroupBaseInfoDTO getGroupInfoById(const QString& groupId);
    bool updateGroupInfo(const GroupBaseInfoDTO& groupInfo);
private:
    GroupInfoDAO groupInfoDAO;
};



#endif //GROUPINFOSERVICE_H
