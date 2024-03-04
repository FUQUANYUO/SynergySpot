//
// Created by FU-QAQ on 2024/2/10.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MsgPage.h" resolved
#include "MsgPage.h"
#include "ui_MsgPage.h"

#include "base-arch/arch-page/ArchPage.h"
#include "forward_msg/ForwardMsgDTO.pb.h"
#include "help.h"

extern std::string CurSSID;

MsgPage::MsgPage(MessageList * ml,QWidget *parent) : ui(new Ui::MsgPage) {
    ui->setupUi(this);
    BusinessListen * bl = dynamic_cast<ArchPage*>(parent)->getBusinessObj();

    // 当点击后获取待发送的消息对象
    connect(ml,&MessageList::SelectedMsgItem,this,[=](const std::string &ssid){
        _sendTo = ssid;
    });
    // 发送信息
    connect(ui->send,&QPushButton::clicked,this,[=](){
        QString inputTxt = ui->text->toPlainText();
        if(!inputTxt.isEmpty()){
            // 获取当前时间
            std::time_t now = std::time(nullptr);
            char timebuf[20];
            tm localTime{};
            localtime_s(&localTime,&now);
            std::strftime(timebuf,sizeof(timebuf),"%Y-%m-%d %H:%M:%S",&localTime);

            // 封装 forward_dto
            SSDTO::ForwardMsg_DTO fmdto;
            fmdto.set_type(SSDTO::Business_Type::FOWARD_MSG);
            fmdto.set_recv_ssid(_sendTo);
            fmdto.set_send_ssid(CurSSID);
            fmdto.set_date_time(timebuf);
            fmdto.set_content(inputTxt.toStdString());
            fmdto.set_is_forward_success(false);
            fmdto.set_ip_sender("");
            std::string out;
            fmdto.SerializeToString(&out);

            // 发送转发请求信号
            emit bl->FORWARD_MSG(out);
        }
    });

    // 接收转发信息
    connect(bl,&BusinessListen::RECV_MSG,[=](const std::string rawFmdto){
        SSDTO::ForwardMsg_DTO fmdto;
        fmdto.ParseFromString(rawFmdto);
        LOG(fmdto.content());
    });
}

MsgPage::~MsgPage() {
    delete ui;
}
