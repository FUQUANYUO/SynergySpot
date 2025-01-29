//
// Created by FUQAQ on 2025/1/26.
//

#include "StickerDAO.h"

StickerDAO::StickerDAO(LiteConn &db) : _db(db) {}

StickerDAO::~StickerDAO() {}

qint64 StickerDAO::insertBaseSticker(const BaseStickerDO &sticker) {
    std::string sql = "INSERT INTO base_stickers (imageUrl) VALUES (?);";
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
    std::string sql = "DELETE FROM base_stickers WHERE stickerId = ?;";
    std::vector<std::string> params = { std::to_string(stickerId) };

    return _db.update(sql, params);
}

QList<BaseStickerDO> StickerDAO::listBaseStickers() {
    std::string sql = "SELECT sticker_id , image_url FROM base_stickers;";

    auto result = _db.query(sql, {});
    QList<BaseStickerDO> stickers;
    for (const auto &row : result) {
        BaseStickerDO sticker;
        sticker.stickerId = std::stoll(row[0]);
        sticker.imageUrl = QString::fromStdString(row[1]);
        stickers.append(sticker);
    }
    return stickers;
}

bool StickerDAO::insertCollectSticker(const CollectedStickerDO &sticker) {
    std::string sql = "INSERT INTO user_collected_stickers (userSSID, imageUrl, isCustom) "
                  "VALUES (?, ?, ?);";
    std::vector<std::string> params = {
        sticker.userSSID.toStdString(),
        sticker.imageUrl.toStdString(),
        sticker.isCustom ? "1" : "0" // 布尔值转换为 "1" 或 "0"
    };

    return _db.update(sql, params);
}

bool StickerDAO::removeCollectedSticker(const QString &userSsid, const QString &imageUrl) {
    std::string sql = "DELETE FROM user_collected_stickers WHERE userSSID = ? AND imageUrl = ?;";
    std::vector<std::string> params = {
        userSsid.toStdString(),
        imageUrl.toStdString()
    };

    return _db.update(sql, params);
}

QList<CollectedStickerDO> StickerDAO::listCollectedStickers(const QString &userSsid) {
    std::string sql = "SELECT userSSID, imageUrl, isCustom FROM user_collected_stickers WHERE userSSID = ?;";
    std::vector<std::string> params = { userSsid.toStdString() };

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