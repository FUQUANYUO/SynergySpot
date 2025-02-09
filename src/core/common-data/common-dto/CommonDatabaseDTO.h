//
// Created by FUQAQ on 2025/1/27.
//

#ifndef COMMONDATABASEDTO_H
#define COMMONDATABASEDTO_H

#include <QDate>
#include "../common-do/CommonDatabaseDO.h"

struct UserBaseInfoDTO{
    QString     ssid;
    QString     username;
    QString     avatarPath;
    QString     sex;
    QString     personalSign;
    QDateTime   birthDate;
    quint32     thumbUpCount;
    quint8      region{};
    QDateTime   createTime;
    UserBaseInfoDTO() = default;
    UserBaseInfoDTO(const UserBaseInfoDTO& other) = default;
};

struct FriendshipDTO{
    QString ssid;
    QString groupingName;
    QString friendSSID;
    qint32 shipStatus;
    qint32 friendType; // 1 用户 2 群组
    QDateTime createTime;
};

struct GroupBaseInfoDTO{
    QString   ssidGroup;
    QString   groupName;
    QString   avatarPath;
    QString   createSSID;
    QString   profile;
    QDateTime createTime;
};

struct GroupMemberInfoDTO{
    QString ssidGroup;
    QString ssidMember;
    QDateTime createTime;
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
    QString fileId;
    MessageRecipientDTO recipient;
    QDateTime createTime;
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
    QDateTime createdAt;
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

#endif //COMMONDATABASEDTO_H
