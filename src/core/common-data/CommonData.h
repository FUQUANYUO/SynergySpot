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
#include "common-service/login-record-service/LoginRecordService.h"
#include "common-service/file-service/FileService.h"


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

class SS_API CommonData : public QObject {
    Q_OBJECT
public:
    static CommonData * getInstance();
    static void destroyInstance();

    // yamlPath //
    std::string                getYamlPath() const;
    void                       setYamlPath(std::string yamlPath);

    YAML::Node                 getYamlNode() const;

    // user info //
    UserBaseInfoDTO            getCurUserInfo() const;
    void                       setCurUserInfo(const UserBaseInfoDTO& curUserInfo);

    UserBaseInfoDTO            getUserInfoBySSID(const QString& ssid);
    bool                       updateUserInfoBySSID(const UserBaseInfoDTO& userInfo);
    bool                       addUserInfoByServer(const UserBaseInfoDTO& userInfo);

    // user login record
    bool                       setLoginRecord(const LoginRecordDTO& loginInfo);
    bool                       removeLoginRecordBySSID(const QString& ssid);
    bool                       removeLoginRecordBefore(time_t date);
    QList<LoginRecordDTO>      getLoginRecord(int limit);


    // friendship
    QList<FriendshipDTO>       getCurUserFriendship();
    bool                       setFriendshipData(const QList<FriendshipDTO>& dto);
    bool                       isCurUserFriend(const QString& ssid);

    // msg content
    QList<MessageContentDTO>   getMessageContentData(int pageSize, int pageNum);
    bool                       setMessageContentData(const QList<MessageContentDTO>& dto,bool isFromRemote = false);
    time_t                     getLastMessageTime() const;

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

    // file data
    FileStorageDTO             getFileInfoById(const QString& fileId);
    FileStorageDTO             getFileInfoByPath(const QString& filePath);
    QList<FileStorageDTO>      getFileInfosBySSID(const QString& ssid,int pageSize,int pageNum);
    bool                       setFileInfo(const FileStorageDTO& fileInfo);

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
signals:
    void sigUpdateAvatarData();
    void sigUpdateMsgView();
    void sigSyncMsgContentDTO(const QList<MessageContentDTO>& dto);
    void sigSyncMsgPicFromRemote(const QList<QString>& files);

    // fuzzy search
    void sigFuzzySearchRequest(const QString& content,bool isGroup);
    void sigFuzzySearchFriendResponse(QList<UserBaseInfoDTO> dto,int waitCount);
    void sigFuzzySearchGroupResponse(QList<GroupBaseInfoDTO> dto,int waitCount);

    // add/join friend/group
    void sigAddFriendOrGroup(const QString& ssid, bool isGroup);

    // contact notice accept/reject
    void sigReplyFriendOrGroup(const QString& ssid,bool isAccept, bool isGroup);

    // get file
    void sigGetAvatarFileFromRemote(
        const QString& fileID,
        const QString& ssid,
        const QString& remotePath
    );

    void sigAllDataLoadFinished();
private:
    void init();
    explicit CommonData();
    ~CommonData();
private:
    bool                    _enable;
    std::string             _yamlPath;
    std::string             _imageEx;
    std::string             _liteDBName;
    UserBaseInfoDTO         _userInfo;
    YAML::Node              _node;
    LiteConn                *_liteConn;
    LiteConn                *_loginDB;

    int                     _cacheMaxSize;
// ------------------ service -------------------- //
    UserService             *userService;
    StickerService          *stickerService;
    MessageContentService   *messageContentService;
    GroupMemberService      *groupMemberService;
    GroupInfoService        *groupInfoService;
    FriendshipService       *friendshipService;
    FileService             *fileService;
    LoginRecordService      *loginRecordService;
// ------------------ service -------------------- //

// ------------------ cache map ------------------ //
    QCache<QString, UserBaseInfoDTO>            userInfoCacheMap;
    QCache<QString, GroupBaseInfoDTO>           groupInfoCacheMap;
    QCache<QString, QList<GroupMemberInfoDTO>>  groupMemberInfoCacheMap;
// ------------------ cache map ------------------ //

    static CommonData * instance;
};


#endif //COMMONDATA_H
