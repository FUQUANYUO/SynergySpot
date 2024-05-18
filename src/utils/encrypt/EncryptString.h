//
// Created by FU-QAQ on 2024/5/3.
//

#ifndef SYNERGYSPOT_ENCRYPTSTRING_H
#define SYNERGYSPOT_ENCRYPTSTRING_H

#include "help.h"

class EncryptString {
public:
    static EncryptString *getEncryptObj(const std::string& pub_key = "");
    std::string encryptString(const std::string& str);

    void destroyObj();
private:
    static EncryptString * obj;
    EncryptString() = default;
    ~EncryptString() = default;
};


#endif//SYNERGYSPOT_ENCRYPTSTRING_H
