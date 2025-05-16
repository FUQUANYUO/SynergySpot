//
// Created by FU-QAQ on 2024/4/18.
//

#ifndef SYNERGYSPOT_EMAILVERIFY_H
#define SYNERGYSPOT_EMAILVERIFY_H

#include <QObject>
#include "get-time/GetCurTime.h"

#define g_pEmailVerify EmailVerify::getInstance()

class EmailVerify : public QObject {
    Q_OBJECT
public:
    static EmailVerify *getInstance();
    static void destroyInstance();

    // 发送邮箱验证码（不能并发进行发生或解析验证码）
    void sendEmailVerifyCode(const std::string &emailAddress, const std::string& queryTime, std::string ssid = "-1");

    // 获取有效期时间
    std::string getStartTime();
    std::string getValidTime();

    // 设置有效期时间
    void setStartTime(std::string time);
    void setValidTime(std::string time);

private:
    EmailVerify(QObject *bobj = nullptr);
    ~EmailVerify() = default;

private:
    // 获取请求的时刻
    std::string startTime;
    // 验证码有效时长
    std::string validTime;

    static EmailVerify *_instance;
};

#endif//SYNERGYSPOT_EMAILVERIFY_H
