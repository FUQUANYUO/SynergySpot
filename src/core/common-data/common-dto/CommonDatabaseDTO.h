//
// Created by FUQAQ on 2025/1/27.
//

#ifndef COMMONDATABASEDTO_H
#define COMMONDATABASEDTO_H

#include <QDate>
#include "../common-do/CommonDatabaseDO.h"

struct UserBaseInfoDTO{
    QString     ssid = "";
    QString     username = "";
    QString     avatarPath = "";
    QString     sex = "";
    QString     personalSign = "";
    time_t      birthDate = 0;
    quint32     thumbUpCount = static_cast<quint32>(-1);
    quint8      region =  static_cast<quint8>(-1);
    time_t      createTime = 0;
    UserBaseInfoDTO() = default;
    UserBaseInfoDTO(const UserBaseInfoDTO& other) = default;
};

struct FriendshipDTO{
    QString ssid;
    QString groupingName;
    QString friendSSID;
    qint32 shipStatus;
    qint32 friendType; // 1 用户 2 群组
    time_t createTime;
};

struct GroupBaseInfoDTO{
    QString        ssidGroup;
    QString        groupName;
    QString        avatarPath;
    QString        createSSID;
    QString        profile;
    QList<QString> admins;
    time_t         createTime;
};

struct GroupMemberInfoDTO{
    QString ssidGroup;
    QString ssidMember;
    time_t createTime;
};

struct MessageRecipientDTO{
    qint32 recipientType;    // 1-用户 2-群组
    QString recipientSSID;
    bool readStatus = false; // DEFAULT 0
};

struct MessageContentDTO{
    QString senderSSID;
    ContentType contentType;
    QString content;
    QList<QString> fileId;
    MessageRecipientDTO recipient;
    time_t createTime;
};
Q_DECLARE_METATYPE(MessageContentDO)

struct BaseStickerDTO {
    qint64 stickerId;
    QString imageUrl;
};

struct CollectedStickerDTO {
    QString userSSID;
    QString imageUrl;
    bool isCustom = false;
    time_t createdAt;
};

struct MsgCombineDTO {
    UserBaseInfoDTO              userBaseInfo;
    GroupBaseInfoDTO             groupBaseInfo;
    QList<GroupMemberInfoDTO>    groupMemberInfo;
    QString                      content;
    qint64                       timestamp = 0;
    bool isGroup = false;
};
Q_DECLARE_METATYPE(MsgCombineDTO)

struct LoginRecordDTO {
    QString account;         // 登录账号
    QString plainPassword;   // 明文密码
    time_t loginTime;     // 登录时间
    QString deviceInfo;      // 设备信息
};

struct FileStorageDTO {
    QString fileId;
    QString uploaderSsid;
    QString fileName;
    int64_t fileSize;
    QString fileType;
    QString storagePath;
    time_t uploadTime;

    int pageSize;
    int pageNum;
};



#endif //COMMONDATABASEDTO_H
