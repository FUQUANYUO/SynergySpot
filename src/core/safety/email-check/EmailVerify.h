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
    void sendEmailVerifyCode(const std::string &emailAddress, const std::string& queryTime);

    // 获取信息
    std::string getStartTime();
    std::string getValidTime();

private:
    EmailVerify(QObject *bobj = nullptr);
    ~EmailVerify() = default;
public slots:
    // 解析邮箱验证码（不能并发进行发生或解析验证码）
    std::string parseEmailVerifyCode(const std::string &rawdto);

private:
    // 获取请求的时刻
    std::string startTime;
    // 验证码有效时长
    std::string validTime;

    static EmailVerify *_instance;
};

#endif//SYNERGYSPOT_EMAILVERIFY_H
