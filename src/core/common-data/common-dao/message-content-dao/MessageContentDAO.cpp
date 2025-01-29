//
// Created by FUQAQ on 2025/1/26.
//

#include "MessageContentDAO.h"

MessageContentDAO::MessageContentDAO(LiteConn &db) : _db(db){}

MessageContentDAO::~MessageContentDAO() {}

qint64 MessageContentDAO::insertMessage(const MessageContentDO &message) {
    std::string sql = "INSERT INTO message_content (senderSSID, contentType, content, fileId, createTime) "
                  "VALUES (?, ?, ?, ?, ?);";
    std::vector<std::string> params = {
        message.senderSSID.toStdString(),
        std::to_string(static_cast<int>(message.contentType)), // 枚举转换为 int
        message.content.toStdString(),
        message.fileId.toStdString(),
        message.createTime.toString("yyyy-MM-dd HH:mm:ss").toStdString()
    };

    if (_db.update(sql, params)) {
        // 获取最后插入的ID
        std::string lastIdSql = "SELECT last_insert_rowid();";
        auto result = _db.query(lastIdSql, {});
        if (!result.empty() && !result[0].empty()) {
            return std::stoll(result[0][0]);
        }
    }
    return -1;
}

MessageContentDO MessageContentDAO::findMessageById(qint64 id) {
    std::string sql = "SELECT id, senderSSID, contentType, content, fileId, createTime "
                      "FROM message_content WHERE id = ?;";
    std::vector<std::string> params = { std::to_string(id) };

    auto result = _db.query(sql, params);
    if (!result.empty()) {
        MessageContentDO message;
        message.id = std::stoll(result[0][0]);
        message.senderSSID = QString::fromStdString(result[0][1]);
        message.contentType = static_cast<ContentType>(std::stoi(result[0][2])); // int 转换为枚举
        message.content = QString::fromStdString(result[0][3]);
        message.fileId = QString::fromStdString(result[0][4]);
        message.createTime = QDateTime::fromString(QString::fromStdString(result[0][5]), "yyyy-MM-dd HH:mm:ss");
        return message;
    }
    return MessageContentDO();
}

QList<MessageContentDO> MessageContentDAO::listMessagesBySender(const QString &senderSsid) {
    std::string sql = "SELECT id, senderSSID, contentType, content, fileId, createTime "
                  "FROM message_content WHERE senderSSID = ?;";
    std::vector<std::string> params = { senderSsid.toStdString() };

    auto result = _db.query(sql, params);
    QList<MessageContentDO> messages;
    for (const auto &row : result) {
        MessageContentDO message;
        message.id = std::stoll(row[0]);
        message.senderSSID = QString::fromStdString(row[1]);
        message.contentType = static_cast<ContentType>(std::stoi(row[2])); // int 转换为枚举
        message.content = QString::fromStdString(row[3]);
        message.fileId = QString::fromStdString(row[4]);
        message.createTime = QDateTime::fromString(QString::fromStdString(row[5]), "yyyy-MM-dd HH:mm:ss");
        messages.append(message);
    }
    return messages;
}