//
// Created by FU-QAQ on 2024/2/10.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MsgPage.h" resolved
#include "MsgPage.h"
#include "ui_MsgPage.h"

#include "base-arch/arch-page/ArchPage.h"
#include "forward_msg/ForwardMsgDTO.pb.h"

#include "get-time/GetCurTime.h"

#include "help.h"

// 每一个用户（好友/群）拥有一个MsgPage
MsgPage::MsgPage(QWidget *parent) : ui(new Ui::MsgPage) {
    ui->setupUi(this);
    BusinessListen * bl = dynamic_cast<ArchPage*>(parent)->getBusinessObj();
    mcs = new MsgContentShow(ui->msgList,bl);

    ui->msgList->setStyleSheet(
        "QListView {"
        "background-color: #f5f5f5;"
        "}"
    );

    // 发送信息
    connect(ui->send,&QPushButton::clicked,this,[=](){
        QString inputTxt = ui->text->toPlainText();
        if(!inputTxt.isEmpty()){
            // 清空信息
            ui->text->clear();

            // 在 ui listview 中显示
            emit mcs->PRESENT_SENDCONTENT(inputTxt);

            // 封装 forward_dto
            SSDTO::ForwardMsg_DTO fmdto;
            fmdto.set_type(SSDTO::Business_Type::FOWARD_MSG);
            fmdto.set_recv_ssid(_sendTo);
            fmdto.set_send_ssid(CurSSID);
            fmdto.set_date_time(GetCurTime::getTimeObj()->getCurTime());
            fmdto.set_content(inputTxt.toStdString());
            fmdto.set_ip_sender("");
            std::string out;
            fmdto.SerializeToString(&out);

            // 发送转发请求信号
            emit bl->FORWARD_MSG(out);
        }
    });
}

MsgPage::~MsgPage() {
    delete ui;
}

void MsgPage::setSendTo(std::string sendTo) {
    _sendTo = std::move(sendTo);
}

void MsgPage::addNewInfo(std::vector<std::string> args) {
    mcs->addNewItem(args[0],args[1]);
}
