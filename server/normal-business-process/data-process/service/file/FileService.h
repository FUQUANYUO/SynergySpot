//
// Created by FUQAQ on 2025/2/6.
//

#ifndef FILESERVICE_H
#define FILESERVICE_H

#include "../../dao/file/FileDAO.h"
#include "../../dto/DataProcessDTO.h"

class FileService {
public:
    // add file
    bool addFile(const FileStorageDTO& dto);

    // remove file
    bool removeFile(const FileStorageDTO& dto);

    // get file
    std::string getFile(const FileStorageDTO& dto);
private:
    FileStorageDAO fileDAO;
};



#endif //FILESERVICE_H
