// GenerateUserSig.cpp
#include "GenerateUserSig.h"
#include <stdio.h>
#include <zlib.h>
#include <ctime>
#include <memory>
#include <cassert>
#include <cstring>
#include <openssl/hmac.h>
#include <openssl/sha.h>

class GenerateUserSigImpl {
public:
    GenerateUserSigImpl(uint32_t SDKAppID,
                        const std::string& secretKey,
                        uint32_t currTime,
                        uint32_t expireTime);
    ~GenerateUserSigImpl();

    std::string genUserSig(const std::string& userId);
private:
    std::string genHMACSHA256(const std::string& userId);
    void base64Enc(char* dest, const void* src, uint32_t length);
    char bit6ToAscii(uint8_t a);
private:
    const uint32_t          m_SDKAppID;
    const uint32_t          m_currTime;
    const uint32_t          m_expireTime;
    const std::string       m_secretKey;
};

GenerateUserSigImpl::GenerateUserSigImpl(uint32_t SDKAppID,
                                         const std::string& secretKey,
                                         uint32_t currTime,
                                         uint32_t expireTime)
    : m_SDKAppID(SDKAppID)
    , m_currTime(currTime)
    , m_expireTime(expireTime)
    , m_secretKey(secretKey) {
    assert(m_SDKAppID > 0);
    assert(m_currTime > 0);
    assert(m_expireTime > 0);
    assert(!m_secretKey.empty());
}

GenerateUserSigImpl::~GenerateUserSigImpl() = default;

std::string GenerateUserSigImpl::genUserSig(const std::string& userId) {
    assert(!userId.empty());

    std::string sig = genHMACSHA256(userId);
    size_t dataBufferLen = userId.size() + 256;
    std::unique_ptr<char[]> dataBuffer(new char[dataBufferLen]);

    int count = snprintf(dataBuffer.get(), dataBufferLen,
        "{"
        "\"TLS.ver\":\"2.0\","
        "\"TLS.identifier\":\"%s\","
        "\"TLS.sdkappid\":%u,"
        "\"TLS.expire\":%u,"
        "\"TLS.time\":%u,"
        "\"TLS.sig\":\"%s\""
        "}", userId.c_str(), m_SDKAppID, m_expireTime, m_currTime, sig.c_str());

    if (count < 0 || static_cast<size_t>(count) >= dataBufferLen) {
        return "";
    }

    uLong upperBound = compressBound(count);
    std::unique_ptr<Bytef[]> zipDest(new Bytef[count]);
    uLongf zipDestLen = count;
    int ret = compress2(zipDest.get(), &zipDestLen,
                      (const Bytef*)dataBuffer.get(), count, Z_BEST_SPEED);
    if (ret != Z_OK) {
        return "";
    }

    size_t base64Len = (zipDestLen / 3 + 1) * 4 + 1;
    std::unique_ptr<char[]> base64Buffer(new char[base64Len]);
    base64Enc(base64Buffer.get(), zipDest.get(), zipDestLen);

    std::string result(base64Buffer.get());
    for (auto& c : result) {
        switch (c) {
            case '+': c = '*'; break;
            case '/': c = '-'; break;
            case '=': c = '_'; break;
            default: break;
        }
    }

    return result;
}

std::string GenerateUserSigImpl::genHMACSHA256(const std::string& userId) {
    size_t dataBufferLen = userId.size() + 256;
    std::unique_ptr<char[]> dataBuffer(new char[dataBufferLen]);

    int count = snprintf(dataBuffer.get(), dataBufferLen,
        "TLS.identifier:%s\n"
        "TLS.sdkappid:%u\n"
        "TLS.time:%u\n"
        "TLS.expire:%u\n",
        userId.c_str(), m_SDKAppID, m_currTime, m_expireTime);

    if (count < 0 || static_cast<size_t>(count) >= dataBufferLen) {
        return "";
    }

    std::string data(dataBuffer.get(), count);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    unsigned int hashLen = 0;

    HMAC_CTX* ctx = HMAC_CTX_new();
    HMAC_Init_ex(ctx,
                reinterpret_cast<const unsigned char*>(m_secretKey.data()),
                m_secretKey.size(),
                EVP_sha256(),
                nullptr);
    HMAC_Update(ctx,
                reinterpret_cast<const unsigned char*>(data.data()),
                data.size());
    HMAC_Final(ctx, hash, &hashLen);
    HMAC_CTX_free(ctx);

    size_t base64Len = (hashLen / 3 + 1) * 4 + 1;
    std::unique_ptr<char[]> base64Buffer(new char[base64Len]);
    base64Enc(base64Buffer.get(), hash, hashLen);

    return std::string(base64Buffer.get());
}

void GenerateUserSigImpl::base64Enc(char* dest, const void* src, uint32_t length) {
    const uint8_t* data = reinterpret_cast<const uint8_t*>(src);
    uint32_t i = 0;
    uint32_t j = 0;
    uint8_t a[4] = {0};

    for (i = 0; i < length / 3; ++i) {
        a[0] = (data[i*3+0] & 0xFC) >> 2;
        a[1] = ((data[i*3+0] & 0x03) << 4) + ((data[i*3+1] & 0xF0) >> 4);
        a[2] = ((data[i*3+1] & 0x0F) << 2) + ((data[i*3+2] & 0xC0) >> 6);
        a[3] = data[i*3+2] & 0x3F;

        for (j = 0; j < 4; ++j) {
            *dest++ = bit6ToAscii(a[j]);
        }
    }

    // 处理剩余字节
    switch (length % 3) {
        case 1:
            a[0] = (data[i*3+0] & 0xFC) >> 2;
            a[1] = (data[i*3+0] & 0x03) << 4;
            *dest++ = bit6ToAscii(a[0]);
            *dest++ = bit6ToAscii(a[1]);
            *dest++ = '=';
            *dest++ = '=';
            break;
        case 2:
            a[0] = (data[i*3+0] & 0xFC) >> 2;
            a[1] = ((data[i*3+0] & 0x03) << 4) + ((data[i*3+1] & 0xF0) >> 4);
            a[2] = (data[i*3+1] & 0x0F) << 2;
            *dest++ = bit6ToAscii(a[0]);
            *dest++ = bit6ToAscii(a[1]);
            *dest++ = bit6ToAscii(a[2]);
            *dest++ = '=';
            break;
    }

    *dest = '\0';
}

char GenerateUserSigImpl::bit6ToAscii(uint8_t a) {
    a &= 0x3F;
    if (a < 26) return a + 'A';
    if (a < 52) return a - 26 + 'a';
    if (a < 62) return a - 52 + '0';
    return a == 62 ? '+' : '/';
}

// GenerateUserSig 实现
GenerateUserSig::GenerateUserSig() = default;
GenerateUserSig::~GenerateUserSig() = default;

const char* GenerateUserSig::genUserSig(const char* userId, int appid, const char* token) {
    uint32_t currTime = static_cast<uint32_t>(time(nullptr));
    GenerateUserSigImpl impl(appid, token, currTime, EXPIRETIME);

    std::string usersig_str = impl.genUserSig(userId);
    char* usersig_cstr = new char[usersig_str.size() + 1];
    strcpy(usersig_cstr, usersig_str.c_str());

    return usersig_cstr;
}