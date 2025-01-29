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
    QList<BaseStickerDTO> getBasedStickers();
    QList<CollectedStickerDTO> getCollectedStickers(const QString& ssid);

    bool addBasedSticker(const BaseStickerDTO& dto);
    bool removeBasedSticker(const BaseStickerDTO& dto);

    bool addCollectedSticker(const CollectedStickerDTO& dto);
    bool removeCollectedSticker(const CollectedStickerDTO& dto);
private:
    StickerDAO stickerDAO;
};



#endif //STICKERSERVICE_H
