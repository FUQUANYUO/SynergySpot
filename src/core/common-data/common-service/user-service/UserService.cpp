//
// Created by FUQAQ on 2025/1/27.
//

#include "UserService.h"

#include <CommonData.h>


UserService::UserService(LiteConn& db): userDAO(db) {}

UserBaseInfoDTO UserService::getUserBySSID(const QString& ssid) {
    UserBaseInfoDO result = userDAO.findById(ssid);
    if (!result.ssid.isEmpty()) {
        return {
            ssid,result.ssname,
            result.avatar,
            result.sex==UserSex::Female?"女生":"男生",
            result.personalSign,
            result.birthday,
            result.thumbUpCount,
            result.region,
            result.createTime
        };
    }
    else {
        return {"-1"};
    }
}

bool UserService::setUserInfoData(QList<UserBaseInfoDTO> dto) {
    bool res = true;
    for (auto it : dto) {
        res = userDAO.insert({it.ssid,it.username,it.avatarPath.isEmpty()?"":(it.avatarPath),
        (it.sex==u8"女生"?UserSex::Female:UserSex::Male),
            it.personalSign,0,it.birthDate,it.createTime,it.region});
        if (!res)
            return res;
    }
    return res;
}

bool UserService::addUser(const UserBaseInfoDTO &dto) {
    return userDAO.insert({dto.ssid,dto.username,
        (dto.avatarPath.isEmpty()?"":dto.avatarPath),
        (dto.sex==u8"女生"?UserSex::Female:UserSex::Male),
            dto.personalSign,0,dto.birthDate,dto.createTime,dto.region});
}

bool UserService::updateUserBySSID(const UserBaseInfoDTO &dto) {
    UserBaseInfoDO tmpDO = userDAO.findById(dto.ssid);
    if (tmpDO.createTime != 0) {
        tmpDO.ssname = (dto.username == "" ?tmpDO.ssname : dto.username);
        tmpDO.avatar = (dto.avatarPath == ""?tmpDO.avatar:dto.avatarPath);
        tmpDO.personalSign = (dto.personalSign == ""?tmpDO.personalSign: dto.personalSign);
        tmpDO.birthday = (dto.birthDate!=0?tmpDO.birthday: dto.birthDate);
        tmpDO.region = (dto.region==-1?tmpDO.region: dto.region);
        tmpDO.sex = (dto.sex==""?tmpDO.sex:(dto.sex==u8"女生"?UserSex::Female:UserSex::Male));
        return userDAO.update(tmpDO);
    }else {
        LOG_ERROR("UserService::updateUserBySSID failed : cant find user by ssid [" + dto.ssid.toStdString() + "]")
    }
    return false;
}

bool UserService::updateThumbUpCount(const QString &ssid, int newCount) {
    return userDAO.updateThumbUpCount(ssid, newCount);
}

bool UserService::updateAvatarPath(const QString &ssid, const QString &newUrl) {
    return userDAO.updateAvatar(ssid, newUrl);
}

int UserService::getRegionCount(quint8 code) {
    return userDAO.getRegionCount(code);
}
