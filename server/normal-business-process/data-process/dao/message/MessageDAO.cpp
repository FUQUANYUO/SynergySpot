//
// Created by FUQAQ on 2025/2/6.
//

#include "MessageDAO.h"

int64_t MessageContentDAO::insert(const MessageContentDO &message)  {
    std::string sql = "INSERT INTO message_content (sender_ssid, content_type, content) "
                      "VALUES (?, ?, ?)";
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

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to insert message content for sender: " << message.senderSsid);
        LOG_ERROR(sql + " , " + message.senderSsid + " , " + std::to_string(message.contentType) + " , " + message.content )
        return -1;
    }

    int64_t messageId = static_cast<int64_t>(m_conn->getLastInsertId());

    if (!message.fileIds.empty()) {
        if (!insertFiles(messageId, message.fileIds)) {
            LOG_ERROR("Insert message_file failed");
            return -1;
        }
    }

    return messageId;
}

std::vector<MessageContentDO> MessageContentDAO::listBySender(const std::string& senderSsid, int pageSize, int pageNum, time_t lastTime) {
    std::string sql = R"(
    SELECT
        mc.id,
        mc.sender_ssid,
        mc.content_type,
        mc.content,
        UNIX_TIMESTAMP(mc.create_time) AS create_time_unix,
        mf.file_id,
        mr.recipient_type,
        mr.recipient_ssid,
        mr.read_status
    FROM
        message_content AS mc
    INNER JOIN
        message_recipient AS mr
        ON mc.id = mr.message_id
    LEFT JOIN
        message_file AS mf
        ON mc.id = mf.message_id
    WHERE
        (mc.sender_ssid = ? OR mr.recipient_ssid = ?)
        AND UNIX_TIMESTAMP(mc.create_time) > ?
    ORDER BY
        UNIX_TIMESTAMP(mc.create_time) DESC,
        mf.sequence ASC
    LIMIT ? OFFSET ?;
    )";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramSender;
    paramSender.type = MysqlConn::Param::STRING;
    paramSender.str_val = senderSsid;
    params.push_back(paramSender);

    MysqlConn::Param paramRecipient;
    paramRecipient.type = MysqlConn::Param::STRING;
    paramRecipient.str_val = senderSsid;
    params.push_back(paramRecipient);

    MysqlConn::Param paramLastTime;
    paramLastTime.type = MysqlConn::Param::INT;
    paramLastTime.int_val = lastTime;
    params.push_back(paramLastTime);

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
    std::unordered_map<std::string, MessageContentDO> messageMap;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        int64_t messageId = std::stoll(row[0]);
        std::string ssid = row[1];
        if (messageMap.find(ssid) == messageMap.end()) {
            MessageContentDO msg;
            msg.id = messageId;
            msg.senderSsid = row[1];
            msg.contentType = static_cast<uint8_t>(std::stoul(row[2]));
            msg.content = row[3];
            msg.createTime = row[4] ? std::stoll(row[4]) : 0;
            msg.recipient.recipientType = std::stoi(row[6]);
            msg.recipient.recipientSsid = row[7];
            msg.recipient.readStatus = row[8];
            messageMap[ssid] = msg;
        }

        if (row[5] != nullptr) {
            messageMap[ssid].fileIds.emplace_back(row[5]);
        }
    }

    messages.reserve(messageMap.size());
    for (auto& pair : messageMap) {
        messages.push_back(pair.second);
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

bool MessageContentDAO::insertFiles(int64_t messageId, const std::vector<std::string> &fileIds) {
    std::string sql = "INSERT INTO message_file (message_id, file_id, sequence) VALUES ";
    std::vector<MysqlConn::Param> params;

    for (size_t i = 0; i < fileIds.size(); ++i) {
        sql += "(?, ?, ?)";
        if (i != fileIds.size() - 1) sql += ", ";
    }

    // 绑定参数
    for (size_t i = 0; i < fileIds.size(); ++i) {
        // message_id
        MysqlConn::Param paramMessageId;
        paramMessageId.type = MysqlConn::Param::INT;
        paramMessageId.int_val = messageId;
        params.push_back(paramMessageId);

        // file_id
        MysqlConn::Param paramFileId;
        paramFileId.type = MysqlConn::Param::STRING;
        paramFileId.str_val = fileIds[i];
        params.push_back(paramFileId);

        // sequence（按列表顺序从0开始）
        MysqlConn::Param paramSequence;
        paramSequence.type = MysqlConn::Param::INT;
        paramSequence.int_val = static_cast<int>(i);
        params.push_back(paramSequence);
    }

    return m_conn->update(sql, params);
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