//
// Created by FUQAQ on 2025/2/6.
//

#include "UserService.h"
#include "encrypt/EncryptString.h"

bool UserService::loginCheck(const UserPrivateInfoDTO &pInfo) {
    std::string checkSSID = pInfo.ssid;
    std::string checkPassword = pInfo.password;
    UserPrivateInfoDO res = userPrivateDAO.findById(checkSSID);
    if (!res.ssid.empty() && !res.password.empty())
        return (EncryptString::getEncryptObj()->encryptStringBySHA256(pInfo.password,res.passwordSalt) == res.password);
    else
        return false;
}

UserBaseInfoDTO UserService::getUserInfo(const std::string &ssid) {
    UserBaseInfoDTO resDTO;
    UserBaseInfoDO resDO = userDAO.findById(ssid);
    if (!resDO.ssid.empty()) {
        resDTO.ssid = resDO.ssid;
        resDTO.ssname = resDO.ssname;
        resDTO.avatarPath = resDO.avatar;
        resDTO.sex = resDO.sex;
        resDTO.personalSign = resDO.personalSign;
        resDTO.thumbUpCount = resDO.thumbUpCount;
        resDTO.birthday = resDO.birthday;
        resDTO.region = resDO.region;
        resDTO.createTime = resDO.createTime;
        return resDTO;
    }
    return {};
}

bool UserService::updateUserBaseInfo(const UserBaseInfoDTO &userInfo) {
    UserBaseInfoDO updateDO{
        userInfo.ssid,userInfo.ssname,userInfo.avatarPath,userInfo.sex,userInfo.personalSign,
        userInfo.thumbUpCount,userInfo.birthday,userInfo.region,userInfo.createTime
    };
    return userDAO.update(updateDO);
}

bool UserService::updateUserPrivateInfo(const UserPrivateInfoDTO &userInfo) {
    UserPrivateInfoDO updateDO{
        userInfo.ssid,userInfo.email,userInfo.password,userInfo.passwordSalt,userInfo.accountStatus
    };
    return userPrivateDAO.update(updateDO);
}

std::string UserService::enrollAcc(const UserPrivateInfoDTO &userPrivateInfo) {
    std::string newSSID = userDAO.getLastUserSSID();
    if (newSSID != "-1") {
        newSSID = std::to_string(std::stoi(newSSID) + 1);
    }else {
        // first user
        newSSID = "1000000";
    }
    bool res = userPrivateDAO.insert({
        newSSID,userPrivateInfo.email,userPrivateInfo.password,userPrivateInfo.passwordSalt
    });
    return res?newSSID:"-1";
}

std::vector<UserBaseInfoDTO> UserService::fuzzyMatch(const std::string &ssid, const std::string &name) {
    std::vector<UserBaseInfoDTO> userDto;
    std::vector<UserBaseInfoDO> res = userDAO.fuzzyMatchingByIdOrName(ssid,name);
    for (const auto &it : res) {
        userDto.push_back({it.ssid,it.ssname,it.avatar,it.sex,it.personalSign,it.thumbUpCount,it.birthday,it.region,it.createTime});
    }
    return userDto;
}

