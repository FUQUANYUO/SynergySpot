//
// Created by FUQAQ on 2025/1/26.
//

#ifndef MESSAGECONTENTDAO_H
#define MESSAGECONTENTDAO_H

#include "../CommonDataBaseDAO.h"

class MessageContentDAO : public IMessageDAO {
public:
    MessageContentDAO(LiteConn &db);
    ~MessageContentDAO() override;
    // 消息内容操作
    qint64 insertMessageContent(const MessageContentDO& message) override;
    bool insertMessageRecipient(const MessageRecipientDO& recipient) override;
    bool insertMessageTransaction(QList<QVariant> messages) override;

    // 消息查询
    bool markMessageAsRead(qint64 messageId, const QString& recipientSSID) override;
    QList<QVariant> listMessagesByRecipient(const QString& recipientSSID, int pageSize = 20, int pageNum = 1) override;
    int getMessageCount(const QString& ssid);
private:
    LiteConn& _db;
};



#endif //MESSAGECONTENTDAO_H
