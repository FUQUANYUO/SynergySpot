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
        message.fileId.toStdString(),
        message.createTime.toString(Qt::ISODate).toStdString()
    };

    // 执行插入
    bool success = _db.update(
        "INSERT INTO message_content "
        "(sender_ssid, content_type, content, file_id, create_time) "
        "VALUES (?, ?, ?, ?, ?)",
        params
    );

    if (!success) return -1;

    // 获取最后插入ID
    auto result = _db.query("SELECT last_insert_rowid()", {});
    if (result.empty() || result[0].empty()) return -1;
    return std::stoll(result[0][0]);
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
        std::to_string(pageSize),
        std::to_string(offset)
    };

    // 分页查询SQL
    auto queryResult = _db.query(
        "SELECT mc.sender_ssid, mc.content_type, mc.content, mc.file_id, mc.create_time, "
        "mr.recipient_type, mr.recipient_ssid, mr.read_status "
        "FROM message_content mc "
        "JOIN message_recipient mr ON mc.id = mr.message_id "
        "WHERE mc.sender_ssid = ? "
        "ORDER BY mc.create_time ASC "
        "LIMIT ? OFFSET ?",  // 分页控制
        params
    );

    // 转换结果集
    for (const auto& row : queryResult) {
        if (row.size() < 8) continue;

        MessageContentDTO dto;
        // 消息内容部分
        dto.senderSSID = QString::fromStdString(row[0]);
        dto.content = QString::fromStdString(row[2]);
        dto.fileId = QString::fromStdString(row[3]);
        dto.createTime = QDateTime::fromString(
            QString::fromStdString(row[4]), Qt::ISODate);

        // 内容类型转换
        dto.contentType = static_cast<ContentType>(QString::fromStdString(row[1]).toInt());

        // 接收方信息
        MessageRecipientDTO recip;
        recip.recipientType = QString::fromStdString(row[5]).toInt();
        recip.recipientSSID = QString::fromStdString(row[6]);
        recip.readStatus = QString::fromStdString(row[7]) == "1";
        dto.recipient = recip;

        result.append(QVariant::fromValue(dto));
    }
    return result;

}

int MessageContentDAO::getMessageCount(const QString &ssid) {
    std::string sql = "SELECT COUNT(*) FROM message_content WHERE sender_ssid = ?";
    std::vector<std::string> params = { ssid.toStdString() };

    auto result = _db.query(sql, params);

    if (!result.empty() && !result[0].empty()) {
        return std::stoi(result[0][0]);
    }else {
        return 0;
    }
}