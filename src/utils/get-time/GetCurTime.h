//
// Created by FU-QAQ on 2024/4/19.
//

#ifndef SYNERGYSPOT_GETCURTIME_H
#define SYNERGYSPOT_GETCURTIME_H

#include <ctime>
#include "help.h"

class GetCurTime {
public:
    static GetCurTime * getTimeObj();

    // 返回格式 'xxxx-xx-xx xx:xx:xx' 当前时间
    std::string getCurTime();

    // 返回当前时间的时间戳
    std::time_t getCurTimeStamp();

    // 根据提供的格式 'xxxx-xx-xx xx:xx:xx' 转为时间戳
    std::time_t transformStrToTimeStamp(const std::string& time);

    // 根据时间戳转为格式 'xxxx-xx-xx xx:xx:xx'
    std::string transformTimeStampToStr(std::time_t timestamp);

    // 销毁对象
    void destroyTimeObj();

private:
    GetCurTime() = default;
    ~GetCurTime() = default;
    GetCurTime(const GetCurTime&) = delete;
    GetCurTime& operator=(const GetCurTime&) = delete;
};


#endif//SYNERGYSPOT_GETCURTIME_H
