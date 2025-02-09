//
// Created by FU-QAQ on 2024/5/3.
//
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/err.h>
#include <sstream>
#include <iomanip>
#include <random>

#include "EncryptString.h"


// 打印错误
void handleError(){
    LOG("ERROR : " << ERR_error_string(ERR_get_error(), nullptr))
}
EncryptString *EncryptString::obj = nullptr;
std::string EncryptString::encryptStringByMD5(const std::string& str) {
    // md5
    unsigned char encryptArr[MD5_DIGEST_LENGTH];
    MD5(reinterpret_cast<const unsigned char*>(str.c_str()), str.length(), encryptArr);
    std::stringstream ss;
    for(unsigned char i : encryptArr){
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(i);
    }
    return ss.str();
}

std::string EncryptString::encryptStringBySHA256(const std::string& str,const std::string& passwordSalt) {
    // Combine the input string and the password salt
    std::string data = str + passwordSalt;

    // Create a SHA-256 context
    SHA256_CTX ctx;
    SHA256_Init(&ctx);

    // Update with the data
    SHA256_Update(&ctx, data.c_str(), data.size());

    // Finish the SHA-256 hash
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &ctx);

    // Convert the hash to a hexadecimal string
    std::stringstream hexStream;
    hexStream << std::hex << std::setfill('0');
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        hexStream << std::setw(2) << static_cast<int>(hash[i]);
    }

    return hexStream.str();
}

std::string EncryptString::generatePasswordSalt(size_t length) {
    const std::string charset =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, charset.size() - 1); // 均匀分布

    std::string salt;
    salt.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        salt += charset[dis(gen)];
    }

    return salt;
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
