//
// Created by FUQAQ on 2025/1/21.
//

#include "CommonData.h"
#include "help.h"

#include "db-lite/LiteConn.h"

#include <QDir>
#include <QImage>

#include <mutex>
#include <db-pool/ConnectionPool.h>

CommonData * CommonData::instance = nullptr;
static std::mutex m;

CommonData * CommonData::getInstance() {
    if (instance == nullptr) {
        m.lock();
        if (instance == nullptr) {
            instance = new CommonData();
        }
        m.unlock();
    }
    return instance;
}

void CommonData::destroyInstance() {
    if (instance != nullptr) {
        m.lock();
        if (instance != nullptr) {
            delete instance;
            instance = nullptr;
        }
        m.unlock();
    }
}

std::string CommonData::getYamlPath() const {
    return _yamlPath;
}

void CommonData::setYamlPath(std::string yamlPath) {
    _yamlPath = yamlPath;
}

YAML::Node CommonData::getYamlNode() const {
    return _node;
}

UserBaseInfoDTO CommonData::getCurUserInfo() const {
    return _userInfo;
}

void CommonData::setCurUserInfo(const UserBaseInfoDTO &curUserInfo) {
    _userInfo = curUserInfo;
    init();
    if (!curUserInfo.sex.isEmpty() || !curUserInfo.avatarPath.isEmpty() || !curUserInfo.personalSign.isEmpty()) {
        updateUserInfoBySSID(curUserInfo);
    }
}

UserBaseInfoDTO CommonData::getUserInfoBySSID(const QString &ssid) {
    if (!_enable) {
        LOG_ERROR("Please init cur user info!");
        return {};
    }
    if (!userInfoCacheMap.contains(ssid)) {
        UserBaseInfoDTO uRes = userService->getUserBySSID(ssid);
        if (uRes.ssid == "-1") {
            LOG_WARNING("local cache cant find ssid < " << ssid.toStdString());
            return {};
        }
        userInfoCacheMap.insert(ssid,new UserBaseInfoDTO(uRes));
    }
    return *userInfoCacheMap[ssid];
}

bool CommonData::updateUserInfoBySSID(const UserBaseInfoDTO &userInfo) {
    if (!_enable) {
        LOG_ERROR("Please init cur user info!");
        return {};
    }
    bool res = userService->updateUserBySSID(userInfo);
    if (res) {
        userInfoCacheMap.insert(userInfo.ssid ,new UserBaseInfoDTO(userInfo));
        return true;
    }
    return false;
}

bool CommonData::addUserInfoByServer(const UserBaseInfoDTO &userInfo) {
    if (!_enable) {
        LOG_ERROR("Please init cur user info!");
        return {};
    }
    if (!userService->getUserBySSID(userInfo.ssid).ssid.isEmpty() && userService->getUserBySSID(userInfo.ssid).ssid != "-1" ) {
        updateUserInfoBySSID(userInfo);
    }
    return userService->addUser(userInfo);
}

bool CommonData::setLoginRecord(const LoginRecordDTO &loginInfo) {
    auto res = loginRecordService->getLoginRecordsBySSID(loginInfo.account);
    if (res.account == "-1") {
        loginRecordService->addLoginRecord(loginInfo);
    }else {
        if (res.plainPassword == loginInfo.plainPassword) {
            // update login time
            loginRecordService->updateLoginTime(loginInfo.account,loginInfo.loginTime);
        }else {
            // remove login data and insert new
            loginRecordService->deleteLoginRecord(loginInfo.account);
        }
    }
    return true;
}

bool CommonData::removeLoginRecordBySSID(const QString &ssid) {
    return loginRecordService->deleteLoginRecord(ssid);
}

bool CommonData::removeLoginRecordBefore(time_t date) {
    return loginRecordService->deleteLoginRecordsBefore(date);
}

QList<LoginRecordDTO> CommonData::getLoginRecord(int limit) {
    return loginRecordService->getLoginRecords(limit);
}

QList<FriendshipDTO> CommonData::getCurUserFriendship() {
    if (!_enable) {
        LOG_ERROR("Please init cur user info!");
        return {};
    }
    return friendshipService->getFriendshipsBySSID(_userInfo.ssid);
}

bool CommonData::setFriendshipData(const QList<FriendshipDTO> &dto) {
    if (!_enable) {
        LOG_ERROR("Please init cur user info!");
        return {};
    }
    return friendshipService->setFriendshipData(dto);
}

