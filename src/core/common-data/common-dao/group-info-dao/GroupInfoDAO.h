//
// Created by FUQAQ on 2025/1/26.
//

#ifndef GROUPINFODAO_H
#define GROUPINFODAO_H

#include "../CommonDataBaseDAO.h"

class GroupInfoDAO : public IGroupInfoDAO{
public:
    explicit GroupInfoDAO(LiteConn& db);
    ~GroupInfoDAO() override;

    qint64 createGroup(const GroupBaseInfoDO& group) override;
    bool updateProfile(const QString& groupSsid, const QString& newProfile) override;
    GroupBaseInfoDO findBySsid(const QString& groupSsid) override;
    QList<GroupBaseInfoDO> getAllGroupInfos(int pageSize = 20, int pageNum = 1) override;

    int getGroupInfoDataCount();
private:
    LiteConn& _db;
};



#endif //GROUPINFODAO_H
