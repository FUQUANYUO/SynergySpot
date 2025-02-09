//
// Created by FUQAQ on 2025/2/6.
//

#ifndef USERSERVICE_H
#define USERSERVICE_H

#include "../../dao/user/UserDAO.h"
#include "../../dto/DataProcessDTO.h"

class UserService {
public:
    // login check
    bool loginCheck(const UserPrivateInfoDTO& pInfo);

    // get user info by ssid
    UserBaseInfoDTO getUserInfo(const std::string& ssid);

    // update user info
    bool updateUserBaseInfo(const UserBaseInfoDTO& userInfo);

    // change user private info
    bool updateUserPrivateInfo(const UserPrivateInfoDTO& userInfo);

    // enroll account
    std::string enrollAcc(const UserPrivateInfoDTO& userPrivateInfo);

    // fuzzy matching
    std::vector<UserBaseInfoDTO> fuzzyMatch(const std::string& ssid,const std::string& name);

private:
    UserDAO        userDAO;
    UserPrivateDAO userPrivateDAO;
};



#endif //USERSERVICE_H
