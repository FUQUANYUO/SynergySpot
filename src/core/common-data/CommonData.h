//
// Created by FUQAQ on 2025/1/21.
//

#ifndef COMMONDATA_H
#define COMMONDATA_H

#include "define.h"
#include "yaml-cpp/yaml.h"

#ifdef SS_PLATFORM_WINDOWS
    #ifdef SS_COMMON_DATA_EXPORTS
        #define SS_API __declspec(dllexport)
    #else
        #define SS_API __declspec(dllimport)
    #endif
#elif defined SS_PLATFORM_LINUX
    #ifdef SS_COMMON_DATA_EXPORTS
        #define SS_API __attribute__((visibility("default")))
    #else
        #define SS_API
    #endif
#endif

#define g_pCommonData CommonData::getInstance()


struct CurUserInfoDataStruct {
    std::string CurSSID;
    std::string CurSSname;
    std::string CurUserAvatarPath;
};

enum CommonPath {
    avatar,
    emoji,
    snap,
    db,
    msgPic,
    file,
};

class SS_API CommonData {
public:
    static CommonData * getInstance();
    static void destroyInstance();

    // yamlPath //
    std::string getYamlPath() const;
    void setYamlPath(std::string yamlPath);

    YAML::Node getYamlNode() const;

    // cur user info //
    CurUserInfoDataStruct getCurUserInfo() const;
    void setCurUserInfo(const CurUserInfoDataStruct& curUserInfo);

    //       cache data       //
    //   cache dir framework  //
    // ---------------------- //
    // SynergySpot            //
    // -- ${CurSSID}          //
    //    -- data             //
    //       -- avatar        //
    //       -- emoji         //
    //       -- snap-pic      //
    //    -- db               //
    //    -- tmp              //
    //       -- msg-pic       //
    //       -- file          //
    // ---------------------- //

    // init user app cache
    bool initCurUserInfoDir();

    // get path
    std::string getDataPath(CommonPath type) const;

    // clear tmp data
    void clearTmpData() const;

    // tmp data for the msg-pic
    void addMsgPicToTmp(const QImage& image, const std::string& picName);
    QImage getMsgPicPathFromTmp(const std::string& picName);

    // tmp data for the file
    void addFileToTmp();
    std::string getFilePathFromTmp();

    // avatar data
    void addAvatarToData();
    std::string getAvatarPathFromData(const QString& ssid) const;

    // emoji
    void addEmojiToData();

    // init database
    bool initUserDatabase();

    std::string getImageEx() {
        return _imageEx;
    }

private:
    explicit CommonData();
    ~CommonData();
private:
    std::string             _yamlPath;
    std::string             _imageEx;
    std::string             _liteDBName;
    CurUserInfoDataStruct   _userInfo;
    YAML::Node              _node;

    static CommonData * instance;
};



#endif //COMMONDATA_H
