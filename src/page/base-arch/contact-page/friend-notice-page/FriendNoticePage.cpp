//
// Created by FU-QAQ on 2024/5/3.
//

// You may need to build the project (run Qt uic code generator) to get "ui_FriendNoticePage.h" resolved

#include "FriendNoticePage.h"
#include "ui_FriendNoticePage.h"

#include "friend/FriendDTO.pb.h"

#include "yaml-cpp/yaml.h"

QWidget * _parent;

#ifdef WIN32
static std::string yamlPathServer = "../../conf/serverInfo.yaml";
#else
static std::string yamlPathServer = "../conf/serverInfo.yaml";
#endif


FriendNoticePage::FriendNoticePage(QWidget *parent) : ui(new Ui::FriendNoticePage) {
    _parent = parent;
    ui->setupUi(this);
    ui->lnotice->layout()->setSpacing(10);

    // 清空通知
    connect(ui->clear,&QPushButton::clicked,this,[=](){
        for(auto it : noticeArr){
            it->hide();
            ui->lnotice->layout()->removeWidget(it);
            it->deleteLater();
        }
        noticeArr.clear();
    });
}

FriendNoticePage::~FriendNoticePage() {
    for(auto it : noticeArr){
        it->deleteLater();
    }
    delete ui;
}

void FriendNoticePage::AddNewNotice(const std::string &outdto) {
    YAML::Node node = YAML::LoadFile(yamlPathServer);
    auto acceptStr = node["request-friend-status"]["acceptStr"].as<std::string>();
    auto rejectStr = node["request-friend-status"]["rejectStr"].as<std::string>();
    auto waitedStr = node["request-friend-status"]["waitedStr"].as<std::string>();
    SSDTO::AddFriend_DTO afdto;
    afdto.ParseFromString(outdto);

    LOG(afdto.receiver().ssid() << " , " << CurSSID);
    // 过滤 同意\拒绝 申请信息
    if(afdto.sender().ssid() == CurSSID && afdto.is_accept() != waitedStr)
        return ;

    NoticeFormMeb nfm;
    NoticeForm * nf = new NoticeForm(_parent);
    // 判断Notice是：响应结果 OR 当前用户的请求 OR 其他用户的请求

    // 当前用户的请求记录
    if(afdto.sender().ssid() == CurSSID && afdto.is_accept() == waitedStr){
        nfm.isGroup = afdto.receiver().is_group();
        nfm.nickname = afdto.receiver().nickname();
        nfm.ssid = afdto.receiver().ssid();
        nfm.senderSSid = CurSSID;
    }else{
        nfm.isGroup = afdto.sender().is_group();
        nfm.nickname = afdto.sender().nickname();
        nfm.ssid = afdto.sender().ssid();
        nfm.senderSSid = nfm.ssid;
    }
    nfm.status = afdto.is_accept();
    nfm.msg = afdto.msg();
    nfm.time = afdto.time();
    nf->setNoticeFormDisplay(nfm);
    QLayout * litem = ui->lnotice->layout();
    litem->addWidget(nf);
}
