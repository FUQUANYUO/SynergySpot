//
// Created by FUQAQ on 2025/2/5.
//

#ifndef DATAPROCESSDO_H
#define DATAPROCESSDO_H

#include <string>
#include <cstdint>
#include <ctime>

// 用户基础信息 DO
struct UserBaseInfoDO {
    std::string ssid;
    std::string ssname;
    std::string avatar;
    char sex;
    std::string personalSign;
    uint32_t thumbUpCount;
    time_t birthday;
    uint8_t region;
    time_t createTime;
};

// 用户私有信息 DO
struct UserPrivateInfoDO {
    std::string ssid;
    std::string email;
    std::string password;
    std::string passwordSalt;
    uint8_t accountStatus;
};

// 好友关系 DO
struct FriendshipDO {
    int64_t id;
    std::string ssid;
    std::string grouping;
    std::string remark;
    std::string friendSsid;
    uint8_t shipStatus;
    uint8_t friendType;
    time_t createTime;
};

// 群组基础信息 DO
struct GroupBaseInfoDO {
    int64_t id;
    std::string ssidGroup;
    std::string name;
    std::string avatar;
    std::string createSsid;
    std::string profile;
    time_t createTime;
};

// 群管理员 DO
struct GroupAdminDO {
    int64_t id;
    int64_t groupId;
    std::string opSsid;
};

// 群公告 DO
struct GroupNoticeDO {
    int64_t id;
    int64_t groupId;
    std::string notice;
    time_t createTime;
};

// 群成员信息 DO
struct GroupMemberInfoDO {
    int64_t id;
    std::string ssidGroup;
    std::string ssidMember;
    time_t createTime;
};

// 消息内容 DO
struct MessageContentDO {
    int64_t id;
    std::string senderSsid;
    uint8_t contentType;
    std::string content;
    std::string fileId;
    time_t createTime;
};

// 消息接收者 DO
struct MessageRecipientDO {
    int64_t id;
    int64_t messageId;
    uint8_t recipientType;
    std::string recipientSsid;
    bool readStatus;
};

// 地区 DO
struct DistrictDO {
    uint16_t districtId;
    uint16_t pid;
    std::string district;
    uint8_t level;
};

// 文件存储 DO
struct FileStorageDO {
    std::string fileId;
    std::string uploaderSsid;
    std::string fileName;
    int64_t fileSize;
    std::string fileType;
    std::string storagePath;
    time_t uploadTime;
};

// 操作日志 DO
struct OperationLogDO {
    int64_t id;
    std::string ssid;
    std::string operationType;
    std::string detail;
    std::string ipAddress;
    time_t createTime;
};

// 基础表情 DO
struct BaseStickerDO {
    int64_t stickerId;
    std::string imageUrl;
    std::string tags;
    time_t createdAt;
};

// 用户收藏表情 DO
struct UserCollectedStickerDO {
    int64_t collectionId;
    std::string userSsid;
    bool isCustom;
    std::string imageUrl;
    time_t createdAt;
};

#endif //DATAPROCESSDO_H
