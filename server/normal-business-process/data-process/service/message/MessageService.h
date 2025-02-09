//
// Created by FUQAQ on 2025/2/6.
//

#ifndef MESSAGESERVICE_H
#define MESSAGESERVICE_H

#include "../../dao/message/MessageDAO.h"
#include "../../dto/DataProcessDTO.h"

class MessageService {
public:
    // get all message for ssid
    std::vector<MessageContentDTO> getUserAllMessages(const MessageContentDTO& dto);

    // store user message
    bool insertUserMessage(const MessageContentDTO& content);
private:
    MessageContentDAO   messageContentDAO;
    MessageRecipientDAO messageRecipientDAO;
};



#endif //MESSAGESERVICE_H
