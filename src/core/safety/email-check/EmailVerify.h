//
// Created by FU-QAQ on 2024/4/18.
//

#ifndef SYNERGYSPOT_EMAILVERIFY_H
#define SYNERGYSPOT_EMAILVERIFY_H

#include <QObject>

class EmailVerify : public QObject{
    Q_OBJECT
public:
    EmailVerify() = default;
    EmailVerify(QObject * bobj,std::string queryTime);
    ~EmailVerify() = default;
    // 发送邮箱验证码
    void sendEmailVerifyCode(const std::string& emailAddress);

    // 获取信息
    std::string getVerifyCode();
    std::string getStartTime();
    std::string getValidTime();

public slots:
    // 解析邮箱验证码
    void parseEmailVerifyCode(const std::string& rawdto);
private:
    // 六位随机验证码
    std::string verifyCode;
    // 获取请求的时刻
    std::string startTime;
    // 验证码有效时长
    std::string validTime;
};

#endif//SYNERGYSPOT_EMAILVERIFY_H
