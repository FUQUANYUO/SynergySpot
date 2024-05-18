//
// Created by FU-QAQ on 2024/5/9.
//
#include <sstream>

#include "DoAddFriend.h"
#include "../Server.h"
#include "db-pool/ConnectionPool.h"

#include "friend/FriendDTO.pb.h"

#include "yaml-cpp/yaml.h"

std::string acceptStr;
std::string rejectStr;
std::string waitedStr;

extern std::mutex m;
extern std::string yamlPath;

DoAddFriend::DoAddFriend() {
    YAML::Node node = YAML::LoadFile(yamlPath);
    acceptStr = node["request-friend-status"]["acceptStr"].as<std::string>();
    rejectStr = node["request-friend-status"]["rejectStr"].as<std::string>();
    waitedStr = node["request-friend-status"]["waitedStr"].as<std::string>();
}


void DoAddFriend::execStoreContactInfo(const Friendship& fship) {
    // 检查是否存在关系数据
    std::stringstream sqlForCheckExits;
    sqlForCheckExits << "SELECT `ssid` FROM `friendship` "
                     << "WHERE ("
                     << "ssid=" << SQL_STR(fship.ssid) << " AND " << "friend_ssid=" << SQL_STR(fship.friend_ssid) << ") OR ("
                     << "ssid=" << SQL_STR(fship.friend_ssid) << " AND " << "friend_ssid=" << SQL_STR(fship.ssid) << ")";
    {
        // 查询
        std::lock_guard<std::mutex> lg(m);
        MYSQL_RES *res;
        res = ConnectionPool::getConnectPool()->getConnection()->query(sqlForCheckExits.str());
        MYSQL_ROW row = mysql_fetch_row(res);
        if(row != nullptr)
        {
            LOG("the wait to store data for friendship has been stored, that's repeated operated!")
            return ;
        }
    }

    std::stringstream sqlForStoreContactInfo;
    sqlForStoreContactInfo << "INSERT INTO `friendship` "
                           << "(ssid,friend_ssid,ship_status) "
                           << "VALUES ("
                           << SQL_STR(fship.ssid) << ","
                           << SQL_STR(fship.friend_ssid) << ","
                           << fship.ship_status << "), ("
                           << SQL_STR(fship.friend_ssid) << ","
                           << SQL_STR(fship.ssid) << ","
                           << fship.ship_status << ")";
    // 写入加锁
    std::lock_guard<std::mutex> lg(m);
    bool res = ConnectionPool::getConnectPool()->getConnection()->update(sqlForStoreContactInfo.str());
    if(!res){
        LOG("store friendship has occur some error!")
    }
}

void DoAddFriend::execStoreContactInfo(const GroupMemberInfo &gminfo) {
    // 检查是否存在关系数据
    std::stringstream sqlForCheckExits;
    sqlForCheckExits << "SELECT `ssid_group` FROM `group_member_info` "
                     << "WHERE "
                     << "ssid_group=" << SQL_STR(gminfo.ssid_group) << " AND " << "ssid_member=" << SQL_STR(gminfo.ssid_member);
    {
        // 查询
        std::lock_guard<std::mutex> lg(m);
        MYSQL_RES *res;
        res = ConnectionPool::getConnectPool()->getConnection()->query(sqlForCheckExits.str());
        MYSQL_ROW row = mysql_fetch_row(res);
        if(row != nullptr)
        {
            LOG("the wait to store data for friendship has been stored, that's repeated operated!")
            return ;
        }
    }

    std::stringstream sqlForStoreContactInfo;
    sqlForStoreContactInfo << "INSERT INTO `group_member_info` "
                           << "(ssid_group,ssid_member) "
                           << "VALUES ("
                           << SQL_STR(gminfo.ssid_group) << ","
                           << SQL_STR(gminfo.ssid_member) << ")";
    // 写入加锁
    std::lock_guard<std::mutex> lg(m);
    bool res = ConnectionPool::getConnectPool()->getConnection()->update(sqlForStoreContactInfo.str());
    if(!res){
        LOG("store group member info has occur some error!")
    }
}

std::string DoAddFriend::execAddFriend(const std::string &dto) {
    SSDTO::AddFriend_DTO afdto;
    afdto.ParseFromString(dto);

    std::string out;

    bool isGroup = afdto.receiver().is_group();
    if(afdto.is_accept() == acceptStr || afdto.is_accept() == rejectStr){
        if(isGroup){
            GroupMemberInfo gminfo;
            gminfo.ssid_group = afdto.receiver().ssid();
            gminfo.ssid_member = afdto.sender().ssid();
            execStoreContactInfo(gminfo);
        }else{
            Friendship fship;
            fship.ssid = afdto.sender().ssid();
            fship.friend_ssid = afdto.receiver().ssid();
            execStoreContactInfo(fship);
        }
        afdto.SerializeToString(&out);
    }else if(afdto.is_accept() == waitedStr){
        // 转发给 receiver
        auto res = onlineList.find(afdto.receiver().ssid());
        afdto.SerializeToString(&out);
        if(res != onlineList.end()){
            // 在线
            res->second->tcp->sendMsg(out,SSDTO::Business_Type::ADD_FRIEND);
        }else{
            // 离线
            auto findReceiver = businessSent.find(afdto.receiver().ssid());
            if(findReceiver != businessSent.end()){
                findReceiver->second.emplace_back(out,SSDTO::Business_Type::ADD_FRIEND);
            }else{
                businessSent[afdto.receiver().ssid()] = {{out,SSDTO::Business_Type::ADD_FRIEND}};
            }
        }
        return "";
    }
    return out;
}
