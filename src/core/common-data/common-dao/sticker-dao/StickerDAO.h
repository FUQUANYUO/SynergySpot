//
// Created by FUQAQ on 2025/1/26.
//

#ifndef STICKERDAO_H
#define STICKERDAO_H

#include "../CommonDataBaseDAO.h"

class StickerDAO : public IStickerDAO{
public:
    explicit StickerDAO(LiteConn& db);
    ~StickerDAO() override;

    // 基础表情操作
    qint64 insertBaseSticker(const BaseStickerDO& sticker) override;
    bool deleteBaseSticker(qint64 stickerId) override;
    QList<BaseStickerDO> listBaseStickers(int pageSize = 20, int pageNum = 1) override;
    int getBasedStickerCount();

    // 用户收藏操作
    bool insertCollectSticker(const CollectedStickerDO& sticker) override;
    bool removeCollectedSticker(const QString& userSsid, const QString& imageUrl) override;
    QList<CollectedStickerDO> listCollectedStickers(const QString& userSsid, int pageSize = 20, int pageNum = 1) override;
    int getCollectedStickerCount(const QString& ssid);
private:
    LiteConn& _db;
};



#endif //STICKERDAO_H
