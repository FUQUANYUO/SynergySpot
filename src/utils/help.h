//
// Created by FU-QAQ on 2024/2/25.
//

#ifndef SYNERGYSPOT_HELP_H
#define SYNERGYSPOT_HELP_H

#include <iostream>
#include <algorithm>
#include <sstream>

extern std::string yamlPath;
extern std::string CurSSID;
extern std::string CurSSname;

#include "log/Log.h"

// 日志宏
#define LOG_INFO(__EXPR__) \
    do { \
        std::ostringstream oss; \
        oss << __EXPR__; \
        SSLog::log(SSLog::LogLevel::SS_INFO, __FILE__, __LINE__, oss.str()); \
    } while (0);

#define LOG_WARNING(__EXPR__) \
    do { \
        std::ostringstream oss; \
        oss << __EXPR__; \
        SSLog::log(SSLog::LogLevel::SS_WARNING, __FILE__, __LINE__, oss.str()); \
    } while (0);

#define LOG_ERROR(__EXPR__) \
    do { \
        std::ostringstream oss; \
        oss << __EXPR__; \
        SSLog::log(SSLog::LogLevel::SS_ERROR, __FILE__, __LINE__, oss.str()); \
    } while (0);

#define LOG(__EXPR__) \
    do { \
        std::ostringstream oss; \
        oss << __EXPR__; \
        SSLog::log(SSLog::LogLevel::SS_DEFAULT, __FILE__, __LINE__, oss.str()); \
    } while (0);
#endif//SYNERGYSPOT_HELP_H
