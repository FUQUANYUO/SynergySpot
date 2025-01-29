//
// Created by FUQAQ on 2025/1/26.
//

#ifndef MESSAGERECIPIENTDAO_H
#define MESSAGERECIPIENTDAO_H

#include "../CommonDataBaseDAO.h"

class MessageRecipientDAO : public IMessageRecipientDAO{
public:
    explicit MessageRecipientDAO(LiteConn& db);
    ~MessageRecipientDAO() override;
    bool addRecipient(const MessageRecipientDO& recipient) override;
    bool markAsRead(qint64 recipientId) override;
    QList<MessageRecipientDO> listUnread(const QString& userSSID) override;
private:
    LiteConn& _db;
};



#endif //MESSAGERECIPIENTDAO_H
