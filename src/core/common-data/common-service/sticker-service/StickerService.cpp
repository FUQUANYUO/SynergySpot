//
// Created by FUQAQ on 2025/1/27.
//

#include "StickerService.h"

StickerService::StickerService(LiteConn &db) : stickerDAO(db) {}

QList<BaseStickerDTO> StickerService::getBasedStickers(int pageSize, int pageNum) {
    QList<BaseStickerDTO> stickers;
    QList<BaseStickerDO> result = stickerDAO.listBaseStickers(pageSize, pageNum);
    for (int i = 0; i < result.size(); i++) {
        stickers.append({result[i].stickerId, result[i].imageUrl});
    }
    return stickers;
}

QList<CollectedStickerDTO> StickerService::getCollectedStickers(const QString& ssid, int pageSize, int pageNum) {
    QList<CollectedStickerDTO> stickers;
    QList<CollectedStickerDO> result = stickerDAO.listCollectedStickers(ssid,pageSize, pageNum);
    for (int i = 0; i < result.size(); i++) {
        stickers.append({ssid, result[i].imageUrl,result[i].isCustom,result[i].createdAt});
    }
    return stickers;
}

bool StickerService::setBasedStickerData(QList<BaseStickerDTO> dto) {
    bool res = true;
    for (auto it : dto) {
        res = stickerDAO.insertBaseSticker({it.stickerId, it.imageUrl});
        if (!res)
            return res;
    }
    return res;
}

bool StickerService::setCollectedStickerData(QList<CollectedStickerDTO> dto) {
    bool res = true;
    for (auto it : dto) {
        res = stickerDAO.insertCollectSticker({it.userSSID, it.imageUrl, it.isCustom, it.createdAt});
        if (!res)
            return res;
    }
    return res;
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

int StickerService::getBasedStickerCount() {
    return stickerDAO.getBasedStickerCount();
}

int StickerService::getCollectedStickerCount(const QString& ssid) {
    return stickerDAO.getCollectedStickerCount(ssid);
}