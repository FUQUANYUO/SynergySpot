//
// Created by FU-QAQ on 2024/5/9.
//

#include "DoSearchFriend.h"

#include "friend/FriendDTO.pb.h"

#include "table-api/user-base-info/UserBaseInfoApi.h"

std::string DoSearchFriend::execSearchFriendByKeyword(const std::string& dto) {
    SSDTO::Search_DTO sdto;
    sdto.ParseFromString(dto);

    std::string out;

    std::vector<UserBaseInfo> res;
    res = UserBaseInfoApi::execQueryBaseInfo(sdto.key_word());
    if(!res.empty()) {
        for (auto it: res) {
            if(it.ssid == sdto.request_ssid()) continue;
            SSDTO::FriendInfo *finfo = sdto.add_search_infos();
            finfo->set_ssid(it.ssid);
            finfo->set_nickname(it.ssname);
            finfo->set_remark(finfo->remark());
            finfo->set_is_group(false);
        }
    }
    sdto.SerializeToString(&out);
    return out;
}
