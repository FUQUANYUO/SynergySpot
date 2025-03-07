//
// Created by FUQAQ on 2025/2/5.
//

#ifndef DATAPROCESSDAO_H
#define DATAPROCESSDAO_H


#include <string>
#include <vector>
#include "../do/DataProcessDO.h"
#include "db-pool/ConnectionPool.h"

// 用户 DAO 接口
class IUserDAO {
public:
    virtual ~IUserDAO() = default;

    virtual bool insert(const UserBaseInfoDO& user) = 0;
    virtual bool update(const UserBaseInfoDO& user) = 0;
    virtual bool deleteById(const std::string& ssid) = 0;
    virtual UserBaseInfoDO findById(const std::string& ssid) = 0;
    virtual bool updateThumbUpCount(const std::string& ssid, int newCount) = 0;

    virtual std::vector<UserBaseInfoDO> findByRegion(uint8_t region,int pageSize, int pageNum) = 0;
    virtual int getRegionCount(uint8_t region) = 0;
};

// 用户私有信息 DAO 接口
class IUserPrivateDAO {
public:
    virtual ~IUserPrivateDAO() = default;

    virtual bool insert(const UserPrivateInfoDO& user) = 0;
    virtual bool update(const UserPrivateInfoDO& user) = 0;
    virtual bool deleteById(const std::string& ssid) = 0;
    virtual UserPrivateInfoDO findById(const std::string& ssid) = 0;
};

// 好友关系 DAO 接口
class IFriendshipDAO {
public:
    virtual ~IFriendshipDAO() = default;

    virtual int64_t create(const FriendshipDO& friendship) = 0;
    virtual bool update(const FriendshipDO& friendship) = 0;
    virtual bool deleteById(const std::string& ssid, const std::string& friendSsid) = 0;
    virtual FriendshipDO findRelationship(const std::string& ssid, const std::string& friendSsid) = 0;

    virtual std::vector<FriendshipDO> listByUser(const std::string& ssid) = 0;
    virtual int getFriendshipCount(const std::string& ssid) = 0;
};

// 群组基础信息 DAO 接口
class IGroupBaseInfoDAO {
public:
    virtual ~IGroupBaseInfoDAO() = default;

    virtual int64_t createGroup(const GroupBaseInfoDO& group) = 0;
    virtual bool deleteById(const std::string& groupSsid) = 0;
    virtual bool update(const GroupBaseInfoDO& group) = 0;
    virtual GroupBaseInfoDO findBySsid(const std::string& groupSsid) = 0;
};

// 群管理员 DAO 接口
class IGroupAdminDAO {
public:
    virtual ~IGroupAdminDAO() = default;

    virtual bool addAdmin(int64_t groupId, const std::string& opSsid) = 0;
    virtual bool removeAdmin(int64_t groupId, const std::string& opSsid) = 0;
    virtual std::vector<GroupAdminDO> getAdmins(int64_t groupId) = 0;
};

// 群公告 DAO 接口
class IGroupNoticeDAO {
public:
    virtual ~IGroupNoticeDAO() = default;

    virtual bool deleteNotice(int64_t groupId,int64_t noticeId) = 0;
    virtual int64_t addNotice(int64_t groupId, const std::string& notice) = 0;
    virtual std::vector<GroupNoticeDO> listNotices(int64_t groupId, int pageSize, int pageNum) = 0;
};

// 群成员信息 DAO 接口
class IGroupMemberDAO {
public:
    virtual ~IGroupMemberDAO() = default;

    virtual bool addMember(const std::string& groupSsid, const std::string& memberSsid) = 0;
    virtual bool removeMember(const std::string& groupSsid, const std::string& memberSsid) = 0;
    virtual std::vector<GroupMemberInfoDO> listMembers(const std::string& groupSsid) = 0;
    virtual int getMemberCount(const std::string& groupSsid) = 0;
};

// 消息内容 DAO 接口
class IMessageContentDAO {
public:
    virtual ~IMessageContentDAO() = default;

    virtual int64_t insert(const MessageContentDO& message) = 0;
    virtual std::vector<MessageContentDO> listBySender(const std::string& senderSsid, int pageSize, int pageNum, time_t lastTime) = 0;
    virtual int getMessageContentCount(const std::string& senderSsid) = 0;
};

// 消息接收者 DAO 接口
class IMessageRecipientDAO {
public:
    virtual ~IMessageRecipientDAO() = default;

    virtual bool insert(const MessageRecipientDO& recipient) = 0;
    virtual bool markAsRead(int64_t messageId, const std::string& recipientSsid) = 0;
    virtual  MessageRecipientDO getRecipientData(int64_t messageID) = 0;
    virtual int getRecipientCount(const std::string& recipientSSID) = 0;
};

// 地区 DAO 接口
class IDistrictDAO {
public:
    virtual ~IDistrictDAO() = default;

    virtual std::string getDistrictName(uint16_t pid) = 0;
    virtual std::vector<DistrictDO> findByLevel(uint8_t level) = 0;
};

// 文件存储 DAO 接口
class IFileStorageDAO {
public:
    virtual ~IFileStorageDAO() = default;

    virtual bool deleteFile(const std::string& fileId) = 0;
    virtual bool upload(const FileStorageDO& file) = 0;
    virtual FileStorageDO findById(const std::string& fileId) = 0;
};

// 操作日志 DAO 接口
class IOperationLogDAO {
public:
    virtual ~IOperationLogDAO() = default;

    virtual bool log(const OperationLogDO& log) = 0;
    virtual std::vector<OperationLogDO> listByUser(const std::string& ssid) = 0;
};

// 基础表情 DAO 接口
class IBaseStickerDAO {
public:
    virtual ~IBaseStickerDAO() = default;

    virtual int64_t insert(const BaseStickerDO& sticker) = 0;
    virtual bool deleteById(int64_t stickerId) = 0;
    virtual std::vector<BaseStickerDO> listBaseStickers() = 0;
};

// 用户收藏表情 DAO 接口
class IUserCollectedStickerDAO {
public:
    virtual ~IUserCollectedStickerDAO() = default;

    virtual bool collect(const UserCollectedStickerDO& sticker) = 0;
    virtual bool unCollect(const std::string& userSsid, const std::string& imageUrl) = 0;
    virtual std::vector<UserCollectedStickerDO> listByUser(const std::string& userSsid,int pageSize, int pageNum) = 0;
};

#endif //DATAPROCESSDAO_H
