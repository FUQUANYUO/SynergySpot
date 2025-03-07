//
// Created by FUQAQ on 2025/2/6.
//

#include "MessageService.h"

std::vector<MessageContentDTO> MessageService::getUserAllMessages(const MessageContentDTO &dto) {
    std::vector<MessageContentDTO> msgDTOs;
    std::vector<MessageContentDO>  msg = messageContentDAO.listBySender(dto.senderSsid,dto.pageSize,dto.pageNum,dto.createTime);
    for (const auto& it : msg) {
        MessageRecipientDO reDO= it.recipient;

        std::vector<std::string> files;
        files.reserve(it.fileIds.size());
        for (const auto& file : it.fileIds) {
            files.push_back(file);
        }
        MessageContentDTO msgDTO;
        msgDTO.id = it.id;
        msgDTO.senderSsid = it.senderSsid;
        msgDTO.contentType = it.contentType;
        msgDTO.content = it.content;
        msgDTO.fileId = files;
        msgDTO.recipient = {reDO.id,reDO.messageId,reDO.recipientType,reDO.recipientSsid,reDO.readStatus};
        msgDTO.createTime = it.createTime;
        msgDTOs.push_back(msgDTO);
    }
    return msgDTOs;
}

bool MessageService::insertUserMessage(const MessageContentDTO &content) {
    int messageID = messageContentDAO.insert({-1,content.senderSsid,content.contentType,content.content,content.fileId});
    return messageRecipientDAO.insert({-1,messageID,content.recipient.recipientType,content.recipient.recipientSsid,content.recipient.readStatus});
}
