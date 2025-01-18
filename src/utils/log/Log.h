//
// Created by FUQAQ on 2025/1/14.
//

#ifndef LOG_H
#define LOG_H
#pragma once

#include <fstream>
#include <filesystem>
#include <memory>
#include <mutex>
#include "get-time/GetCurTime.h"

namespace SSLog {
    // 日志级别
    enum class LogLevel {
        SS_INFO = 1,
        SS_WARNING = 2,
        SS_ERROR = 3,
        SS_DEFAULT = 4,
    };

    class LogFile {
    public:
        LogFile(const std::string& logName);

        ~LogFile();

        void write(const std::string& message);
    private:
        std::ofstream file;
        std::mutex mutex;
    };

    inline std::unique_ptr<LogFile> logFile;

    // 初始化日志文件
    inline void initLogFile(const std::string& logName) {
        logFile = std::make_unique<LogFile>(logName);
    }

    // 日志函数
    void log(LogLevel level, const std::string& file, int line, const std::string& message);
}

#endif //LOG_H
