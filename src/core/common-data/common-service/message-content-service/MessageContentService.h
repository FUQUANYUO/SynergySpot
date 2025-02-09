//
// Created by FUQAQ on 2025/1/28.
//

#ifndef MESSAGECONTENTSERVICE_H
#define MESSAGECONTENTSERVICE_H

#include "../../common-dto/CommonDatabaseDTO.h"
#include "../../common-dao/message-content-dao/MessageContentDAO.h"

class MessageContentService {
public:
    explicit MessageContentService(LiteConn& db);

    QList<MessageContentDTO> getAllMessages(const QString& ssid, int pageSize = 20, int pageNum = 1);
    bool storeMessage(const QList<MessageContentDTO> &dto);

    int getMessageCount(const QString& ssid);
private:
    MessageContentDAO messageContentDAO;
};



#endif //MESSAGECONTENTSERVICE_H
