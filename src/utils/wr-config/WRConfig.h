//
// Created by FU-QAQ on 2025/5/26.
//

#ifndef WRCONFIG_H
#define WRCONFIG_H

#include "help.h"

#include <QString>

class QSettings;

#define g_pGenUUID WRConfig::getInstance()

class WRConfig {
public:
    static WRConfig * getInstance();
    static void destroyInstance();

    // -- get settings page config --
    // get download file path
    QString getDownloadPath();

    // set download file path
    void setDownloadPath(QString downloadPath);

private:
    WRConfig();
    ~WRConfig();

    static WRConfig * _instance;

    QSettings       * _settings = nullptr;
};



#endif //WRCONFIG_H
