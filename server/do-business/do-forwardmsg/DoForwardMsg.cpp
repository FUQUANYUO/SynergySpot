//
// Created by FU-QAQ on 2024/3/1.
//
#include <sstream>
#include <thread>
#include <yaml-cpp/yaml.h>
#include <nlohmann/json.hpp>

#include "DoForwardMsg.h"
#include "../../Server.h"

#include "forward_msg/ForwardMsgDTO.pb.h"

#include "db-pool/ConnectionPool.h"

extern std::mutex m;
extern std::mutex groupM;
// 构造json工具map   使用在并发环境下
extern std::unordered_map<std::string,nlohmann::json> jmp;

void DoForwardMsg::execForward(const std::string &dto) {
    SSDTO::ForwardMsg_DTO fmdto;
    fmdto.ParseFromString(dto);

    std::string send_ssid = fmdto.send_ssid();
    std::string recv_ssid = fmdto.recv_ssid();
    std::string content = fmdto.content();
    std::string date_time = fmdto.date_time();
    std::string outForward;
    std::cout << fmdto.content() << std::endl;
    if(isGroup(recv_ssid)){
        // 群聊消息转发，采用json格式来记录群聊中消息发送的所属关系
        fmdto.set_is_group(true);
        fmdto.set_send_ssid(recv_ssid);
        fmdto.set_content(makeJsonData(send_ssid,content));
        std::vector<std::string> ans = getGroupMember(recv_ssid);
        // 找到所有群成员
        for(auto const& member: ans){
            if(send_ssid == member) continue;
            outForward.clear();
            fmdto.set_recv_ssid(member);
            fmdto.SerializeToString(&outForward);
            auto res = onlineList.find(member);
            if(res != onlineList.end()){// 在线
                res->second->tcp->sendMsg(outForward,SSDTO::Business_Type::FOWARD_MSG);
            }else{// 不在线
                auto findReceiver = sentLog.find(member);
                if(findReceiver != sentLog.end()){// 存在消息未发
                    auto findSender = findReceiver->second.find(recv_ssid);
                    if(findSender != findReceiver->second.end()){// 将消息加入list
                        // 最多维护数目
                        // TODO : 需要存储溢出的消息
                        YAML::Node node = YAML::LoadFile(yamlPath);
                        while(findSender->second.size() >= node["offline-maintenance-cfg"]["maxGroupMsg"].as<int>()){
                            findSender->second.pop_front();
                        }
                        findSender->second.push_back(outForward);
                    }else{// 新的发送者
                        std::deque<std::string> de;
                        de.push_back(outForward);
                        findReceiver->second.insert({recv_ssid,de});
                    }
                }else{// 不存在消息未发
                    chatLog log;
                    std::deque<std::string> de;
                    de.push_back(outForward);
                    log.insert({recv_ssid,de});
                    sentLog[member] = log;
                }
            }
        }
    }else{
        fmdto.set_is_group(false);
        fmdto.SerializeToString(&outForward);
        auto res = onlineList.find(recv_ssid);
        if(res != onlineList.end()){// 在线
            res->second->tcp->sendMsg(outForward,SSDTO::Business_Type::FOWARD_MSG);
        }else{// 不在线
            auto findReceiver = sentLog.find(recv_ssid);
            if(findReceiver != sentLog.end()){// 存在消息未发
                auto findSender = findReceiver->second.find(send_ssid);
                if(findSender != findReceiver->second.end()){// 将消息加入list
                    findSender->second.push_back(outForward);
                }else{// 新的发送者
                    std::deque<std::string> de;
                    de.push_back(outForward);
                    findReceiver->second.insert({send_ssid,de});
                }
            }else{// 不存在消息未发
                chatLog log;
                std::deque<std::string> de;
                de.push_back(outForward);
                log.insert({send_ssid,de});
                sentLog[recv_ssid] = log;
            }
        }
    }
}
void DoForwardMsg::execForwardByMap(const std::string& curSSID) {
    auto res = sentLog.find(curSSID);
    if(res != sentLog.end()){ // 存在未发送的消息
        for(const auto& it : res->second){
            for(const auto& msg : it.second){
                onlineList[curSSID]->tcp->sendMsg(msg,SSDTO::Business_Type::FOWARD_MSG);
            }
            onlineList.erase(it.first);
        }
    }
}


bool DoForwardMsg::isGroup(const std::string &ssid) {
    std::stringstream sqlForIsGroup;
    sqlForIsGroup << "SELECT `ssid_group` FROM `group_base_info` "
                  << "WHERE `ssid_group`=" << SQL_STR(ssid);
    // 加锁
    std::lock_guard<std::mutex> lg(m);
    MYSQL_RES * res = ConnectionPool::getConnectPool()->getConnection()->query(sqlForIsGroup.str());
    MYSQL_ROW row;
    row = mysql_fetch_row(res);
    if(row != nullptr){
        return true;
    }
    return false;
}

std::vector<std::string> DoForwardMsg::getGroupMember(const std::string &ssid) {
    std::stringstream sqlForGetGroupMember;
    sqlForGetGroupMember << "SELECT `ssid_member` FROM `group_member_info` "
                         << "WHERE `ssid_group`=" << SQL_STR(ssid);
    // 加锁
    std::lock_guard<std::mutex> lg(m);
    MYSQL_RES * res = ConnectionPool::getConnectPool()->getConnection()->query(sqlForGetGroupMember.str());
    MYSQL_ROW row;
    std::vector<std::string> ans;
    while((row = mysql_fetch_row(res))){
        if(row[0] != nullptr){
            ans.emplace_back(row[0]);
        }
    }
    return ans;
}

std::string DoForwardMsg::makeJsonData(const std::string &ssid, const std::string &content) {
    std::unique_lock<std::mutex> ul(groupM);
    auto res = jmp.find(ssid);
    if(res != jmp.end()){
        res->second[ssid] = content;
    }else{
        nlohmann::json jobj;
        jobj[ssid] = content;
        jmp[ssid] = jobj;
    }
    ul.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // 等待并发
    ul.lock();
    std::string out = jmp[ssid].dump();
    jmp.erase(ssid);
    return out;
}
