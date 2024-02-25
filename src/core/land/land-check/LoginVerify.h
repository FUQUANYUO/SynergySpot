//
// Created by FU-QAQ on 2024/2/25.
//

#ifndef SYNERGYSPOT_LOGINVERIFY_H
#define SYNERGYSPOT_LOGINVERIFY_H

#include "net-work/ClientConServer.h"

class LoginVerify : public QObject{
    Q_OBJECT
public:
    explicit LoginVerify() = default;
    ~LoginVerify() = default;
    void verifyInServer(std::string ssid,std::string password);
signals:
    void LAND_SUCCESS();
    void LAND_FAIL();
private:
    ClientConServer _ccons;
};


#endif//SYNERGYSPOT_LOGINVERIFY_H
