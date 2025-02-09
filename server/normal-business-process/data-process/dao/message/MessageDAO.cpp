//
// Created by FUQAQ on 2025/2/6.
//

#include "MessageDAO.h"

int64_t MessageContentDAO::insert(const MessageContentDO &message)  {
    std::string sql = "INSERT INTO message_content (sender_ssid, content_type, content, file_id) "
                      "VALUES (?, ?, ?, ?)";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramSender;
    paramSender.type = MysqlConn::Param::STRING;
    paramSender.str_val = message.senderSsid;
    params.push_back(paramSender);

    MysqlConn::Param paramType;
    paramType.type = MysqlConn::Param::INT;
    paramType.int_val = message.contentType;
    params.push_back(paramType);

    MysqlConn::Param paramContent;
    paramContent.type = MysqlConn::Param::STRING;
    paramContent.str_val = message.content;
    params.push_back(paramContent);

    MysqlConn::Param paramFileId;
    paramFileId.type = MysqlConn::Param::STRING;
    paramFileId.str_val = message.fileId;
    params.push_back(paramFileId);

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to insert message content for sender: " << message.senderSsid);
        return -1;
    }

    return static_cast<int64_t>(m_conn->getLastInsertId());
}

std::vector<MessageContentDO> MessageContentDAO::listBySender(const std::string &senderSsid, int pageSize, int pageNum) {
    std::string sql = "SELECT id, sender_ssid, content_type, content, file_id, create_time FROM message_content "
                      "WHERE sender_ssid = ? "
                      "LIMIT ? OFFSET ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramSender;
    paramSender.type = MysqlConn::Param::STRING;
    paramSender.str_val = senderSsid;
    params.push_back(paramSender);

    MysqlConn::Param paramPageSize;
    paramPageSize.type = MysqlConn::Param::INT;
    paramPageSize.int_val = pageSize;
    params.push_back(paramPageSize);

    MysqlConn::Param paramOffset;
    paramOffset.type = MysqlConn::Param::INT;
    paramOffset.int_val = (pageNum - 1) * pageSize;
    params.push_back(paramOffset);

    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG_ERROR("Failed to list messages by sender: " << senderSsid);
        return {};
    }

    std::vector<MessageContentDO> messages;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        MessageContentDO message;
        message.id = std::stoll(row[0]);
        message.senderSsid = row[1];
        message.contentType = static_cast<uint8_t>(std::stoul(row[2]));
        message.content = row[3];
        message.fileId = row[4];
        message.createTime = row[5] ? std::stoul(row[5]) : 0;
        messages.push_back(message);
    }

    mysql_free_result(result);
    return messages;
}

int MessageContentDAO::getMessageContentCount(const std::string &senderSsid)  {
    std::string sql = "SELECT COUNT(*) FROM message_content WHERE sender_ssid = ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramSender;
    paramSender.type = MysqlConn::Param::STRING;
    paramSender.str_val = senderSsid;
    params.push_back(paramSender);

    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG_ERROR("Failed to get message count for sender: " << senderSsid);
        return 0;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    int count = 0;
    if (row && row[0]) {
        count = std::stoi(row[0]);
    }

    mysql_free_result(result);
    return count;
}

bool MessageRecipientDAO::insert(const MessageRecipientDO &recipient) {
    std::string sql = "INSERT INTO message_recipient (message_id, recipient_type, recipient_ssid, read_status) "
                      "VALUES (?, ?, ?, ?)";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramMessageId;
    paramMessageId.type = MysqlConn::Param::INT;
    paramMessageId.int_val = recipient.messageId;
    params.push_back(paramMessageId);

    MysqlConn::Param paramType;
    paramType.type = MysqlConn::Param::INT;
    paramType.int_val = recipient.recipientType;
    params.push_back(paramType);

    MysqlConn::Param paramSsid;
    paramSsid.type = MysqlConn::Param::STRING;
    paramSsid.str_val = recipient.recipientSsid;
    params.push_back(paramSsid);

    MysqlConn::Param paramReadStatus;
    paramReadStatus.type = MysqlConn::Param::INT;
    paramReadStatus.int_val = recipient.readStatus;
    params.push_back(paramReadStatus);

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to insert message recipient for message ID: " << recipient.messageId <<
                  ", recipient Ssid: " << recipient.recipientSsid);
        return false;
    }

    return true;
}

bool MessageRecipientDAO::markAsRead(int64_t messageId, const std::string &recipientSsid) {
    std::string sql = "UPDATE message_recipient SET read_status = 1 WHERE message_id = ? AND recipient_ssid = ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramMessageId;
    paramMessageId.type = MysqlConn::Param::INT;
    paramMessageId.int_val = messageId;
    params.push_back(paramMessageId);

    MysqlConn::Param paramSsid;
    paramSsid.type = MysqlConn::Param::STRING;
    paramSsid.str_val = recipientSsid;
    params.push_back(paramSsid);

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to mark message as read for message ID: " << messageId <<
                  ", recipient Ssid: " << recipientSsid);
        return false;
    }

    return true;
}

MessageRecipientDO MessageRecipientDAO::getRecipientData(int64_t messageID) {
    std::string sql = "SELECT id, message_id, recipient_type, recipient_ssid, read_status "
                      "FROM message_recipient WHERE message_id = ? ";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramSsid;
    paramSsid.type = MysqlConn::Param::INT;
    paramSsid.int_val = messageID;
    params.push_back(paramSsid);

    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG_ERROR("Failed to get messages recipient for messageID: " << messageID);
        return {};
    }

    MessageRecipientDO recipient;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        recipient.id = std::stoll(row[0]);
        recipient.messageId = std::stoll(row[1]);
        recipient.recipientType = static_cast<uint8_t>(std::stoul(row[2]));
        recipient.recipientSsid = row[3];
        recipient.readStatus = static_cast<bool>(std::stoul(row[4]));
    }

    mysql_free_result(result);
    return recipient;
}

int MessageRecipientDAO::getRecipientCount(const std::string &recipientSsid)  {
    std::string sql = "SELECT COUNT(*) FROM message_recipient WHERE recipient_ssid = ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramSsid;
    paramSsid.type = MysqlConn::Param::STRING;
    paramSsid.str_val = recipientSsid;
    params.push_back(paramSsid);

    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG_ERROR("Failed to get message count for recipient: " << recipientSsid);
        return 0;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    int count = 0;
    if (row && row[0]) {
        count = std::stoi(row[0]);
    }

    mysql_free_result(result);
    return count;
}