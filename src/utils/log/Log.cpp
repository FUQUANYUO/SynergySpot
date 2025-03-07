//
// Created by FUQAQ on 2025/1/14.
//

#include <iostream>
#include "Log.h"
namespace SSLog {
    LogFile::LogFile(const std::string &logName) {
        std::filesystem::path dir = "log";
        if (!std::filesystem::exists(dir)) {
            if (!std::filesystem::create_directories(dir)) {
                throw std::runtime_error("Failed to create log directory");
            }
        }
        std::string filename = "log/" + logName + "_" + GetCurTime::getTimeObj()->getCurTime("%Y-%m-%d") + ".log";
        file.open(filename, std::ios::out | std::ios::app);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open log file: " + filename);
        }
    }

    LogFile::~LogFile() {
        if (file.is_open()) {
            file.close();
        }
    }

    void LogFile::write(const std::string &message)  {
        std::lock_guard<std::mutex> lock(mutex);
        if (file.is_open()) {
            file << message << std::endl;
            file.flush();
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