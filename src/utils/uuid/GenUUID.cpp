//
// Created by FUQAQ on 2025/2/27.
//

#include "GenUUID.h"
#include <mutex>
#include <random>

GenUUID * GenUUID::instance = nullptr;
static std::mutex m;


GenUUID *GenUUID::getInstance() {
    if (instance == nullptr) {
        m.lock();
        if (instance == nullptr) {
            instance = new GenUUID;
        }
        m.unlock();
    }
    return instance;
}
void GenUUID::destroyInstance() {
    if (instance != nullptr) {
        m.lock();
        if (instance != nullptr) {
            delete instance;
        }
        m.unlock();
    }
}
std::string GenUUID::generateUUID(std::string prefix) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);

    const char* hex_chars = "0123456789abcdef";
    std::string uuid(18, '-');

    for (size_t i=0; i<18; ++i) {
        switch(i) {
            case 5: case 12:
                break;
            default:
                uuid[i] = hex_chars[dis(gen) % 16];
        }
    }
    return prefix + "_" + uuid + "_" + GetCurTime::getTimeObj()->getCurTime("%Y%m%d%H%M%S");
}

GenUUID::GenUUID() {

}

GenUUID::~GenUUID() {

}

