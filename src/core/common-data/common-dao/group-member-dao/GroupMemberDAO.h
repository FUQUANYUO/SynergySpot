//
// Created by FUQAQ on 2025/1/26.
//

#ifndef GROUPMEMBERDAO_H
#define GROUPMEMBERDAO_H

#include "../CommonDataBaseDAO.h"

class GroupMemberDAO : public IGroupMemberDAO {
public:
    explicit GroupMemberDAO(LiteConn& db);
    ~GroupMemberDAO() override;
    bool addMember(const GroupMemberInfoDO& member) override;
    bool removeMember(const QString& groupSsid, const QString& memberSsid) override;
    QList<GroupMemberInfoDO> listMembers(const QString& groupSsid, int pageSize = 20, int pageNum = 1) override;
    int getMemberCount(const QString& ssid);
private:
    LiteConn& _db;
};



#endif //GROUPMEMBERDAO_H
