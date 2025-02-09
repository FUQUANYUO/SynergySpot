//
// Created by FUQAQ on 2025/2/6.
//

#include "StickerService.h"


std::vector<BaseStickerDTO> StickerService::getBasedSticker() {
    std::vector<BaseStickerDTO> stickersDTO;
    std::vector<BaseStickerDO> stickers = stickerDAO.listBaseStickers();
    for (const auto &it : stickers) {
        stickersDTO.push_back({it.stickerId,it.imageUrl,it.tags,it.createdAt});
    }
    return stickersDTO;
}

std::vector<UserCollectedStickerDTO> StickerService::getUserCollectedSticker(const UserCollectedStickerDTO & dto) {
    std::vector<UserCollectedStickerDTO> stickersDTO;
    std::vector<UserCollectedStickerDO>  stickers = userCollectedStickerDAO.listByUser(dto.userSsid,dto.pageSize,dto.pageNum);
    for (const auto &it : stickers) {
        stickersDTO.push_back({it.collectionId,it.userSsid,it.isCustom,it.imageUrl,it.createdAt});
    }
    return stickersDTO;
}

bool StickerService::removeUserCollectedSticker(const std::string &ssid, const std::string &url) {
    return userCollectedStickerDAO.unCollect(ssid,url);
}