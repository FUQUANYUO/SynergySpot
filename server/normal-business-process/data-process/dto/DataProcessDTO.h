//
// Created by FUQAQ on 2025/2/5.
//

#ifndef DATAPROCESSDTO_H
#define DATAPROCESSDTO_H

#include <string>
#include <vector>
#include <cstdint>
#include <ctime>

// 用户基础信息 DTO
struct UserBaseInfoDTO {
    std::string ssid;
    std::string ssname;
    std::string avatarPath;
    char sex; // 'M' 或 'F'
    std::string personalSign;
    uint32_t thumbUpCount;
    time_t birthday;
    uint8_t region;
    time_t createTime;
};

// 用户私有信息 DTO
struct UserPrivateInfoDTO {
    std::string ssid;
    std::string email;
    std::string password;
    std::string passwordSalt;
    uint8_t accountStatus;
};

// 好友关系 DTO
struct FriendshipDTO {
    int64_t id;
    std::string ssid;
    std::string grouping;
    std::string remark;
    std::string friendSsid;
    uint8_t shipStatus;
    uint8_t friendType;
    time_t createTime;
};

// 群管理员 DTO
struct GroupAdminDTO {
    int64_t id;
    int64_t groupId;
    std::string opSsid;
};

// 群组基础信息 DTO
struct GroupBaseInfoDTO {
    int64_t id;
    std::string ssidGroup;
    std::string name;
    std::string avatar;
    std::string createSsid;
    std::string profile;
    std::vector<std::string> admins;
    time_t createTime;
};

// 群公告 DTO
struct GroupNoticeDTO {
    int64_t id;
    int64_t groupId;
    std::string notice;
    time_t createTime;
};

// 群成员信息 DTO
struct GroupMemberInfoDTO {
    int64_t id;
    std::string ssidGroup;
    std::string ssidMember;
    time_t createTime;
};

// 消息接收者 DTO
struct MessageRecipientDTO {
    int64_t id;
    int64_t messageId;
    uint8_t recipientType;
    std::string recipientSsid;
    bool readStatus;
};

// 消息内容 DTO
struct MessageContentDTO {
    int64_t id;
    std::string senderSsid;
    uint8_t contentType;
    std::string content;
    std::string fileId;
    MessageRecipientDTO recipient;
    time_t createTime;

    int pageSize;
    int pageNum;
};

// 地区 DTO
struct DistrictDTO {
    uint16_t districtId;
    uint16_t pid;
    std::string district;
    uint8_t level;
};

// 文件存储 DTO
struct FileStorageDTO {
    std::string fileId;
    std::string uploaderSsid;
    std::string fileName;
    int64_t fileSize;
    std::string fileType;
    std::string storagePath;
    time_t uploadTime;

    int pageSize;
    int pageNum;
};

// 操作日志 DTO
struct OperationLogDTO {
    int64_t id;
    std::string ssid;
    std::string operationType;
    std::string detail;
    std::string ipAddress;
    time_t createTime;
};

// 基础表情 DTO
struct BaseStickerDTO {
    int64_t stickerId;
    std::string imageUrl;
    std::string tags;
    time_t createdAt;
};

// 用户收藏表情 DTO
struct UserCollectedStickerDTO {
    int64_t collectionId;
    std::string userSsid;
    bool isCustom;
    std::string imageUrl;
    time_t createdAt;

    int pageSize;
    int pageNum;
};

#endif //DATAPROCESSDTO_H
