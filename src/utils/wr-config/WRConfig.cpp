//
// Created by FU-QAQ on 2025/5/26.
//

#include "WRConfig.h"

#include <QCoreApplication>
#include <QSettings>

WRConfig * WRConfig::_instance = nullptr;
static std::mutex m;

WRConfig * WRConfig::getInstance() {
    if (_instance == nullptr) {
        m.lock();
        if (_instance == nullptr) {
            _instance = new WRConfig;
        }
        m.unlock();
    }
    return _instance;
}

void WRConfig::destroyInstance() {
    if (_instance != nullptr) {
        m.lock();
        if (_instance != nullptr) {
            delete _instance;
            _instance = nullptr;
        }
        m.unlock();
    }
}

QString WRConfig::getDownloadPath() {
    return _settings->value("download-path").toString();
}

void WRConfig::setDownloadPath(QString downloadPath) {
    _settings->setValue("download-path", downloadPath);
    _settings->sync();
}

WRConfig::WRConfig() {
    QString curPath = QCoreApplication::applicationDirPath() + "/synergyspot-config.ini";
    _settings = new QSettings(curPath, QSettings::IniFormat);
}

WRConfig::~WRConfig() {}