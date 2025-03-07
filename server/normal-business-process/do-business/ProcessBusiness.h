//
// Created by FUQAQ on 2025/3/7.
//

#ifndef PROCESSBUSINESS_H
#define PROCESSBUSINESS_H

#include "GeneralServer.h"

class ProcessBusiness {
public:
    ProcessBusiness() = default;
    ProcessBusiness(int fd, EpollEngine *en);

    bool parseCompleteRequest(std::vector<char>& buffer, std::string &dto, int &businessType);
    int processBusiness(std::string dto, int businessType, std::shared_ptr<SockInfo> info);
private:
    int _fd;
    EpollEngine *en;
};



#endif //PROCESSBUSINESS_H
