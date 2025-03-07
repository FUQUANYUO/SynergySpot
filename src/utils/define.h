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
    Friends,
    Groups,
};

struct ChatMessage{
    QString ssid;           // id
    QString name;           // name
    QString content;        // html rich text
    QString avatarPath;     // pic path
    bool isOwnMessage;      // distinguish between sender and receiver
};
Q_DECLARE_METATYPE(ChatMessage)

struct LocalInfo{
    QString province;
    QString city;
    QString district;
};
Q_DECLARE_METATYPE(LocalInfo)

struct UserInfo{
    UserType    _type;
    int         _joinDay = 0;
    int         _likeCount = 0;
    QString     _ssid;
    QString     _name;
    QString     _remark;
    QString     _signContent;
    QString     _picPath;
    LocalInfo   _localInfo;
    QString     _sex;
    bool isEmpty() const {
        if(_ssid.isEmpty() && _name.isEmpty() &&
           _remark.isEmpty() && _signContent.isEmpty())
            return true;
        return false;
    }
};
Q_DECLARE_METATYPE(UserInfo)

struct GroupNotice {
    QString content;
};

struct GroupInfo{
    UserType            _type;
    QString             _ssid;
    QString             _name;
    QString             _remark;
    QString             _resume;
    QString             _picPath;
    int                 _memberCount;
    QList<GroupNotice>  _notices;
    bool isEmpty() const {
        if(_ssid.isEmpty() && _name.isEmpty() &&
           _remark.isEmpty() && _picPath.isEmpty())
            return true;
        return false;
    }
};
Q_DECLARE_METATYPE(GroupInfo)


#endif//SYNERGYSPOT_DEFINE_H
