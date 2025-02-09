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
    QList<GroupBaseInfoDTO> getGroupInfos(int pageSize, int pageNum);
    int createGroup(const QString &ssidGroup, const GroupBaseInfoDTO& groupInfo);

    bool setGroupInfoData(QList<GroupBaseInfoDTO> dto);
    bool updateGroupInfo(const QString& ssidGroup,const QString& profile);
private:
    GroupInfoDAO groupInfoDAO;
};



#endif //GROUPINFOSERVICE_H
