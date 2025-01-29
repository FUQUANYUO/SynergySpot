//
// Created by FUQAQ on 2025/1/27.
//

#ifndef COMMONDATABASEDTO_H
#define COMMONDATABASEDTO_H

#include <QDate>

struct UserBaseInfoDTO{
    QString username;
    QString avatarPath;
    QString sex;
    QString personalSign;
    QDate   birthDate;
    quint8  region;
    QDateTime createTime;
};

struct FriendshipDTO{
    QString ssid;
    QString friendSSID;
    qint32 shipStatus;
    qint32 friendType;
    QDateTime createTime;
};

struct GroupBaseInfoDTO{
    QString groupName;
    QString createSSID;
    QString profile;
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
    QString contentType;
    QString content;
    QString fileId;
    MessageRecipientDTO recipient;
    QDateTime createTime;
};

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

#endif //COMMONDATABASEDTO_H
