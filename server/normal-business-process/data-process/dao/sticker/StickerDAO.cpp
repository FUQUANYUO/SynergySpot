//
// Created by FUQAQ on 2025/2/6.
//

#include "StickerDAO.h"

int64_t BaseStickerDAO::insert(const BaseStickerDO &sticker) {
    std::string sql = "INSERT INTO base_stickers (image_url, tags) VALUES (?, ?)";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramImageUrl;
    paramImageUrl.type = MysqlConn::Param::STRING;
    paramImageUrl.str_val = sticker.imageUrl;
    params.push_back(paramImageUrl);

    MysqlConn::Param paramTags;
    paramTags.type = MysqlConn::Param::STRING;
    paramTags.str_val = sticker.tags;
    params.push_back(paramTags);

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to insert base sticker: " << sticker.imageUrl);
        return -1;
    }

    return static_cast<int64_t>(m_conn->getLastInsertId());
}

bool BaseStickerDAO::deleteById(int64_t stickerId) {
    std::string sql = "DELETE FROM base_stickers WHERE sticker_id = ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramId;
    paramId.type = MysqlConn::Param::INT;
    paramId.int_val = stickerId;
    params.push_back(paramId);

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to delete base sticker with ID: " << stickerId);
        return false;
    }

    return true;
}

std::vector<BaseStickerDO> BaseStickerDAO::listBaseStickers() {
    std::string sql = "SELECT sticker_id, image_url, tags, UNIX_TIMESTAMP(created_at) FROM base_stickers "
                      "ORDER BY UNIX_TIMESTAMP(created_at) DESC";

    MYSQL_RES* result = m_conn->query(sql, {});
    if (!result) {
        LOG_ERROR("Failed to list base stickers.");
        return {};
    }

    std::vector<BaseStickerDO> stickers;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        BaseStickerDO sticker;
        sticker.stickerId = std::stoll(row[0]);
        sticker.imageUrl = row[1];
        sticker.tags = row[2];
        sticker.createdAt = row[3] ? std::stoll(row[3]) : 0;
        stickers.push_back(sticker);
    }

    mysql_free_result(result);
    return stickers;
}

bool UserCollectedStickerDAO::collect(const UserCollectedStickerDO &sticker) {
    std::string sql = "INSERT INTO user_collected_stickers (user_ssid, is_custom, image_url) VALUES (?, ?, ?)";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramUserSsid;
    paramUserSsid.type = MysqlConn::Param::STRING;
    paramUserSsid.str_val = sticker.userSsid;
    params.push_back(paramUserSsid);

    MysqlConn::Param paramIsCustom;
    paramIsCustom.type = MysqlConn::Param::INT;
    paramIsCustom.int_val = sticker.isCustom;
    params.push_back(paramIsCustom);

    MysqlConn::Param paramImageUrl;
    paramImageUrl.type = MysqlConn::Param::STRING;
    paramImageUrl.str_val = sticker.imageUrl;
    params.push_back(paramImageUrl);

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to collect sticker for user: " << sticker.userSsid << ", image: " << sticker.imageUrl);
        return false;
    }

    return true;
}

bool UserCollectedStickerDAO::unCollect(const std::string &userSsid, const std::string &imageUrl) {
    std::string sql = "DELETE FROM user_collected_stickers WHERE user_ssid = ? AND image_url = ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramUserSsid;
    paramUserSsid.type = MysqlConn::Param::STRING;
    paramUserSsid.str_val = userSsid;
    params.push_back(paramUserSsid);

    MysqlConn::Param paramImageUrl;
    paramImageUrl.type = MysqlConn::Param::STRING;
    paramImageUrl.str_val = imageUrl;
    params.push_back(paramImageUrl);

    if (!m_conn->update(sql, params)) {
        LOG_ERROR("Failed to uncollect sticker for user: " << userSsid << ", image: " << imageUrl);
        return false;
    }

    return true;
}

std::vector<UserCollectedStickerDO> UserCollectedStickerDAO::listByUser(const std::string &userSsid, int pageSize, int pageNum) {
    std::string sql = "SELECT collection_id, user_ssid, is_custom, image_url, UNIX_TIMESTAMP(created_at) "
                      "FROM user_collected_stickers WHERE user_ssid = ? "
                      "ORDER BY UNIX_TIMESTAMP(created_at) DESC LIMIT ? OFFSET ?";
    std::vector<MysqlConn::Param> params;

    MysqlConn::Param paramUserSsid;
    paramUserSsid.type = MysqlConn::Param::STRING;
    paramUserSsid.str_val = userSsid;
    params.push_back(paramUserSsid);

    MysqlConn::Param paramPageSize;
    paramPageSize.type = MysqlConn::Param::INT;
    paramPageSize.int_val = pageSize;
    params.push_back(paramPageSize);

    MysqlConn::Param paramOffset;
    paramOffset.type = MysqlConn::Param::INT;
    paramOffset.int_val = (pageNum - 1) * pageSize;
    params.push_back(paramOffset);

    MYSQL_RES* result = m_conn->query(sql, params);
    if (!result) {
        LOG_ERROR("Failed to list collected stickers for user: " << userSsid);
        return {};
    }

    std::vector<UserCollectedStickerDO> stickers;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        UserCollectedStickerDO sticker;
        sticker.collectionId = std::stoll(row[0]);
        sticker.userSsid = row[1];
        sticker.isCustom = static_cast<bool>(std::stoul(row[2]));
        sticker.imageUrl = row[3];
        sticker.createdAt = row[4] ? std::stoll(row[4]) : 0;
        stickers.push_back(sticker);
    }

    mysql_free_result(result);
    return stickers;
}
