//
// Created by FUQAQ on 2025/2/27.
//

#ifndef GENUUID_H
#define GENUUID_H

#include "help.h"

#define g_pGenUUID GenUUID::getInstance()

class GenUUID {
public:
    static GenUUID * getInstance();
    static void destroyInstance();

    std::string generateUUID(std::string prefix);
private:
    GenUUID();
    ~GenUUID();

private:
    static GenUUID *instance;
};



#endif //GENUUID_H
