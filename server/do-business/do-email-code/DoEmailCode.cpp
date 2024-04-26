//
// Created by FU-QAQ on 2024/4/20.
//

#include <random>

#include "DoEmailCode.h"

#include <curl/curl.h>
#include "yaml-cpp/yaml.h"
#include "email/EmailVerifyCode.pb.h"

extern std::string yamlPath;

class EmailSendByCurl{
public:
    EmailSendByCurl() = default;
    EmailSendByCurl(const std::string& sender,const std::string& receivers,const std::string& accessToken,const std::string& emailServer = "smtp.qq.com:587");
    std::string content(const std::string& sender, const std::string& receiver, const std::string& subject, const std::string& emailCode);

    void sendEmail(const std::string& emailCode);
    ~EmailSendByCurl();
private:
    CURL *curl;
    std::string _sender;
    std::string _receivers;
    std::string _accessToken;
    std::string _emailServer;
};

// curl回调函数
size_t curlReadCallback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    auto dataPtr = static_cast<std::string*>(userdata);

    // 计算此次调用可以读取的最大字节数
    size_t totalSize = size * nmemb;

    // 确保不会读取超出字符串结束的字节
    size_t len = std::min(totalSize, dataPtr->size());

    // 将数据从 userdata 复制到提供的缓冲区
    memcpy(ptr, dataPtr->data(), len);

    // 从 userdata 字符串中移除已读取的部分
    dataPtr->erase(0, len);

    // 返回实际读取的字节数
    return len;
}

void DoEmailCode::genEmailCode(int len) {
    const std::string chars = "0123456789";
    std::random_device rd;
    // 梅森旋转算法
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0,chars.size()-1);

    this->emailCode = "";
    for(int i = 0; i < len; ++i){
        emailCode += chars[distribution(generator)];
    }
}

std::string DoEmailCode::sendEmailCode(std::string &rawdto) {
    YAML::Node node = YAML::LoadFile(yamlPath);
    if (node.IsNull()) {
        LOG("open server yaml failed, please check path!")
        return "";
    }
    std::string sender = node["email-info"]["sendAccount"].as<std::string>();
    std::string emailServer = node["email-info"]["emailServer"].as<std::string>();
    std::string accessToken = node["email-info"]["accessToken"].as<std::string>();

    SSDTO::EmailVerifyCode_DTO evdto;
    evdto.ParseFromString(rawdto);

    if(!evdto.is_request())
       return "";

    LOG(evdto.email_address())
    std::string targetEmail = evdto.email_address();
    EmailSendByCurl ecl(sender,targetEmail,accessToken,emailServer);

    // 生成验证码
    genEmailCode(6);

    // 发送
    ecl.sendEmail(emailCode);

    // 回传信息
    evdto.set_verify_code(emailCode);
    evdto.set_valid_time("10");
    evdto.set_is_request(false);
    rawdto = "";
    evdto.SerializeToString(&rawdto);
    return rawdto;
}

EmailSendByCurl::EmailSendByCurl(const std::string& sender,const std::string& receivers,const std::string& accessToken,const std::string& emailServer) :
    _sender(sender), _receivers(receivers), _accessToken(accessToken), _emailServer(emailServer)
{
    curl = curl_easy_init();
    if(curl) {
        // 设置邮件发送服务器
        curl_easy_setopt(curl, CURLOPT_URL, emailServer.c_str());
        // 设置使用SSL
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_LOGIN_OPTIONS, "AUTH=LOGIN");
        curl_easy_setopt(curl, CURLOPT_USERNAME, sender.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, accessToken.c_str());

        // 设置发件人
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM,sender.c_str());

        curl_easy_setopt(curl, CURLOPT_READFUNCTION,curlReadCallback);

//        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    }else{
        LOG("curl init failed!!!")
    }
}

void EmailSendByCurl::sendEmail(const std::string& emailCode) {
    if(curl){
        CURLcode res;
        // 收件人
        curl_slist * recipients = nullptr;
        std::string data = content(_sender,_receivers,"SynergySpot Register (注册验证码)",emailCode);
        recipients = curl_slist_append(recipients,_receivers.c_str());

        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
        curl_easy_setopt(curl, CURLOPT_READDATA, &data);
        // 发送邮件
        res = curl_easy_perform(curl);
        if(res != CURLE_OK){
            LOG("curl_easy_perform() failed: " << curl_easy_strerror(res))
        }
        curl_slist_free_all(recipients);
    }else{
        LOG("curl init failed!!!")
    }
}

EmailSendByCurl::~EmailSendByCurl() {
    curl_easy_cleanup(curl);
}

std::string generatorHtmlTemp(const std::string& emailCode){
    return
            R"(
            <!DOCTYPE html>
            <html lang="en">
            <head>
                <meta charset="UTF-8">
                <title>验证码通知</title>
                <style>
                    body {
                        font-family: Arial, sans-serif;
                        background-color: #f4f4f4;
                        margin: 0;
                        padding: 0;
                    }
                    .container {
                        width: 100%;
                        max-width: 600px;
                        margin: 20px auto;
                        background-color: #ffffff;
                        padding: 20px;
                        border-radius: 8px;
                        box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
                    }
                    h1 {
                        color: #333333;
                        text-align: center;
                    }
                    p {
                        color: #666666;
                        line-height: 1.6;
                    }
                    .code {
                        font-weight: bold;
                        font-size: 40px; /* 设置验证码的字体大小 */
                        letter-spacing: 5px; /* 设置字符间距 */
                        color: #0056b3;
                        display: block;
                        text-align: center;
                        margin-bottom: 20px;
                    }
                </style>
            </head>
            <body>
                <div class="container">
                    <h1>SynergySpot注册验证码</h1>
                    <p>亲爱的用户，您正在进行注册操作的身份验证操作。请在客户端输入您的验证码：</p>
                    <p><span class="code">)" + emailCode + R"(</span></p>
                    <p>注意：验证码有效期为10分钟。</p>
                </div>
            </body>
            </html>)";
}

std::string EmailSendByCurl::content(const std::string &sender, const std::string &receiver, const std::string &subject, const std::string &emailCode) {
    return
            "To: <" + receiver + ">\r\n"
            "From: SynergySpot 注册中心 <" + sender + ">\r\n"
            "Subject: " + subject + "\r\n"
            "MIME-version: 1.0\r\n"
            "Content-Type: text/html; charset=\"UTF-8\"\r\n"
            "\r\n"
            + generatorHtmlTemp(emailCode) + "\r\n";
}