bool CommonData::isCurUserFriend(const QString &ssid) {
    if (!_enable) {
        LOG_ERROR("Please init cur user info!");
        return {};
    }
    return friendshipService->isCurUserFriend(_userInfo.ssid, ssid);
}

QList<MessageContentDTO> CommonData::getMessageContentData(int pageSize, int pageNum) {
    if (!_enable) {
        LOG_ERROR("Please init cur user info!");
        return {};
    }
    return messageContentService->getAllMessages(_userInfo.ssid,pageSize,pageNum);
}

bool CommonData::setMessageContentData(const QList<MessageContentDTO> &dto,bool isFromRemote) {
    if (!_enable) {
        LOG_ERROR("Please init cur user info!");
        return false;
    }
    if (!isFromRemote) { // cur user send msg doesn't immediately store in the local db
        emit sigSyncMsgContentDTO(dto);
        return true;
    }else {
        bool res = messageContentService->storeMessage(dto);

        // download from server
        QList<QString> needFiles;
        for (const auto& it : dto) {
            needFiles.append(it.fileId);
        }
        emit sigSyncMsgPicFromRemote(needFiles);
        return res;
    }
}
time_t CommonData::getLastMessageTime() const {
    if (!_enable) {
        LOG_ERROR("Please init cur user info!");
        return {};
    }
    return messageContentService->getLastMsgTime();
}

QList<GroupBaseInfoDTO> CommonData::getAllGroupInfo(int pageSize, int pageNum) {
    if (!_enable) {
        LOG_ERROR("Please init cur user info!");
        return {};
    }
    return groupInfoService->getGroupInfos(pageSize,pageNum);
}

GroupBaseInfoDTO CommonData::getGroupInfoDataBySSID(const QString &ssidGroup) {
    if (!_enable) {
        LOG_ERROR("Please init cur user info!");
        return {};
    }
    if (!groupInfoCacheMap.contains(ssidGroup)) {
        GroupBaseInfoDTO gRes = groupInfoService->getGroupInfoById(ssidGroup);
        if (gRes.ssidGroup == "-1") {
            LOG_WARNING("local cache cant find ssid < " << ssidGroup.toStdString())
            return {"-1"};
        }
        groupInfoCacheMap.insert(ssidGroup, new GroupBaseInfoDTO(gRes));
    }
    return *groupInfoCacheMap[ssidGroup];
}

bool CommonData::setGroupInfoData(const QList<GroupBaseInfoDTO> &dto) {
    if (!_enable) {
        LOG_ERROR("Please init cur user info!");
        return {};
    }
    return groupInfoService->setGroupInfoData(dto);
}

QList<GroupMemberInfoDTO> CommonData::getGroupMemberInfoData(const QString &ssidGroup,int pageSize, int pageNum) {
    if (!_enable) {
        LOG_ERROR("Please init cur user info!");
        return {};
    }
    if (!groupMemberInfoCacheMap.contains(ssidGroup)) {
        auto mRes = groupMemberService->getAllGroupMember(ssidGroup,pageSize,pageNum);
        if (mRes.count() <= 0) {
            LOG_WARNING("local cache cant find members < " << ssidGroup.toStdString());
            return {};
        }
        groupMemberInfoCacheMap.insert(ssidGroup,new QList<GroupMemberInfoDTO>(mRes));
    }
    return *groupMemberInfoCacheMap[ssidGroup];
}

bool CommonData::setGroupMemberInfoData(const QList<GroupMemberInfoDTO> &dto) {
    if (!_enable) {
        LOG_ERROR("Please init cur user info!");
        return {};
    }
    return groupMemberService->addGroupMember(dto);
}

