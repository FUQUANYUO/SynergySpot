//
// Created by FU-QAQ on 2024/5/6.
//

// You may need to build the project (run Qt uic code generator) to get "ui_SearchForm.h" resolved

#include "SearchForm.h"
#include "ui_SearchForm.h"

#include "../../../arch-page/ArchPage.h"

#include "friend/FriendDTO.pb.h"

#include "yaml-cpp/yaml.h"
#include "get-time/GetCurTime.h"

extern std::string CurSSname;

#ifdef WIN32
std::string yamlPathServer = "../../conf/serverInfo.yaml";
#else
std::string yamlPathServer = "../conf/serverInfo.yaml";
#endif

SearchForm::SearchForm(QWidget *parent) :  ui(new Ui::SearchForm) {
    ui->setupUi(this);
    BusinessListen * bl = dynamic_cast<ArchPage*>(parent->parent()->parent())->getBusinessObj();

    YAML::Node nodeServer = YAML::LoadFile(yamlPathServer);
    auto acceptStr = nodeServer["request-friend-status"]["acceptStr"].as<std::string>();
    auto rejectStr = nodeServer["request-friend-status"]["rejectStr"].as<std::string>();
    auto waitedStr = nodeServer["request-friend-status"]["waitedStr"].as<std::string>();
    YAML::Node nodeClient = YAML::LoadFile("../../conf/clientInfo.yaml");
    auto defaultMsg = nodeClient["request-friend-msg"]["default"].as<std::string>();

    connect(ui->applyBtn,&QPushButton::clicked,this,[=](){
        SSDTO::AddFriend_DTO afdto;
        afdto.set_type(SSDTO::ADD_FRIEND);
        afdto.set_is_accept(waitedStr);
        afdto.set_time(GetCurTime::getTimeObj()->getCurTime());
        afdto.set_msg(defaultMsg);

        SSDTO::FriendInfo * sender = afdto.mutable_sender();
        SSDTO::FriendInfo * receiver = afdto.mutable_receiver();

        sender->set_is_group(false);
        sender->set_nickname(CurSSname);
        sender->set_ssid(CurSSID);

        if(ui->applyBtn->text() == "申请加群"){
            receiver->set_is_group(true);
        }else{
            receiver->set_is_group(false);
        }
        receiver->set_nickname(ui->nickname->text().toStdString());
        receiver->set_ssid(ui->ssid->text().toStdString());

        std::string out;
        afdto.SerializeToString(&out);
        emit bl->ADD_FRIEND(out);
        LOG("sender : " << sender->ssid() << "," << "receiver : " << receiver->ssid())
    });
}

SearchForm::~SearchForm() {
    delete ui;
}
void SearchForm::setSearchFormDisplay(SearchFormMeb &sfm) {
    ui->nickname->setText(QString::fromStdString(sfm.nickname));
    ui->ssid->setText(QString::fromStdString(sfm.ssid));
}

void SearchForm::setApplyBtnText(const std::string &text) {
    ui->applyBtn->setText(QString::fromStdString(text));
}
