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
    std::vector<FileStorageDTO> getFileByUserSSID(const std::string& userSSID,int pageSize, int pageNum);

    std::vector<FileStorageDTO> getFileByFileName(const std::string& fileName,int pageSize, int pageNum);

    FileStorageDTO getFileByFileID(const std::string& fileID);

    FileStorageDTO getFileByFilePath(const std::string& filePath);

private:
    FileStorageDAO fileDAO;
};



#endif //FILESERVICE_H