bool CommonData::initCurUserInfoDir() {
    if (!_enable) {
        LOG_ERROR("Please init cur user info!");
        return {};
    }
    do {
        QString userDir = _userInfo.ssid;
        QDir user(userDir);
        if (!user.exists() && !user.mkpath(".")) {
            LOG_ERROR("Failed to create user directory")
            return false;
        }
        // data
        QString dataDir = userDir + "/data";
        QDir data(dataDir);
        if (!data.exists() && !data.mkpath(".")) {
            LOG_ERROR( "Failed to create data directory:" << dataDir.toStdString());
            return false;
        }

        QString avatarDir = dataDir + "/avatar";
        QDir avatar(avatarDir);
        if (!avatar.exists() && !avatar.mkpath(".")) {
            LOG_ERROR( "Failed to create avatar directory:" << avatarDir.toStdString());
            return false;
        }

        QString emojiDir = dataDir + "/emoji";
        QDir emoji(emojiDir);
        if (!emoji.exists() && !emoji.mkpath(".")) {
            LOG_ERROR( "Failed to create emoji directory:" << emojiDir.toStdString());
            return false;
        }

        QString snapPicDir = dataDir + "/snap-pic";
        QDir snapPic(snapPicDir);
        if (!snapPic.exists() && !snapPic.mkpath(".")) {
            LOG_ERROR( "Failed to create snap-pic directory:" << snapPicDir.toStdString());
            return false;
        }

        // db
        QString dbDir = userDir + "/db";
        QDir db(dbDir);
        if (!db.exists() && !db.mkpath(".")) {
            LOG_ERROR( "Failed to create db directory:" << dbDir.toStdString());
            return false;
        }

        // tmp
        QString tmpDir = userDir + "/tmp";
        QDir tmp(tmpDir);
        if (!tmp.exists() && !tmp.mkpath(".")) {
            LOG_ERROR( "Failed to create tmp directory:" << tmpDir.toStdString());
            return false;
        }

        QString msgPicDir = tmpDir + "/msg-pic";
        QDir msgPic(msgPicDir);
        if (!msgPic.exists() && !msgPic.mkpath(".")) {
            LOG_ERROR( "Failed to create msg-pic directory:" << msgPicDir.toStdString());
            return false;
        }

        QString fileDir = tmpDir + "/file";
        QDir file(fileDir);
        if (!file.exists() && !file.mkpath(".")) {
            LOG_ERROR( "Failed to create file directory:" << fileDir.toStdString());
            return false;
        }
    }while (false);
    return true;
}

std::string CommonData::getDataPath(CommonPath type) const {
    if (!_enable) {
        LOG_ERROR("Please init cur user info!");
        return {};
    }
    std::string path = _userInfo.ssid.toStdString();
    switch (type) {
        case avatar:
            path.append("/data/avatar");
            break;
        case emoji:
            path.append("/data/emoji");
            break;
        case snap:
            path.append("/data/snap-pic");
            break;
        case db:
            path.append("/db");
            break;
        case msgPic:
            path.append("/tmp/msg-pic");
            break;
        case file:
            path.append("/tmp/file");
            break;
    }
    return path;
}

void CommonData::clearTmpData() const {
    QString msgTmp  = QString::fromStdString(getDataPath(msgPic));
    QString fileTmp = QString::fromStdString(getDataPath(file));

    QDir msgTmpDir(msgTmp);
    if (msgTmpDir.exists()) {
        msgTmpDir.setNameFilters(QStringList() << "*");
        msgTmpDir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot); // 包括文件和子目录
        for (const QString& entry : msgTmpDir.entryList()) {
            QFileInfo fileInfo(msgTmpDir.filePath(entry));
            if (fileInfo.isDir()) {
                QDir subDir(fileInfo.filePath());
                subDir.removeRecursively(); // 递归删除子目录
            } else {
                msgTmpDir.remove(entry);
            }
        }
    }
    QDir fileTmpDir(fileTmp);
    if (fileTmpDir.exists()) {
        fileTmpDir.setNameFilters(QStringList() << "*");
        fileTmpDir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot); // 包括文件和子目录
        for (const QString& entry : fileTmpDir.entryList()) {
            QFileInfo fileInfo(fileTmpDir.filePath(entry));
            if (fileInfo.isDir()) {
                QDir subDir(fileInfo.filePath());
                subDir.removeRecursively(); // 递归删除子目录
            } else {
                fileTmpDir.remove(entry);
            }
        }
    }
}

void CommonData::addMsgPicToTmp(const QImage &image, const std::string &picName) {
    std::string msgPicPath = getDataPath(msgPic);
    std::string imagePath = msgPicPath + "/" + picName + _imageEx;
    if (!image.save(QString::fromStdString(imagePath))) {
        LOG_ERROR( "Failed to save image to file:" << imagePath);
    }
}

QImage CommonData::getMsgPicPathFromTmp(const std::string &picName) {
    std::string msgPicPath = getDataPath(msgPic);
    std::string imagePath = msgPicPath + "/" + picName + _imageEx;

    // 检查文件是否存在
    if (!QFileInfo(QString::fromStdString(msgPicPath),QString::fromStdString(picName + _imageEx)).exists()) {
        LOG_ERROR("File does not exist:" << imagePath);
        return QImage();
    }

    // 加载图片
    QImage image;
    if (!image.load(QString::fromStdString(imagePath))) {
        LOG_ERROR("Failed to load image from:" << imagePath);
        return QImage();
    }

    return image;
}

