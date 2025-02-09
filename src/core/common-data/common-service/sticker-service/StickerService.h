//
// Created by FUQAQ on 2025/1/27.
//

#ifndef STICKERSERVICE_H
#define STICKERSERVICE_H

#include "../../common-dto/CommonDatabaseDTO.h"
#include "../../common-dao/sticker-dao/StickerDAO.h"

class StickerService {
public:
    explicit StickerService(LiteConn& db);
    QList<BaseStickerDTO> getBasedStickers(int pageSize = 20, int pageNum = 1);
    QList<CollectedStickerDTO> getCollectedStickers(const QString& ssid, int pageSize = 20, int pageNum = 1);

    bool setBasedStickerData(QList<BaseStickerDTO> dto);
    bool setCollectedStickerData(QList<CollectedStickerDTO> dto);

    bool addBasedSticker(const BaseStickerDTO& dto);
    bool removeBasedSticker(const BaseStickerDTO& dto);

    bool addCollectedSticker(const CollectedStickerDTO& dto);
    bool removeCollectedSticker(const CollectedStickerDTO& dto);

    int getBasedStickerCount();
    int getCollectedStickerCount(const QString& ssid);
private:
    StickerDAO stickerDAO;
};



#endif //STICKERSERVICE_H
