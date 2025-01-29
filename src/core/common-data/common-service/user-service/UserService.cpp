//
// Created by FUQAQ on 2025/1/27.
//

#include "UserService.h"


UserService::UserService(LiteConn& db): userDAO(db) {}

UserBaseInfoDTO UserService::getUserBySSID(const QString& ssid) {
    UserBaseInfoDO result = userDAO.findById(ssid);
    if (result.createTime.isValid()) {
        return {
            result.ssname,result.ssid,
            result.sex==UserSex::Female?"女生":"男生",
            result.personalSign,
            result.birthday,
            result.region,
            result.createTime
        };
    }
    else {
        return {};
    }
}

bool UserService::updateUserBySSID(const QString &ssid, const UserBaseInfoDTO &dto) {
    UserBaseInfoDO tmpDO = userDAO.findById(ssid);
    if (tmpDO.createTime.isValid()) {
        tmpDO.ssname = (dto.username == "" ?tmpDO.ssname : dto.username);
        tmpDO.avatar = (dto.avatarPath == ""?tmpDO.avatar: dto.avatarPath);
        tmpDO.personalSign = (dto.personalSign == ""?tmpDO.personalSign: dto.personalSign);
        tmpDO.birthday = (dto.birthDate.isNull()?tmpDO.birthday: dto.birthDate);
        tmpDO.region = (dto.region==-1?tmpDO.region: dto.region);
        tmpDO.sex = (dto.sex==""?tmpDO.sex:(dto.sex==u8"女生"?UserSex::Female:UserSex::Male));
        return userDAO.update(tmpDO);
    }else {
        LOG_ERROR("UserService::updateUserBySSID failed : cant find user by ssid [" + ssid.toStdString() + "]")
    }
    return false;
}

bool UserService::updateThumbUpCount(const QString &ssid, int newCount) {
    return userDAO.updateThumbUpCount(ssid, newCount);
}

bool UserService::updateAvatarPath(const QString &ssid, const QString &newUrl) {
    return userDAO.updateAvatar(ssid, newUrl);
}