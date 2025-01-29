//
// Created by FUQAQ on 2025/1/14.
//

#include <iostream>
#include "Log.h"
namespace SSLog {
    LogFile::LogFile(const std::string &logName) {
        try {
            if (!std::filesystem::exists("log")) {
                std::filesystem::create_directories("log");  // 使用create_directories创建多级目录
            }
            std::filesystem::path logFilePath = ("log/" + logName + "_" + GetCurTime::getTimeObj()->getCurTime("%Y-%m-%d") + ".log");
            file.open(logFilePath, std::ios::out | std::ios::app);
            if (!file.is_open()) {
                throw std::runtime_error("Failed to open log file: " + logFilePath.string());
            }
        } catch (std::exception &e) {
            std::cerr << "init Log file occur error : " << e.what() << std::endl;
        }
    }

    LogFile::~LogFile() {
        if (file.is_open()) {
            file.close();
        }
    }

    void LogFile::write(const std::string &message)  {
        std::lock_guard<std::mutex> lock(mutex);  // 保证线程安全
        if (file.is_open()) {
            file << message << std::endl;
        }
    }

    void log(LogLevel level, const std::string &file, int line, const std::string &message)  {
        std::string levelStr;
        switch (level) {
            case LogLevel::SS_INFO: levelStr = "INFO"; break;
            case LogLevel::SS_WARNING: levelStr = "WARNING"; break;
            case LogLevel::SS_ERROR: levelStr = "ERROR"; break;
        }

        std::string logMessage = "[" + levelStr + "] [" + file + "] |in " + std::to_string(line) + " line |at " + GetCurTime::getTimeObj()->getCurTime("%Y-%m-%d %H:%M:%S") + "] : " + message;
        if (level == LogLevel::SS_DEFAULT) {
            std::cout << logMessage << std::endl;
        }else {
            // 输出到日志文件
            if (logFile) {
                logFile->write(logMessage);
            }else {
                std::cout << logMessage << std::endl;
            }
        }
    }
}