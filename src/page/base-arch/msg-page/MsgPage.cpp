//
// Created by FU-QAQ on 2024/2/10.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MsgPage.h" resolved
#include "MsgPage.h"
#include "ui_MsgPage.h"

#include "base-arch/arch-page/ArchPage.h"
#include "base-arch/contact-page/ContactPage.h"

#include "forward_msg/ForwardMsgDTO.pb.h"

#include "get-time/GetCurTime.h"

#include "help.h"

// 每一个用户（好友/群）拥有一个MsgPage
MsgPage::MsgPage(QWidget *parent,QObject * obj) : ui(new Ui::MsgPage) {
    ui->setupUi(this);
    BusinessListen * bl = dynamic_cast<ArchPage*>(parent)->getBusinessObj();
    mcs = new MsgContentShow(ui->msgList,bl);

    ui->msgList->setStyleSheet(
        "QListView {"
        "background-color: #f5f5f5;"
        "}"
    );

    // 发送信息
    connect(ui->send, &QPushButton::clicked, this, [=]() {
        QString inputTxt = ui->text->toPlainText();
        if (!inputTxt.isEmpty()) {
            // 清空输入框
            ui->text->clear();

            // 在 ui listview 中显示
            emit mcs->PRESENT_SENDCONTENT(inputTxt);

            // 封装 ChatMessage 及其包含的 ForwardMsg_DTO 消息
            SSDTO::ChatMessage chatMessage;
            auto * fmdto = new SSDTO::ForwardMsg_DTO();

            fmdto->set_type(SSDTO::Business_Type::FOWARD_MSG);
            fmdto->set_recv_ssid(_sendTo);
            fmdto->set_send_ssid(CurSSID);
            fmdto->set_date_time(GetCurTime::getTimeObj()->getCurTime());
            fmdto->set_content(inputTxt.toStdString());
            fmdto->set_ip_sender("");

            // 设置消息体的 ForwardMsg_DTO 类型
            chatMessage.set_allocated_text_message(fmdto);

            std::string out;
            chatMessage.SerializeToString(&out);
            emit bl->FORWARD_MSG(out);
        }
    });

    /*
     * 接收信息于 core/base/contact-list/ContactList.cpp 中，其接收会触发message-list的更新
     * */
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
