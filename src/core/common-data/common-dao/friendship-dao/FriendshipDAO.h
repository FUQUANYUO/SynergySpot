//
// Created by FUQAQ on 2025/1/26.
//

#ifndef FRIENDSHIPDAO_H
#define FRIENDSHIPDAO_H

#include "../CommonDataBaseDAO.h"

class FriendshipDAO : public IFriendshipDAO{
public:
    explicit FriendshipDAO(LiteConn& db);
    ~FriendshipDAO() override;

    qint64 create(const FriendshipDO& friendship) override;
    bool updateStatus(qint64 id, qint32 newStatus) override;
    FriendshipDO findRelationship(const QString& ssid, const QString& friendSsid) override;
    QList<FriendshipDO> listByUser(const QString& ssid) override;
private:
    LiteConn& _db;
};



#endif //FRIENDSHIPDAO_H
