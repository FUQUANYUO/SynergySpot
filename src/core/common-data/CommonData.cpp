//
// Created by FUQAQ on 2025/1/21.
//

#include "CommonData.h"
#include "help.h"

#include "db-lite/LiteConn.h"

#include <QDir>
#include <QImage>

#include <mutex>

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

CurUserInfoDataStruct CommonData::getCurUserInfo() const {
    return _userInfo;
}

void CommonData::setCurUserInfo(const CurUserInfoDataStruct &curUserInfo) {
    _userInfo = curUserInfo;
}

bool CommonData::initCurUserInfoDir() {
    do {
        QString userDir = QString::fromStdString(_userInfo.CurSSID);
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
    std::string path = _userInfo.CurSSID;
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


void CommonData::addFileToTmp() {}

std::string CommonData::getFilePathFromTmp() {
    return "";
}

void CommonData::addAvatarToData() {}

std::string CommonData::getAvatarPathFromData(const QString &ssid) const {
    // get data from sqlite

    return "";
}

void CommonData::addEmojiToData() {}

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
        "  friend_ssid TEXT NOT NULL,"
        "  ship_status INTEGER NOT NULL,"
        "  friend_type INTEGER DEFAULT 1,"
        "  create_time TEXT DEFAULT CURRENT_TIMESTAMP,"
        "  UNIQUE(ssid, friend_ssid)"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_friend_ssid ON friendship(friend_ssid);",

        // 群组基本信息
        "CREATE TABLE IF NOT EXISTS group_base_info ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  ssid_group TEXT NOT NULL UNIQUE,"
        "  name TEXT NOT NULL,"
        "  create_ssid TEXT NOT NULL,"
        "  profile TEXT DEFAULT '',"
        "  create_time TEXT DEFAULT CURRENT_TIMESTAMP"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_create_ssid ON group_base_info(create_ssid);",

        // 群成员信息
        "CREATE TABLE IF NOT EXISTS group_member_info ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  ssid_group TEXT NOT NULL,"
        "  ssid_member TEXT NOT NULL,"
        "  create_time TEXT DEFAULT CURRENT_TIMESTAMP,"
        "  UNIQUE(ssid_group, ssid_member)"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_member ON group_member_info(ssid_member);",

        // 消息内容表
        "CREATE TABLE IF NOT EXISTS message_content ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  sender_ssid TEXT NOT NULL,"
        "  content_type INTEGER NOT NULL,"
        "  content TEXT NOT NULL,"
        "  file_id TEXT,"
        "  create_time TEXT DEFAULT CURRENT_TIMESTAMP"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_sender ON message_content(sender_ssid);",

        // 消息接收表
        "CREATE TABLE IF NOT EXISTS message_recipient ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  message_id INTEGER NOT NULL,"
        "  recipient_type INTEGER NOT NULL,"
        "  recipient_ssid TEXT NOT NULL,"
        "  read_status INTEGER DEFAULT 0"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_recipient ON message_recipient(recipient_ssid, recipient_type);"

        "CREATE TABLE IF NOT EXISTS base_stickers ("
        "  sticker_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  image_url TEXT NOT NULL UNIQUE,"
        "  created_at TEXT DEFAULT CURRENT_TIMESTAMP"
        ");",

        // 用户收藏表情表（合并基础表情和自定义表情）
        "CREATE TABLE IF NOT EXISTS user_collected_stickers ("
        "  user_ssid TEXT NOT NULL,"
        "  image_url TEXT NOT NULL,"
        "  is_custom INTEGER DEFAULT 0 CHECK(is_custom IN (0, 1))," // 0-基础表情，1-自定义
        "  created_at TEXT DEFAULT CURRENT_TIMESTAMP,"
        "  PRIMARY KEY (user_ssid, image_url),"
        "  FOREIGN KEY (user_ssid) REFERENCES user_base_info(ssid) ON DELETE CASCADE"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_user_collect ON user_collected_stickers(user_ssid);"
    };

    LiteConn liteDB((getDataPath(db) + "/" + _liteDBName).c_str());
    for (auto table : tables) {
        if (!liteDB.update(table,{})) {
            LOG("occur error in init sqlite table then terminate execute" )
            return false;
        }
    }
    return true;
}

CommonData::CommonData(){
    _yamlPath = "../../conf/clientInfo.yaml";
    _imageEx  = ".jpg";

     _node = YAML::LoadFile(_yamlPath);
     _liteDBName = _node["sqlite-info"]["dbName"].as<std::string>();
}

CommonData::~CommonData() {}