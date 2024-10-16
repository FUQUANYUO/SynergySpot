//
// Created by FU-QAQ on 2024/5/3.
//
#include <openssl/md5.h>
#include <openssl/err.h>
#include <sstream>
#include <iomanip>
#include "EncryptString.h"

// 打印错误
void handleError(){
    LOG("ERROR : " << ERR_error_string(ERR_get_error(), nullptr))
}
EncryptString *EncryptString::obj = nullptr;
std::string EncryptString::encryptString(const std::string& str) {
    // md5
    unsigned char encryptArr[MD5_DIGEST_LENGTH];
    MD5(reinterpret_cast<const unsigned char*>(str.c_str()), str.length(), encryptArr);
    std::stringstream ss;
    for(unsigned char i : encryptArr){
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(i);
    }
    return ss.str();
}

EncryptString *EncryptString::getEncryptObj() {
    if(obj == nullptr){
        obj = new EncryptString;
    }
    return obj;
}

void EncryptString::destroyObj() {
    delete obj;
}
