//
// Created by FUQAQ on 2025/1/28.
//

#include "MessageContentService.h"

MessageContentService::MessageContentService(LiteConn &db) : messageContentDAO(db){}

QList<MessageContentDTO> MessageContentService::getAllMessages(const QString &ssid, int pageSize, int pageNum) {
    QList<MessageContentDTO> messages;
    QList<QVariant> result = messageContentDAO.listMessagesByRecipient(ssid, pageSize,  pageNum);
    for (auto it : result ) {
        messages.append(it.value<MessageContentDTO>());
    }
    return messages;
}

bool MessageContentService::storeMessage(const QList<MessageContentDTO> &dto) {
    QList<QVariant> messages;
    for (auto it : dto) {
        messages.append(QVariant::fromValue(it));
    }
    return messageContentDAO.insertMessageTransaction(messages);
}

int MessageContentService::getMessageCount(const QString &ssid) {
    return messageContentDAO.getMessageCount(ssid);
}