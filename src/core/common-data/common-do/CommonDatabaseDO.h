//
// Created by FUQAQ on 2025/1/26.
//

#ifndef COMMONDATABASEDO_H
#define COMMONDATABASEDO_H

#include <QList>

// 性别枚举（与数据库CHECK约束对应）
enum class UserSex {
    Male,   // 对应数据库'M'
    Female  // 对应数据库'F'
};

// 消息内容类型枚举
enum class ContentType {
    Text,
    Image,
    File,
    Sticker
};

// ================== DO 定义 ================== //
// 命名规范：表名 + "DO"后缀，字段与数据库完全一致

// 用户基础信息（对应user_base_info表）
struct UserBaseInfoDO {
    QString ssid;            // PRIMARY KEY
    QString ssname;
    QString avatar;
    UserSex sex;             // 存储时需要转换为'M'/'F'
    QString personalSign;
    quint32 thumbUpCount = 0;
    time_t birthday;
    time_t createTime;
    quint8 region = 2;       // DEFAULT 2
};

// 好友关系（对应friendship表）
struct FriendshipDO {
    qint64 id;               // AUTOINCREMENT
    QString ssid;
    QString grouping;
    QString friendSSID;
    qint32 shipStatus;       // 状态值（可配合枚举使用）
    qint32 friendType = 1;   // DEFAULT 1 用户 2 群组
    time_t createTime;
    bool isEmpty() {
        if ((ssid.isEmpty() && grouping.isEmpty() && friendSSID.isEmpty()) || id == -1) {
            return true;
        }else {
            return false;
        }
    }
};

// 群组基础信息（对应group_base_info表）
struct GroupBaseInfoDO {
    qint64 id;               // AUTOINCREMENT
    QString ssidGroup;       // UNIQUE
    QString name;
    QString avatar;
    QString createSSID;
    QString profile;
    time_t createTime;
};

// 群成员信息（对应group_member_info表）
struct GroupMemberInfoDO {
    qint64 id;               // AUTOINCREMENT
    QString ssidGroup;
    QString ssidMember;
    time_t createTime;
};

// 消息内容（对应message_content表）
struct MessageContentDO {
    qint64 id;               // AUTOINCREMENT
    QString senderSSID;
    ContentType contentType;
    QString content;
    QList<QString> fileId;
    time_t createTime;
};

// 消息接收方（对应message_recipient表）
struct MessageRecipientDO {
    qint64 id;               // AUTOINCREMENT
    qint64 messageId;
    qint32 recipientType;    // 1-用户 2-群组
    QString recipientSsid;
    bool readStatus = false; // DEFAULT 0
};

// 基础表情（对应base_stickers表）
struct BaseStickerDO {
    qint64 stickerId;        // AUTOINCREMENT
    QString imageUrl;        // UNIQUE
    time_t createdAt;
};

// 用户收藏表情（对应user_collected_stickers表）
struct CollectedStickerDO {
    QString userSSID;        // PRIMARY KEY (与imageUrl组合)
    QString imageUrl;
    bool isCustom = false;   // DEFAULT 0
    time_t createdAt;
};

struct LoginRecordDO {
    qint64 id;                 // AUTOINCREMENT
    QString account;           // 登录账号
    QString encryptedPassword; // 加密后的密码
    time_t loginTime;       // 登录时间
    QString deviceInfo;        // 设备信息（可选）
};

struct FileStorageDO {
    QString fileId;
    QString uploaderSsid;
    QString fileName;
    int64_t fileSize;
    QString fileType;
    QString storagePath;
    time_t uploadTime;
};

#endif //COMMONDATABASEDO_H
