//
// Created by FU-QAQ on 2024/5/10.
//

#include "DoBusinessBase.h"
#include "../Server.h"


void DoBusinessBase::execBusinessForwardByMap(const std::string& curSSID) {
    auto res = businessSent.find(curSSID);
    if(res != businessSent.end()){ // 存在未发送的业务
        for(const auto& it : res->second){
            onlineList[curSSID]->tcp->sendMsg(it.first,it.second);
        }
    }
}


