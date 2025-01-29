//
// Created by FUQAQ on 2025/1/27.
//

#include "StickerService.h"

StickerService::StickerService(LiteConn &db) : stickerDAO(db) {}

QList<BaseStickerDTO> StickerService::getBasedStickers() {
    QList<BaseStickerDTO> stickers;
    QList<BaseStickerDO> result = stickerDAO.listBaseStickers();
    for (int i = 0; i < result.size(); i++) {
        stickers.append({result[i].stickerId, result[i].imageUrl});
    }
    return stickers;
}

QList<CollectedStickerDTO> StickerService::getCollectedStickers(const QString& ssid) {
    QList<CollectedStickerDTO> stickers;
    QList<CollectedStickerDO> result = stickerDAO.listCollectedStickers(ssid);
    for (int i = 0; i < result.size(); i++) {
        stickers.append({ssid, result[i].imageUrl,result[i].isCustom,result[i].createdAt});
    }
    return stickers;
}

bool StickerService::addBasedSticker(const BaseStickerDTO &dto) {
    return stickerDAO.insertBaseSticker({dto.stickerId,dto.imageUrl});
}

bool StickerService::removeBasedSticker(const BaseStickerDTO &dto) {
    return stickerDAO.deleteBaseSticker({dto.stickerId});
}

bool StickerService::addCollectedSticker(const CollectedStickerDTO &dto) {
    return stickerDAO.insertCollectSticker({dto.userSSID,dto.imageUrl,dto.isCustom,dto.createdAt});
}

bool StickerService::removeCollectedSticker(const CollectedStickerDTO &dto) {
    return stickerDAO.removeCollectedSticker(dto.userSSID,dto.imageUrl);
}