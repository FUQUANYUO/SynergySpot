//
// Created by FUQAQ on 2025/1/26.
//

#ifndef COMMONDATABASEDAO_H
#define COMMONDATABASEDAO_H

#include "db-lite/LiteConn.h"
#include "../common-do/CommonDataBaseDO.h"

// ================== DAO 接口 ==================
// 命名规范：表名 + "DAO"后缀，仅声明原子操作

class IUserDAO {
public:
    virtual ~IUserDAO() = default;

    // 核心方法
    virtual bool insert(const UserBaseInfoDO& user) = 0;
    virtual bool update(const UserBaseInfoDO& user) = 0;
    virtual bool deleteById(const QString& ssid) = 0;
    virtual UserBaseInfoDO findById(const QString& ssid) = 0;

    // 扩展方法
    virtual QList<UserBaseInfoDO> findByRegion(quint8 region) = 0;
    virtual bool updateThumbUpCount(const QString& ssid, int newCount) = 0;
};

class IFriendshipDAO {
public:
    virtual ~IFriendshipDAO() = default;

    virtual qint64 create(const FriendshipDO& friendship) = 0;
    virtual bool updateStatus(qint64 id, qint32 newStatus) = 0;
    virtual FriendshipDO findRelationship(const QString& ssid, const QString& friendSsid) = 0;
    virtual QList<FriendshipDO> listByUser(const QString& ssid) = 0;
};

class IGroupInfoDAO {
public:
    virtual ~IGroupInfoDAO() = default;

    virtual qint64 createGroup(const GroupBaseInfoDO& group) = 0;
    virtual bool updateProfile(const QString& groupSsid, const QString& newProfile) = 0;
    virtual GroupBaseInfoDO findBySsid(const QString& groupSsid) = 0;
};

class IGroupMemberDAO {
public:
    virtual ~IGroupMemberDAO() = default;

    virtual bool addMember(const GroupMemberInfoDO& member) = 0;
    virtual bool removeMember(const QString& groupSsid, const QString& memberSsid) = 0;
    virtual QList<GroupMemberInfoDO> listMembers(const QString& groupSsid) = 0;
};

class IMessageContentDAO {
public:
    virtual ~IMessageContentDAO() = default;

    virtual qint64 insertMessage(const MessageContentDO& message) = 0;
    virtual MessageContentDO findMessageById(qint64 id) = 0;
    virtual QList<MessageContentDO> listMessagesBySender(const QString& senderSsid) = 0;
};

class IMessageRecipientDAO {
public:
    virtual ~IMessageRecipientDAO() = default;

    virtual bool addRecipient(const MessageRecipientDO& recipient) = 0;
    virtual bool markAsRead(qint64 recipientId) = 0;
    virtual QList<MessageRecipientDO> listUnread(const QString& userSsid) = 0;
};

class IStickerDAO {
public:
    virtual ~IStickerDAO() = default;

    // 基础表情操作
    virtual qint64 insertBaseSticker(const BaseStickerDO& sticker) = 0;
    virtual bool deleteBaseSticker(qint64 stickerId) = 0;
    virtual QList<BaseStickerDO> listBaseStickers() = 0;

    // 用户收藏操作
    virtual bool insertCollectSticker(const CollectedStickerDO& sticker) = 0;
    virtual bool removeCollectedSticker(const QString& userSsid, const QString& imageUrl) = 0;
    virtual QList<CollectedStickerDO> listCollectedStickers(const QString& userSsid) = 0;
};

#endif //COMMONDATABASEDAO_H
