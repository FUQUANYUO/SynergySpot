//
// Created by FUQAQ on 2025/2/6.
//

#include "MessageService.h"

std::vector<MessageContentDTO> MessageService::getUserAllMessages(const MessageContentDTO &dto) {
    std::vector<MessageContentDTO> msgDTO;
    std::vector<MessageContentDO>  msg = messageContentDAO.listBySender(dto.senderSsid,dto.pageSize,dto.pageSize);
    for (const auto& it : msg) {
        MessageRecipientDO reDO= messageRecipientDAO.getRecipientData(it.id);
        msgDTO.push_back({it.id,it.senderSsid,it.contentType,it.content,it.fileId,
            {reDO.id,reDO.messageId,reDO.recipientType,reDO.recipientSsid,reDO.readStatus},
            it.createTime
        });
    }
    return msgDTO;
}

bool MessageService::insertUserMessage(const MessageContentDTO &content) {
    int messageID = messageContentDAO.insert({-1,content.senderSsid,content.contentType,content.content,content.fileId});
    return messageRecipientDAO.insert({-1,messageID,content.recipient.recipientType,content.recipient.recipientSsid,content.recipient.readStatus});;
}