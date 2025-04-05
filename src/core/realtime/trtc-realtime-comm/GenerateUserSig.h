//
// Created by FU-QAQ on 2025/4/5.
//

#ifndef GENERATEUSERSIG_H
#define GENERATEUSERSIG_H

#include <string>
#include <vector>
#include <stdint.h>

#define EXPIRETIME 604800 // 7 day

class GenerateUserSig {
public:
    GenerateUserSig();
    ~GenerateUserSig();
    static const char* genUserSig(const char* identifier, int appId, const char* secretKey);
};



#endif //GENERATEUSERSIG_H
