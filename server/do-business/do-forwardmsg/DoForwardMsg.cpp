//
// Created by FU-QAQ on 2024/3/1.
//
#include "DoForwardMsg.h"
#include "../../Server.h"

#include "forward_msg/ForwardMsgDTO.pb.h"


//未发送的聊天记录表  key:接收消息的账号  v:接收的消息
extern std::unordered_map<std::string, chatLog> sentLog;
//在线人员表  key:账号  v:与这个账号通信的Socket指针
extern std::unordered_map<std::string, TcpSocket *> onlineList;

bool execStoreMsg();

void DoForwardMsg::execForward(const std::string &dto) {
    SSDTO::ForwardMsg_DTO fmdto;
    fmdto.ParseFromString(dto);

    std::string send_ssid = fmdto.send_ssid();
    std::string recv_ssid = fmdto.recv_ssid();
    std::string content = fmdto.content();
    std::string date_time = fmdto.date_time();
    fmdto.set_is_forward_success(true);
    std::string outForward;
    fmdto.SerializeToString(&outForward);
    std::cout << fmdto.content() << std::endl;
    auto res = onlineList.find(recv_ssid);
    if(res != onlineList.end()){// 在线
        res->second->sendMsg(outForward,SSDTO::Business_Type::FOWARD_MSG);
    }else{// 不在线
        auto findRecver = sentLog.find(recv_ssid);
        if(findRecver != sentLog.end()){// 存在消息未发
            auto findSender = findRecver->second.find(send_ssid);
            if(findSender != findRecver->second.end()){// 将消息加入list
                findSender->second.push_back(outForward);
            }else{// 新的发送者
                std::list<std::string> l;
                l.push_back(outForward);
                findRecver->second.insert({send_ssid,l});
            }
        }else{// 不存在消息未发
            chatLog log;
            std::list<std::string> l;
            l.push_back(outForward);
            log.insert({send_ssid,l});
            sentLog[recv_ssid] = log;
        }
    }
}
void DoForwardMsg::execForwardByMap(std::string curSSID) {
    auto res = sentLog.find(curSSID);
    if(res != sentLog.end()){ // 存在未发送的消息
        for(auto it : res->second){
            for(auto msg : it.second){
                onlineList[curSSID]->sendMsg(msg,SSDTO::Business_Type::FOWARD_MSG);
            }
            onlineList.erase(it.first);
        }
    }
}


bool execStoreMsg() {
    return false;
}