FileStorageDTO CommonData::getFileInfoById(const QString &fileId) {
    if (!_enable) {
        LOG_ERROR("Please init cur user info!");
        return {"-1"};
    }
    return fileService->getFileByFileID(fileId);
}

FileStorageDTO CommonData::getFileInfoByPath(const QString &filePath) {
    if (!_enable) {
        LOG_ERROR("Please init cur user info!");
        return {"-1"};
    }
    return fileService->getFileByFilePath(filePath);
}

QList<FileStorageDTO> CommonData::getFileInfosBySSID(const QString &ssid, int pageSize, int pageNum) {
    if (!_enable) {
        LOG_ERROR("Please init cur user info!");
        return {};
    }
    return fileService->getFileByUserSSID(ssid, pageSize, pageNum);
}

bool CommonData::setFileInfo(const FileStorageDTO &fileInfo) {
    if (!_enable) {
        LOG_ERROR("Please init cur user info!");
        return false;
    }
    return fileService->addFile(fileInfo);
}


void CommonData::addAvatarToData() {
    if (!_enable) {
        LOG_ERROR("Please init cur user info!");
        return ;
    }
}

std::string CommonData::getAvatarPathFromData(const QString &ssid) const {
    // get data from sqlite

    return "";
}

bool CommonData::addCollectionEmoji(const CollectedStickerDTO &dto) {
    if (!_enable) {
        LOG_ERROR("Please init cur user info!");
        return false;
    }
    return stickerService->addCollectedSticker(dto);
}

QList<CollectedStickerDTO> CommonData::getCollectionEmoji(int pageSize, int pageNum) {
    if (!_enable) {
        LOG_ERROR("Please init cur user info!");
        return {};
    }
    return stickerService->getCollectedStickers(_userInfo.ssid,pageSize, pageNum);
}

QList<BaseStickerDTO> CommonData::getBaseEmoji(int pageSize, int pageNum) {
    if (!_enable) {
        LOG_ERROR("Please init cur user info!");
        return {};
    }
    return stickerService->getBasedStickers(pageSize, pageNum);
}

