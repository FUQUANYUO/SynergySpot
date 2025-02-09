//
// Created by FUQAQ on 2025/2/6.
//

#ifndef STICKERSERVICE_H
#define STICKERSERVICE_H

#include "../../dao/sticker/StickerDAO.h"
#include "../../dto/DataProcessDTO.h"

class StickerService {
public:
    // get based sticker
    std::vector<BaseStickerDTO> getBasedSticker();

    // get collected sticker by ssid
    std::vector<UserCollectedStickerDTO> getUserCollectedSticker(const UserCollectedStickerDTO & dto);

    // remove collected sticker
    bool removeUserCollectedSticker(const std::string& ssid, const std::string &url);
private:
    BaseStickerDAO          stickerDAO;
    UserCollectedStickerDAO userCollectedStickerDAO;
};



#endif //STICKERSERVICE_H
