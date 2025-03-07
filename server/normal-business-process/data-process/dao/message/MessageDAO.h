//
// Created by FUQAQ on 2025/2/6.
//

#ifndef MESSAGEDAO_H
#define MESSAGEDAO_H

#include "../DataProcessDAO.h"

class MessageContentDAO : public IMessageContentDAO {
public:
    MessageContentDAO() {
        m_conn = ConnectionPool::getConnectPool()->getConnection();
        if (!m_conn) {
            LOG_ERROR("Failed to get MySQL connection from pool.");
        }
    }

    virtual ~MessageContentDAO() = default;

    int64_t insert(const MessageContentDO& message) override;

    std::vector<MessageContentDO> listBySender(const std::string& senderSsid, int pageSize, int pageNum, time_t lastTime) override;

    int getMessageContentCount(const std::string& senderSsid) override;

    bool insertFiles(int64_t messageId, const std::vector<std::string>& fileIds);
private:
    std::shared_ptr<MysqlConn> m_conn;
};

class MessageRecipientDAO : public IMessageRecipientDAO {
public:
    MessageRecipientDAO() {
        m_conn = ConnectionPool::getConnectPool()->getConnection();
        if (!m_conn) {
            LOG_ERROR("Failed to get MySQL connection from pool.");
        }
    }

    virtual ~MessageRecipientDAO() = default;

    bool insert(const MessageRecipientDO& recipient) override;

    bool markAsRead(int64_t messageId, const std::string& recipientSsid) override;

    MessageRecipientDO getRecipientData(int64_t messageID) override;

    int getRecipientCount(const std::string& recipientSsid) override;
private:
    std::shared_ptr<MysqlConn> m_conn;
};

#endif //MESSAGEDAO_H