bool CommonData::initUserDatabase() {
    const char* tables[] = {
        // 用户基本信息
        "CREATE TABLE IF NOT EXISTS user_base_info ("
        "  ssid TEXT PRIMARY KEY NOT NULL,"
        "  ssname TEXT NOT NULL,"
        "  avatar TEXT,"
        "  sex TEXT NOT NULL CHECK(sex IN ('M', 'F')),"
        "  personal_sign TEXT DEFAULT '',"
        "  thumb_up_count INTEGER DEFAULT 0,"
        "  birthday TEXT,"
        "  create_time TEXT NOT NULL,"
        "  region INTEGER NOT NULL DEFAULT 2"
        ");",

        // 好友关系表
        "CREATE TABLE IF NOT EXISTS friendship ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  ssid TEXT NOT NULL,"
        "  grouping TEXT NOT NULL,"
        "  friend_ssid TEXT NOT NULL,"
        "  ship_status INTEGER NOT NULL,"
        "  friend_type INTEGER DEFAULT 1,"
        "  create_time TEXT NOT NULL,"
        "  UNIQUE(ssid, friend_ssid)"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_friend_ssid ON friendship(friend_ssid);",

        // 群组基本信息
        "CREATE TABLE IF NOT EXISTS group_base_info ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  ssid_group TEXT NOT NULL UNIQUE,"
        "  name TEXT NOT NULL,"
        "  avatar TEXT,"
        "  create_ssid TEXT NOT NULL,"
        "  profile TEXT DEFAULT '',"
        "  create_time TEXT NOT NULL"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_create_ssid ON group_base_info(create_ssid);",

        // 群成员信息
        "CREATE TABLE IF NOT EXISTS group_member_info ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  ssid_group TEXT NOT NULL,"
        "  ssid_member TEXT NOT NULL,"
        "  create_time TEXT NOT NULL,"
        "  UNIQUE(ssid_group, ssid_member)"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_member ON group_member_info(ssid_member);",

        // 消息内容表
        "CREATE TABLE IF NOT EXISTS message_content ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  sender_ssid TEXT NOT NULL,"
        "  content_type INTEGER NOT NULL,"
        "  content TEXT NOT NULL,"
        "  create_time TEXT NOT NULL"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_sender ON message_content(sender_ssid);",

        // 消息文件内容表
        "CREATE TABLE IF NOT EXISTS message_file ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  message_id INTEGER NOT NULL,"
        "  file_id TEXT NOT NULL,"
        "  sequence INTEGER NOT NULL,"
        "  FOREIGN KEY (message_id) REFERENCES message_content(id) ON DELETE CASCADE"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_message_id ON message_file(message_id);",

        // 消息接收表
        "CREATE TABLE IF NOT EXISTS message_recipient ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  message_id INTEGER NOT NULL,"
        "  recipient_type INTEGER NOT NULL,"
        "  recipient_ssid TEXT NOT NULL,"
        "  read_status INTEGER DEFAULT 0,"
        "  FOREIGN KEY(message_id) REFERENCES message_content(id) ON DELETE CASCADE"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_recipient ON message_recipient(recipient_ssid, recipient_type);"

        "CREATE TABLE IF NOT EXISTS base_stickers ("
        "  sticker_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  image_url TEXT NOT NULL UNIQUE,"
        "  created_at TEXT NOT NULL"
        ");",

        // 用户收藏表情表（合并基础表情和自定义表情）
        "CREATE TABLE IF NOT EXISTS user_collected_stickers ("
        "  user_ssid TEXT NOT NULL,"
        "  image_url TEXT NOT NULL,"
        "  is_custom INTEGER DEFAULT 0 CHECK(is_custom IN (0, 1))," // 0-基础表情，1-自定义
        "  created_at TEXT NOT NULL,"
        "  PRIMARY KEY (user_ssid, image_url),"
        "  FOREIGN KEY (user_ssid) REFERENCES user_base_info(ssid) ON DELETE CASCADE"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_user_collect ON user_collected_stickers(user_ssid);",

        "CREATE TABLE IF NOT EXISTS file_storage ("
        "   file_id TEXT PRIMARY KEY NOT NULL,"
        "   uploader_ssid TEXT NOT NULL,"
        "   file_name TEXT NOT NULL,"
        "   file_size INTEGER NOT NULL,"
        "   file_type TEXT NOT NULL,"
        "   storage_path TEXT NOT NULL,"
        "   upload_time TEXT NOT NULL"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_file_id ON file_storage(file_id);",
    };

    for (auto table : tables) {
        if (!_liteConn->update(table,{})) {
            LOG("occur error in init sqlite table then terminate execute" )
            return false;
        }
    }
    return true;
}

void CommonData::init() {
    if (!_enable) {
        _enable = true;

        initCurUserInfoDir();
        // _liteConn               =   new LiteConn((getDataPath(db) + "/" + _liteDBName).c_str(),_node["sqlite-info"]["accessKey"].as<std::string>());
        _liteConn               =   new LiteConn((getDataPath(db) + "/" + _liteDBName).c_str());
        userService             =   new UserService(*_liteConn);
        stickerService          =   new StickerService(*_liteConn);
        messageContentService   =   new MessageContentService(*_liteConn);
        groupMemberService      =   new GroupMemberService(*_liteConn);
        groupInfoService        =   new GroupInfoService(*_liteConn);
        friendshipService       =   new FriendshipService(*_liteConn);
        fileService             =   new FileService(*_liteConn);
        initUserDatabase();
    }
}

CommonData::CommonData(){
    _yamlPath = "../../conf/clientInfo.yaml";
    _imageEx  = ".jpg";

    _node = YAML::LoadFile(_yamlPath);
    _liteDBName = _node["sqlite-info"]["dbName"].as<std::string>();
    _enable = false;

    _cacheMaxSize = 20;

    userInfoCacheMap.setMaxCost(_cacheMaxSize);
    groupInfoCacheMap.setMaxCost(_cacheMaxSize);
    groupMemberInfoCacheMap.setMaxCost(_cacheMaxSize);

    // login record
    _loginDB                =   new LiteConn("loginCache.db");
    loginRecordService      =   new LoginRecordService(*_loginDB);

    std::string loginRecordsSql =
        "CREATE TABLE IF NOT EXISTS login_record ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  account VARCHAR(64) NOT NULL,"
        "  encrypted_password VARCHAR(256) NOT NULL,"
        "  login_time TEXT NOT NULL,"
        "  device_info TEXT"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_user_collect ON login_record(account);";
    _loginDB->update(loginRecordsSql,{});
}

CommonData::~CommonData() {}