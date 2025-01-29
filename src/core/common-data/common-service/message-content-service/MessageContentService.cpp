//
// Created by FUQAQ on 2025/1/28.
//

#include "MessageContentService.h"

MessageContentService::MessageContentService(LiteConn &db) : messageContentDAO(db) , messageRecipientDAO(db){}

QList<MessageContentDTO> MessageContentService::getAllMessages(const QString &ssid) {
    QList<MessageContentDTO> messages;
    QList<MessageContentDO> result =
}

bool MessageContentService::storeMessage(QList<MessageContentDTO> &dto) {}