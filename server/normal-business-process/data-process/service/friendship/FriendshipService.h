//
// Created by FUQAQ on 2025/2/6.
//

#ifndef FRIENDSHIPSERVICE_H
#define FRIENDSHIPSERVICE_H

#include "../../dao/friendship/FriendshipDAO.h"
#include "../../dto/DataProcessDTO.h"

class FriendshipService {
public:
    // add friendship
    bool addFriendship(const FriendshipDTO& dto);

    // remove friendship
    bool removeFriendship(const FriendshipDTO& dto);

    // change friend info
    bool updateFriendship(const FriendshipDTO& dto);

    // get all friendship for ssid
    std::vector<UserBaseInfoDTO> getAllFriendshipBySSID(const std::string& ssid);

    // get friend remark
    std::string getFriendRemark(const std::string& ssid, const std::string& friendSSID);

    // confirm friendship between ssid and the other
    bool isFriendship(const std::string& ssid, const std::string& otherSSID);
private:
    FriendshipDAO friendshipDAO;
};

#endif //FRIENDSHIPSERVICE_H
