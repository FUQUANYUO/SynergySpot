//
// Created by FU-QAQ on 2025/4/5.
//

#ifndef GENERATEUSERSIG_H
#define GENERATEUSERSIG_H

#include <string>
#include <vector>
#include <stdint.h>

#define EXPIRETIME 3600 // 1 hour

class GenerateUserSig {
public:
    GenerateUserSig();
    ~GenerateUserSig();
    static const char* genUserSig(const char* userId, int appId, const char* token);
};



#endif //GENERATEUSERSIG_H
