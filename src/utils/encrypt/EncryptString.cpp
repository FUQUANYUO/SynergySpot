//
// Created by FU-QAQ on 2024/5/3.
//
#include <openssl/evp.h>
#include <openssl/err.h>

#include "EncryptString.h"

static std::string pubKey;
static std::string iv;

// 打印错误
void handleError(){
    LOG("ERROR : " << ERR_error_string(ERR_get_error(), nullptr))
}

EncryptString *EncryptString::obj = nullptr;

std::string EncryptString::encryptString(const std::string& str) {
    EVP_CIPHER_CTX *ctx;
    std::string encrypted;
    int len = 0;
    int ciphertext_len = 0;
    std::reverse(iv.begin()+(int)iv.size()/2,iv.end());

    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleError();
    // AES-256-CBC
    if(EVP_EncryptInit_ex(ctx,EVP_aes_256_cfb(), nullptr,
                           reinterpret_cast<const unsigned char*>(pubKey.c_str()),
                           reinterpret_cast<const unsigned char*>(iv.c_str())) != 1)
        handleError();

    // 加密最大长度
    ciphertext_len = (int)str.size() + EVP_CIPHER_CTX_block_size(ctx);
    encrypted.resize(ciphertext_len);

    // 加密
    if(EVP_EncryptUpdate(ctx,reinterpret_cast<unsigned char*>(encrypted.data())
                            ,&len,reinterpret_cast<const unsigned char*>(str.c_str()),(int)str.size()) != 1)
        handleError();
    ciphertext_len = len;

    if(EVP_EncryptFinal_ex(ctx,reinterpret_cast<unsigned char*>(encrypted.data() + len),&len) != 1)
        handleError();
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    encrypted.resize(ciphertext_len);

    return encrypted;

}

EncryptString *EncryptString::getEncryptObj(const std::string& pub_key) {
    pubKey = pub_key;
    iv = pubKey;
    if(obj == nullptr){
        obj = new EncryptString;
    }
    return obj;
}

void EncryptString::destroyObj() {
    delete obj;
}