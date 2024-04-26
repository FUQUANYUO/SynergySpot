//
// Created by FU-QAQ on 2024/4/19.
//
#include "GetCurTime.h"
#include <array>
#include <chrono>
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
    std::strftime(buffer.data(), buffer.size(), "%Y-%m-%d %H:%M:%S",&local_time);
    return buffer.data();
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
    std::strftime(buffer.data(), buffer.size(), "%Y-%m-%d %H:%M:%S",&local_time);
    return buffer.data();
}

std::time_t GetCurTime::transformStrToTimeStamp(const std::string& time) {
    // 解析年月日
    int year = std::stoi(time.substr(0, 4));
    int month = std::stoi(time.substr(5, 2));
    int day = std::stoi(time.substr(8, 2));

    // 解析时分秒
    int hour = std::stoi(time.substr(11, 2));
    int minute = std::stoi(time.substr(14, 2));
    int second = std::stoi(time.substr(17, 2));

    // 构造tm结构体
    std::tm tmStruct{};
    tmStruct.tm_year = year - 1900; // tm_year是从1900年开始的
    tmStruct.tm_mon = month - 1;     // tm_mon是从0（一月）开始的
    tmStruct.tm_mday = day;
    tmStruct.tm_hour = hour;
    tmStruct.tm_min = minute;
    tmStruct.tm_sec = second;

    // 使用mktime将tm结构体转换为time_t
    std::time_t time_tStruct = std::mktime(&tmStruct);

    // 使用chrono库将time_t转换为时间戳
    auto duration = std::chrono::seconds(time_tStruct);
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    return timestamp;
}

std::time_t GetCurTime::getCurTimeStamp() {
    return std::time(nullptr);
}

