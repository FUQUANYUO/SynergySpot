//
// Created by FUQAQ on 2025/1/21.
//

#ifndef COMMONDATA_H
#define COMMONDATA_H

#include "define.h"
#include "yaml-cpp/yaml.h"

#include "common-dto/CommonDatabaseDTO.h"

#include "common-service/user-service/UserService.h"
#include "common-service/sticker-service/StickerService.h"
#include "common-service/message-content-service/MessageContentService.h"
#include "common-service/group-member-service/GroupMemberService.h"
#include "common-service/group-info-service/GroupInfoService.h"
#include "common-service/friendship-service/FriendshipService.h"

#include <QCache>

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

class SS_API CommonData {
public:
    static CommonData * getInstance();
    static void destroyInstance();

    // yamlPath //
    std::string                getYamlPath() const;
    void                       setYamlPath(std::string yamlPath);

    YAML::Node                 getYamlNode() const;

    // user info //
    CurUserInfoDataStruct      getCurUserInfo() const;
    void                       setCurUserInfo(const CurUserInfoDataStruct& curUserInfo);

    UserBaseInfoDTO            getUserInfoBySSID(const QString& ssid);
    bool                       setUserInfoBySSID(const UserBaseInfoDTO& userInfo);

    // friendship
    QList<FriendshipDTO>       getCurUserFriendship();
    bool                       setFriendshipData(const QList<FriendshipDTO>& dto);
    bool                       isCurUserFriend(const QString& ssid);

    // msg content
    QList<MessageContentDTO>   getMessageContentData(int pageSize, int pageNum);
    bool                       setMessageContentData(const QList<MessageContentDTO>& dto);

    // group info
    QList<GroupBaseInfoDTO>    getAllGroupInfo(int pageSize, int pageNum);
    GroupBaseInfoDTO           getGroupInfoDataBySSID(const QString& ssidGroup);
    bool                       setGroupInfoData(const QList<GroupBaseInfoDTO>& dto);

    // group member
    QList<GroupMemberInfoDTO>  getGroupMemberInfoData(const QString& ssidGroup,int pageSize, int pageNum);
    bool                       setGroupMemberInfoData(const QList<GroupMemberInfoDTO>& dto);

    // init user app cache
    bool                       initCurUserInfoDir();

    // get path
    std::string                getDataPath(CommonPath type) const;

    // clear tmp data
    void                       clearTmpData() const;

    // tmp data for the msg-pic
    void                       addMsgPicToTmp(const QImage& image, const std::string& picName);
    QImage                     getMsgPicPathFromTmp(const std::string& picName);

    // tmp data for the file
    void                       addFileToTmp();
    std::string                getFilePathFromTmp();

    // avatar data
    void                       addAvatarToData();
    std::string                getAvatarPathFromData(const QString& ssid) const;

    // emoji
    bool                       addCollectionEmoji(const CollectedStickerDTO& dto);
    QList<CollectedStickerDTO> getCollectionEmoji(int pageSize, int pageNum);
    QList<BaseStickerDTO>      getBaseEmoji(int pageSize, int pageNum);

    // init database
    bool                       initUserDatabase();

    std::string                getImageEx() {
        return _imageEx;
    }

    // cache map
    void setCacheMaxSize(int maxSize) {
        _cacheMaxSize = maxSize;
    }

private:
    void init();
    explicit CommonData();
    ~CommonData();
private:
    bool                    _enable;
    std::string             _yamlPath;
    std::string             _imageEx;
    std::string             _liteDBName;
    CurUserInfoDataStruct   _userInfo;
    YAML::Node              _node;
    LiteConn                *_liteConn;

    int                     _cacheMaxSize;
// ------------------ service -------------------- //
    UserService             *userService;
    StickerService          *stickerService;
    MessageContentService   *messageContentService;
    GroupMemberService      *groupMemberService;
    GroupInfoService        *groupInfoService;
    FriendshipService       *friendshipService;
// ------------------ service -------------------- //

// ------------------ cache map ------------------ //
    QCache<QString, UserBaseInfoDTO>            userInfoCacheMap;
    QCache<QString, GroupBaseInfoDTO>           groupInfoCacheMap;
    QCache<QString, QList<GroupMemberInfoDTO>>  groupMemberInfoCacheMap;
// ------------------ cache map ------------------ //

    static CommonData * instance;
};



#endif //COMMONDATA_H
