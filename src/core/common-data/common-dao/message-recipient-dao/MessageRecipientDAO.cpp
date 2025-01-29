//
// Created by FUQAQ on 2025/1/26.
//

#include "MessageRecipientDAO.h"

MessageRecipientDAO::MessageRecipientDAO(LiteConn &db) : _db(db){}

MessageRecipientDAO::~MessageRecipientDAO() {}

bool MessageRecipientDAO::addRecipient(const MessageRecipientDO &recipient) {
    std::string sql = "INSERT INTO message_recipient (messageId, recipientType, recipientSsid, readStatus) "
                          "VALUES (?, ?, ?, ?);";
    std::vector<std::string> params = {
        std::to_string(recipient.messageId),
        std::to_string(recipient.recipientType),
        recipient.recipientSsid.toStdString(),
        recipient.readStatus ? "1" : "0" // 布尔值转换为 "1" 或 "0"
    };

    return _db.update(sql, params);
}

bool MessageRecipientDAO::markAsRead(qint64 recipientId) {
    std::string sql = "UPDATE message_recipient SET readStatus = 1 WHERE id = ?;";
    std::vector<std::string> params = { std::to_string(recipientId) };

    return _db.update(sql, params);
}

QList<MessageRecipientDO> MessageRecipientDAO::listUnread(const QString &userSSID) {
    std::string sql = "SELECT id, messageId, recipientType, recipientSsid, readStatus "
                     "FROM message_recipient WHERE recipientSsid = ? AND readStatus = 0;";
    std::vector<std::string> params = { userSSID.toStdString() };

    auto result = _db.query(sql, params);
    QList<MessageRecipientDO> unreadRecipients;
    for (const auto &row : result) {
        MessageRecipientDO recipient;
        recipient.id = std::stoll(row[0]);
        recipient.messageId = std::stoll(row[1]);
        recipient.recipientType = std::stoi(row[2]);
        recipient.recipientSsid = QString::fromStdString(row[3]);
        recipient.readStatus = (row[4] == "1"); // "1" 转换为 true，否则 false
        unreadRecipients.append(recipient);
    }
    return unreadRecipients;
}