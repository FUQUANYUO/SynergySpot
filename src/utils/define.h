//
// Created by FU-QAQ on 2024/2/25.
//

#ifndef SYNERGYSPOT_DEFINE_H
#define SYNERGYSPOT_DEFINE_H

// ---------------- version ----------------
#define SS_VERSION "0.01b"
// ---------------- version ----------------

#ifdef _WIN32
    #define SS_PLATFORM_WINDOWS
#elif defined(__linux__)
    #define SS_PLATFORM_LINUX
#else
    #error "Unsupported platform"
#endif

#include <QObject>

class SS_Plugin : public QObject{
public:
    virtual ~SS_Plugin() = default;
    virtual const char* getPluginName() = 0;
};
Q_DECLARE_INTERFACE(SS_Plugin, "org.synergy-spot.SS_PluginInterface")

enum UserType{
    Myself,
    Strangers,
    Friends
};

struct LocalInfo{
    QString country;
    QString province;
    QString city;
};
Q_DECLARE_METATYPE(LocalInfo)

struct UserInfo{
    UserType    _type;
    int         _joinDay;
    int         _likeCount;
    QString     _ssid;
    QString     _name;
    QString     _remark;
    QString     _signContent;
    QString     _picPath;
    LocalInfo   _localInfo;
    bool isEmpty() const {
        if(_ssid.isEmpty() && _name.isEmpty() &&
           _remark.isEmpty() && _signContent.isEmpty())
            return true;
        return false;
    }
};
Q_DECLARE_METATYPE(UserInfo)


#endif//SYNERGYSPOT_DEFINE_H
