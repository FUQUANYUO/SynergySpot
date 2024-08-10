//
// Created by FU-QAQ on 2024/5/3.
//

// You may need to build the project (run Qt uic code generator) to get "ui_NoticeForm.h" resolved

#include "NoticeForm.h"
#include "ui_NoticeForm.h"

#include "base-arch/arch-page/ArchPage.h"

#include "friend/FriendDTO.pb.h"

#include "get-time/GetCurTime.h"
#include "yaml-cpp/yaml.h"

#ifdef WIN32
static std::string yamlPathServer = "../../conf/serverInfo.yaml";
#else
static std::string yamlPathServer = "../conf/serverInfo.yaml";
#endif


NoticeForm::NoticeForm(QWidget *parent) : ui(new Ui::NoticeForm) {
    ui->setupUi(this);
    BusinessListen * bl = dynamic_cast<ArchPage*>(parent->parent()->parent())->getBusinessObj();
    YAML::Node node = YAML::LoadFile(yamlPathServer);
    auto acceptStr = node["request-friend-status"]["acceptStr"].as<std::string>();
    auto rejectStr = node["request-friend-status"]["rejectStr"].as<std::string>();
    auto waitedStr = node["request-friend-status"]["waitedStr"].as<std::string>();
    connect(ui->isAccept,&QPushButton::clicked,this,[=](){
        ui->isAccept->setDisabled(true);
        ui->isAccept->setText("已同意");
        SSDTO::AddFriend_DTO fdto;
        fdto.set_type(SSDTO::ADD_FRIEND);

        SSDTO::FriendInfo * sender = fdto.mutable_sender();
        SSDTO::FriendInfo * receiver = fdto.mutable_receiver();

        // 用户作为发送者
        sender->set_ssid(CurSSID);
        sender->set_nickname(CurSSname);
        sender->set_is_group(false);

        std::string senderSSID = ui->nickname->text().sliced(ui->nickname->text().indexOf('(')+1,8).toStdString();
        std::string senderName = ui->nickname->text().sliced(0,ui->nickname->text().size()-10).toStdString();

        receiver->set_ssid(senderSSID);
        receiver->set_nickname(senderName);
        if(ui->isAccept->text() == "同意加入群聊")
            receiver->set_is_group(true);
        else
            receiver->set_is_group(false);

        fdto.set_is_accept(acceptStr);
        fdto.set_msg("您好，我已经同意了你的好友请求~");
        fdto.set_time(GetCurTime::getTimeObj()->getCurTime());

        std::string out;
        fdto.SerializeToString(&out);
        emit bl->ADD_FRIEND(out);

        // 请求更新联系人表
        SSDTO::GetFriendList_DTO gfldto;
        gfldto.set_type(SSDTO::GET_CONTACTLIST);
        gfldto.set_request_ssid(CurSSID);
        out.clear();
        gfldto.SerializeToString(&out);
        emit bl->GET_CONTACTLIST(out);
    });
}

NoticeForm::~NoticeForm() {
    delete ui;
}
void NoticeForm::setNoticeFormDisplay(NoticeFormMeb &nfm) {
    ui->nickname->setText(QString::fromStdString(nfm.nickname) + " (" + QString::fromStdString(nfm.ssid) + ")");
    ui->msg->setText(QString::fromStdString(nfm.msg));
    ui->status->setText(QString::fromStdString(nfm.status));
    ui->time->setText(QString::fromStdString(nfm.time).sliced(0,16));

    // 当前用户的请求
    if(nfm.senderSSid == CurSSID){
        ui->isAccept->setDisabled(true);
        ui->isAccept->hide();
        return ;
    }

    // 设置按钮的文本区分群组与用户
    if(nfm.isGroup){
        ui->isAccept->setText("同意加入群聊");
    }else{
        ui->isAccept->setText("同意好友请求");
    }
}
