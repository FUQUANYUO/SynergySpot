//
// Created by FUQAQ on 2025/1/28.
//

#ifndef MESSAGECONTENTSERVICE_H
#define MESSAGECONTENTSERVICE_H

#include "../../common-dto/CommonDatabaseDTO.h"
#include "../../common-dao/message-content-dao/MessageContentDAO.h"
#include "../../common-dao/message-recipient-dao/MessageRecipientDAO.h"

class MessageContentService {
    explicit MessageContentService(LiteConn& db);

    QList<MessageContentDTO> getAllMessages(const QString& ssid);
    bool storeMessage(QList<MessageContentDTO> &dto);

private:
    MessageContentDAO messageContentDAO;
    MessageRecipientDAO messageRecipientDAO;
};



#endif //MESSAGECONTENTSERVICE_H
