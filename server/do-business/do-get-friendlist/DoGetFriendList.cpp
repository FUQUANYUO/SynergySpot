//
// Created by FU-QAQ on 2024/3/10.
//

#include "DoGetFriendList.h"

#include <sstream>

#include "../../../src/utils/db-pool/ConnectionPool.h"
#include "friend/FriendDTO.pb.h"

extern std::mutex m;

std::string DoGetFriendList::execQueryFriendListBySSID(std::string ssid) {
    SSDTO::GetFriendList_DTO gfdto;
    std::string out;
    gfdto.set_type(SSDTO::GET_CONTACTLIST);
    gfdto.set_request_ssid(ssid);
    gfdto.SerializeToString(&out);
    return execQueryFriendList(out);
}

std::string DoGetFriendList::execQueryFriendList(const std::string &dto) {
    SSDTO::GetFriendList_DTO gfdto;
    gfdto.ParseFromString(dto);
    if(gfdto.friend_infos_size() >= 1){
        gfdto.clear_friend_infos();
    }

    // 查询好友
    std::stringstream sqlForFriend;
    sqlForFriend << "SELECT fs.friend_ssid,ub.ssname,uci.friendship_remark ";
    sqlForFriend << "FROM friendship fs JOIN user_base_info ub ON fs.friend_ssid = ub.ssid ";
    sqlForFriend << "LEFT JOIN user_contact_info uci ON uci.friendship_id = fs.id ";
    sqlForFriend << "WHERE fs.ssid = '";
    sqlForFriend << gfdto.request_ssid() << "' order by fs.create_time DESC";

    // 查询群聊
    std::stringstream sqlForGroup;
    sqlForGroup << "SELECT gmi.ssid_group,gbi.name,uci.group_remark ";
    sqlForGroup << "FROM group_member_info gmi JOIN group_base_info gbi ON gmi.ssid_group = gbi.ssid_group ";
    sqlForGroup << "LEFT JOIN user_contact_info uci ON uci.group_id = gmi.id ";
    sqlForGroup << "WHERE gmi.ssid_member = '";
    sqlForGroup << gfdto.request_ssid() << "' order by gmi.create_time DESC";

    // 读锁
    std::lock_guard<std::mutex> lg(m);
    MYSQL_RES * mResF = ConnectionPool::getConnectPool()->getConnection()->query(sqlForFriend.str());
    MYSQL_RES * mResG = ConnectionPool::getConnectPool()->getConnection()->query(sqlForGroup.str());
    if(mResF == nullptr || mResG == nullptr){
        LOG("query res is empty")
    }else{
        MYSQL_ROW row;
        // 提取好友结果
        while((row = mysql_fetch_row(mResF))){
            SSDTO::FriendInfo fi;
            fi.set_is_group(false);
            fi.set_ssid(row[0]);
            fi.set_nickname(row[1]);
            if(row[2] == nullptr){
                fi.set_remark("");
            }
            else{
                fi.set_remark(row[2]);
            }
            gfdto.add_friend_infos()->CopyFrom(fi);
        }
        while((row = mysql_fetch_row(mResG))){
            SSDTO::FriendInfo fi;
            fi.set_is_group(true);
            fi.set_ssid(row[0]);
            fi.set_nickname(row[1]);
            if(row[2] == nullptr){
                fi.set_remark("");
            }
            else{
                fi.set_remark(row[2]);
            }
            gfdto.add_friend_infos()->CopyFrom(fi);
        }
    }
    std::string outGfdto;
    gfdto.SerializeToString(&outGfdto);
    return outGfdto;
}
