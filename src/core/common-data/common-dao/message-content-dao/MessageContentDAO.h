//
// Created by FUQAQ on 2025/1/26.
//

#ifndef MESSAGECONTENTDAO_H
#define MESSAGECONTENTDAO_H

#include "../CommonDataBaseDAO.h"

class MessageContentDAO : public IMessageContentDAO {
public:
    explicit MessageContentDAO(LiteConn& db);
    ~MessageContentDAO() override;
    qint64 insertMessage(const MessageContentDO& message) override;
    MessageContentDO findMessageById(qint64 id) override;
    QList<MessageContentDO> listMessagesBySender(const QString& senderSsid) override;
private:
    LiteConn& _db;
};



#endif //MESSAGECONTENTDAO_H
