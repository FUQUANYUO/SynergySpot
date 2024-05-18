//
// Created by FU-QAQ on 2024/5/5.
//

// You may need to build the project (run Qt uic code generator) to get "ui_SearchFriendPage.h" resolved

#include "SearchFriendPage.h"
#include "ui_SearchFriendPage.h"

#include "../../arch-page/ArchPage.h"

#include "friend/FriendDTO.pb.h"


SearchFriendPage::SearchFriendPage(QWidget *parent) : QMainWindow(parent), ui(new Ui::SearchFriendPage) {
    ui->setupUi(this);
    BusinessListen * bl = dynamic_cast<ArchPage*>(parent->parent())->getBusinessObj();
    sizePage = 0;
    sizeRes = 0;
    ui->lall->layout()->setSpacing(10);
    ui->lusr->layout()->setSpacing(10);
    ui->lgroup->layout()->setSpacing(10);
    connect(ui->searchBtn,&QPushButton::clicked,this,[=](){
        if(!ui->search->text().isEmpty()){
            SSDTO::Search_DTO sdto;
            sdto.set_request_ssid(CurSSID);
            sdto.set_key_word(ui->search->text().toStdString());
            sdto.set_type(SSDTO::FRIEND_SEARCH);

            std::string out;
            sdto.SerializeToString(&out);
            emit bl->REQUEST_SEARCH(out);
        }
    });
}

SearchFriendPage::~SearchFriendPage() {
    for(auto it : delVec)
        it->deleteLater();
    delete ui;
}

// ssid(user) : {nickname,remark}
extern std::map<std::string,std::pair<std::string,std::string>> sWithInfo;
// ssid(group) : {nickname,remark}
extern std::map<std::string,std::pair<std::string,std::string>> gsWithInfo;

void SearchFriendPage::UpdateSearchRes(const std::string& outdto){
    SSDTO::Search_DTO sdto;
    sdto.ParseFromString(outdto);
    sizePage = sdto.page_size();
    sizeRes = sdto.res_size();

    LOG(sdto.search_infos_size())
    res.clear();
    for(const auto& it : (*sdto.mutable_search_infos())){
        if(it.is_group()){
            auto findGroupSSID = gsWithInfo.find(it.ssid());
            if(findGroupSSID != gsWithInfo.end())
                continue;
        }else{
            auto findUsrSSID = sWithInfo.find(it.ssid());
            if(findUsrSSID != sWithInfo.end())
                continue;
        }
        res.push_back({it.nickname(),it.ssid()});

        SearchForm * sf = new SearchForm(this);
        delVec.push_back(sf);
        sf->setSearchFormDisplay(res.back());

        // 全部
        QLayout * litem = ui->lall->layout();
        litem->addWidget(sf);
        // 用户
        if(!it.is_group()){
            QLayout * uitem = ui->lusr->layout();
            SearchForm * usf = new SearchForm(this);
            delVec.push_back(usf);
            usf->setSearchFormDisplay(res.back());
            usf->setApplyBtnText("添加好友");

            uitem->addWidget(usf);
        }
        // 群组
        else{
            QLayout * gitem = ui->lusr->layout();
            SearchForm * gsf = new SearchForm(this);
            delVec.push_back(gsf);
            gsf->setSearchFormDisplay(res.back());
            gsf->setApplyBtnText("申请加群");

            gitem->addWidget(gsf);
        }
    }
}
