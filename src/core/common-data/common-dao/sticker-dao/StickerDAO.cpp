//
// Created by FUQAQ on 2025/1/26.
//

#include "StickerDAO.h"

StickerDAO::StickerDAO(LiteConn &db) : _db(db) {}

StickerDAO::~StickerDAO() {}

qint64 StickerDAO::insertBaseSticker(const BaseStickerDO &sticker) {
    std::string sql = "INSERT INTO base_stickers (image_url) VALUES (?);";
    std::vector<std::string> params = { sticker.imageUrl.toStdString() };

    if (_db.update(sql, params)) {
        // 获取最后插入的ID
        std::string lastIdSql = "SELECT last_insert_rowid();";
        auto result = _db.query(lastIdSql, {});
        if (!result.empty() && !result[0].empty()) {
            return std::stoll(result[0][0]);
        }
    }
    return -1;
}

bool StickerDAO::deleteBaseSticker(qint64 stickerId) {
    std::string sql = "DELETE FROM base_stickers WHERE sticker_id = ?;";
    std::vector<std::string> params = { std::to_string(stickerId) };

    return _db.update(sql, params);
}

QList<BaseStickerDO> StickerDAO::listBaseStickers(int pageSize, int pageNum) {
    if (pageSize <= 0) pageSize = 20;
    if (pageNum <= 0) pageNum = 1;
    const int offset = (pageNum - 1) * pageSize;

    std::string sql = "SELECT sticker_id , image_url FROM base_stickers ORDER BY id DESC LIMIT ? OFFSET ?;";
    auto result = _db.query(sql, {std::to_string(pageSize), std::to_string(offset)});
    QList<BaseStickerDO> stickers;
    for (const auto &row : result) {
        BaseStickerDO sticker;
        sticker.stickerId = std::stoll(row[0]);
        sticker.imageUrl = QString::fromStdString(row[1]);
        stickers.append(sticker);
    }
    return stickers;
}

int StickerDAO::getBasedStickerCount() {
    std::string sql = "SELECT COUNT(*) FROM base_stickers";
    std::vector<std::string> params = {};

    auto result = _db.query(sql, params);

    if (!result.empty() && !result[0].empty()) {
        return std::stoi(result[0][0]);
    }else {
        return 0;
    }
}

bool StickerDAO::insertCollectSticker(const CollectedStickerDO &sticker) {
    std::string sql = "INSERT INTO user_collected_stickers (user_ssid, image_url, is_custom) "
                  "VALUES (?, ?, ?);";
    std::vector<std::string> params = {
        sticker.userSSID.toStdString(),
        sticker.imageUrl.toStdString(),
        sticker.isCustom ? "1" : "0" // 布尔值转换为 "1" 或 "0"
    };

    return _db.update(sql, params);
}

bool StickerDAO::removeCollectedSticker(const QString &userSsid, const QString &imageUrl) {
    std::string sql = "DELETE FROM user_collected_stickers WHERE user_ssid = ? AND image_url = ?;";
    std::vector<std::string> params = {
        userSsid.toStdString(),
        imageUrl.toStdString()
    };

    return _db.update(sql, params);
}

QList<CollectedStickerDO> StickerDAO::listCollectedStickers(const QString &userSsid, int pageSize, int pageNum) {
    if (pageSize <= 0) pageSize = 20;
    if (pageNum <= 0) pageNum = 1;
    const int offset = (pageNum - 1) * pageSize;

    std::string sql = "SELECT user_ssid, image_url, is_custom FROM user_collected_stickers WHERE user_ssid = ? ORDER BY id DESC LIMIT ? OFFSET ?;";
    std::vector<std::string> params = {
        userSsid.toStdString(),
        std::to_string(pageSize),
        std::to_string(offset)
    };
    auto result = _db.query(sql, params);
    QList<CollectedStickerDO> stickers;
    for (const auto &row : result) {
        CollectedStickerDO sticker;
        sticker.userSSID = QString::fromStdString(row[0]);
        sticker.imageUrl = QString::fromStdString(row[1]);
        sticker.isCustom = (row[2] == "1"); // "1" 转换为 true，否则 false
        stickers.append(sticker);
    }
    return stickers;
}

int StickerDAO::getCollectedStickerCount(const QString &ssid) {
    std::string sql = "SELECT COUNT(*) FROM user_collected_stickers WHERE user_ssid = ?";
    std::vector<std::string> params = { ssid.toStdString() };

    auto result = _db.query(sql, params);

    if (!result.empty() && !result[0].empty()) {
        return std::stoi(result[0][0]);
    }else {
        return 0;
    }
}