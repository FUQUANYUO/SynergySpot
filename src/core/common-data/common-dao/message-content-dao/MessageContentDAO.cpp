//
// Created by FUQAQ on 2025/1/26.
//

#include "MessageContentDAO.h"
#include "../../common-dto/CommonDatabaseDTO.h"

MessageContentDAO::MessageContentDAO(LiteConn &db) : _db(db){}

MessageContentDAO::~MessageContentDAO() {}

qint64 MessageContentDAO::insertMessageContent(const MessageContentDO &message) {
    // 构建参数列表
    std::vector<std::string> params{
        message.senderSSID.toStdString(),
        std::to_string(static_cast<int>(message.contentType)),
        message.content.toStdString(),
        std::to_string(message.createTime)
    };

    // 执行插入
    bool success = _db.update(
        "INSERT INTO message_content "
        "(sender_ssid, content_type, content, create_time) "
        "VALUES (?, ?, ?, ?)",
        params
    );

    if (!success) return -1;

    auto result = _db.query("SELECT last_insert_rowid()", {});
    if (result.empty() || result[0].empty()) return -1;

    int msgID = std::stoll(result[0][0]);

    if(!message.fileId.empty()) {
        const char* fileSql = "INSERT INTO message_file (message_id, file_id, sequence) VALUES (?, ?, ?)";
        for(size_t i = 0; i < message.fileId.size(); ++i) {
            params.clear();

            params.emplace_back(result[0][0]);
            params.emplace_back(message.fileId[i].toStdString());
            params.emplace_back(std::to_string(i));
        }

        success = _db.update(fileSql,params);
        if (!success) return -1;
    }
    return msgID;
}

bool MessageContentDAO::insertMessageRecipient(const MessageRecipientDO &recipient) {
    std::vector<std::string> params{
        std::to_string(recipient.messageId),
        std::to_string(recipient.recipientType),
        recipient.recipientSsid.toStdString(),
        std::to_string(recipient.readStatus ? 1 : 0)
    };

    return _db.update(
        "INSERT INTO message_recipient "
        "(message_id, recipient_type, recipient_ssid, read_status) "
        "VALUES (?, ?, ?, ?)",
        params
    );
}

bool MessageContentDAO::insertMessageTransaction(QList<QVariant> messages) {
    _db.beginTransaction();

    for (auto it : messages) {
        auto dto = it.value<MessageContentDTO>();
        const qint64 msgId = insertMessageContent({-1, dto.senderSSID, dto.contentType, dto.content, dto.fileId, dto.createTime});
        if (msgId == -1) {
            _db.rollback();
            return false;
        }

        MessageRecipientDO modifiedRecipient = {-1,msgId,dto.recipient.recipientType,dto.recipient.recipientSSID,dto.recipient.readStatus};
        if (!insertMessageRecipient(modifiedRecipient)) {
            _db.rollback();
            return false;
        }
    }
    _db.commit();
    return true;
}

bool MessageContentDAO::markMessageAsRead(qint64 messageId, const QString &recipientSSID) {
    std::vector<std::string> params{
        std::to_string(messageId),
        recipientSSID.toStdString()
    };

    return _db.update(
        "UPDATE message_recipient "
        "SET read_status = 1 "
        "WHERE message_id = ? AND recipient_ssid = ?",
        params
    );
}

QList<QVariant> MessageContentDAO::listMessagesByRecipient(const QString& recipientSSID, int pageSize, int pageNum) {
    QList<QVariant> result;
    // 参数校验
    if (pageSize <= 0) pageSize = 20;
    if (pageNum <= 0) pageNum = 1;
    const int offset = (pageNum - 1) * pageSize;

    // 构建参数
    std::vector<std::string> params{
        recipientSSID.toStdString(),
        recipientSSID.toStdString(),    // sender is cur user
        std::to_string(pageSize),
        std::to_string(offset)
    };

    // Messages are ordered by create_time in ascending order to ensure that the earliest messages are displayed at the top of the message page.
    const char* sql = R"(
        SELECT mc.id, mc.sender_ssid, mc.content_type, mc.content, mc.create_time,
               mf.file_id, mf.sequence,
               mr.recipient_type, mr.recipient_ssid, mr.read_status
        FROM message_content mc
        JOIN message_recipient mr ON mc.id = mr.message_id
        LEFT JOIN message_file mf ON mc.id = mf.message_id
        WHERE mr.recipient_ssid = ? OR mc.sender_ssid = ?
        ORDER BY mc.create_time ASC, mf.sequence ASC
        LIMIT ? OFFSET ?
    )";

    // 分页查询SQL
    auto queryResult = _db.query(sql,params);

    // 转换结果集
    std::unordered_map<int64_t, MessageContentDTO> msgMap;
    for (const auto& row : queryResult) {
        int64_t msgId = std::stoll(row[0]);

        if (msgMap.find(msgId) == msgMap.end()) {
            MessageContentDTO msg;
            msg.senderSSID  = QString::fromStdString(row[1]);
            msg.contentType = static_cast<ContentType>(std::stoi(row[2]));
            msg.content     = QString::fromStdString(row[3]);
            msg.createTime  = std::stoll(row[4]);

            MessageRecipientDTO recip;
            recip.recipientType = std::stoi(row[7]);
            recip.recipientSSID = QString::fromStdString(row[8]);
            recip.readStatus = std::stoi(row[9]) == 1;
            msg.recipient = recip;

            msgMap[msgId] = msg;
        }
        if (!row[5].empty()) {
            msgMap[msgId].fileId.push_back(QString::fromStdString(row[5]));
        }
    }
    for(auto& pair : msgMap) {
        result.push_back(QVariant::fromValue(pair.second));
    }
    return result;
}

int MessageContentDAO::getMessageCount(const QString &ssid) {
    std::string sql = "SELECT COUNT(*) FROM message_content WHERE sender_ssid = ?";
    std::vector<std::string> params = {ssid.toStdString()};

    auto result = _db.query(sql, params);

    if (!result.empty() && !result[0].empty()) {
        return std::stoi(result[0][0]);
    } else {
        return 0;
    }
}
time_t MessageContentDAO::getLastMsgTime() {
    // 执行 SQL 查询获取最新消息时间
    auto result = _db.query(
        "SELECT MAX(create_time) FROM message_content",
        {}
    );

    // 处理查询结果
    if (!result.empty() && !result[0].empty()) {
        const std::string& timeStr = result[0][0];

        if (timeStr.empty()) {
            return 0;
        }

        return std::stol(timeStr);
    }

    return 0;
}