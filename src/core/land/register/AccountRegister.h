//
// Created by FU-QAQ on 2024/4/18.
//

#ifndef SYNERGYSPOT_ACCOUNTREGISTER_H
#define SYNERGYSPOT_ACCOUNTREGISTER_H

#include "net-work/ClientConServer.h"
#include "../../safety/email-check/EmailVerify.h"

class AccountRegister : public QObject{
    Q_OBJECT
public:
    AccountRegister() = default;
    AccountRegister(const std::string& emailAddress,QObject * parent = nullptr);
    ~AccountRegister();
private:
    EmailVerify *ev;
};


#endif//SYNERGYSPOT_ACCOUNTREGISTER_H
