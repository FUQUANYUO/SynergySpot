//
// Created by FU-QAQ on 2024/4/19.
//
#include "GetCurTime.h"
#include <array>
#include <iomanip>
#include <sstream>

GetCurTime * instance = nullptr;

GetCurTime *GetCurTime::getTimeObj() {
    instance = new GetCurTime();
    return instance;
}

std::string GetCurTime::getCurTime() {
    // 获取当前时间
    std::time_t now = std::time(nullptr);
    // tm 结构体
    std::tm local_time{};
    // 转换为tm结构体
#ifdef WIN32
    localtime_s(&local_time,&now);
#else
    localtime_r(&local_time,&now);
#endif
    // 创建一个足够大的字符串来存储格式化的时间
    std::array<char, 80> buffer{};

    // 使用sprintf来格式化时间
    // %Y 表示四位数的年份，%m 表示月份，%d 表示日，%H 表示小时（24小时制），%M 表示分钟
    std::strftime(buffer.data(), buffer.size(), "%Y-%m-%d %H-%M",&local_time);
    return {buffer.data()};
}

void GetCurTime::destroyTimeObj() {
    delete instance;
    instance = nullptr;
}

std::string GetCurTime::transformTimeStampToStr(std::time_t timestamp) {
    std::tm local_time{};
    // 转换为tm结构体
#ifdef WIN32
    localtime_s(&local_time,&timestamp);
#else
    localtime_r(&local_time,&timestamp);
#endif
    // 创建一个足够大的字符串来存储格式化的时间
    std::array<char, 80> buffer{};

    // 使用sprintf来格式化时间
    // %Y 表示四位数的年份，%m 表示月份，%d 表示日，%H 表示小时（24小时制），%M 表示分钟
    std::strftime(buffer.data(), buffer.size(), "%Y-%m-%d %H-%M",&local_time);
    return {buffer.data()};
}

std::time_t GetCurTime::transformStrToTimeStamp(const std::string& time) {
    std::tm timeStruct = {};
    // 使用std::istringstream来解析日期时间字符串
    std::istringstream stream(time);
    stream >> std::setw(4) >> timeStruct.tm_year // 4位数的年份
            >> std::setw(2) >> timeStruct.tm_mon   // 月份，从1开始计数
            >> std::setw(2) >> timeStruct.tm_mday  // 日期
            >> std::setw(2) >> timeStruct.tm_hour  // 小时
            >> std::setw(2) >> timeStruct.tm_min;  // 分钟
    timeStruct.tm_year -= 1900;
    timeStruct.tm_mon -= 1;

    // 使用mktime将tm结构体转换为时间戳
    std::time_t timestamp = std::mktime(&timeStruct);
    return timestamp;
}

std::time_t GetCurTime::getCurTimeStamp() {
    return std::time(nullptr);
}

