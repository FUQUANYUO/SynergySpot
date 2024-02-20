//
// Created by FU-QAQ on 2024/2/9.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ArchPage.h" resolved

#include "ArchPage.h"
#include "ui_ArchPage.h"

#include "../contact-page/ContactPage.h"        //联系人模块
#include "../msg-page/MsgPage.h"                //消息模块
#include "../more-option-page/MoreOptionPage.h" //更多选项模块
#include "../avatar-page/AvatarPage.h"          //头像模块

//#define SHOW_CONTACT_PAGE                       //测试联系人模块
#define SHOW_MSG_PAGE                           //测试消息模块
#define SHOW_MORE_OPTION_PAGE                   //测试更多设置模块
#define SHOW_AVATAR_PAGE                        //测试头像模块

ArchPage::ArchPage(QWidget *parent) : QMainWindow(parent), ui(new Ui::ArchPage) {
    ui->setupUi(this);
#ifdef SHOW_CONTACT_PAGE
    ContactPage * cp = new ContactPage(this);
    ui->Vlayout->addWidget(cp);
#endif

#ifdef SHOW_MSG_PAGE
    //QListView * msgView = new QListView(this);
    //msgView-

    //外部独立消息窗口（两种方式不能共存）
    MsgPage * msp = new MsgPage();
    delVec.push_back(msp);
    msp->show();

    //内部独立消息窗口（两种方式不能共存）
    ui->Hlayout->addWidget(msp);
#endif

#ifdef SHOW_MORE_OPTION_PAGE
    MoreOptionPage * mop = new MoreOptionPage(this);
    //隐藏
    mop->hide();
    //更多设置按钮槽函数
    connect(ui->option,&QPushButton::clicked,this,[=](){
        //设置出现的位置（经验值）
        mop->move(95,(ui->option->y())-(mop->height())+30);
        mop->show();
    });
#endif

#ifdef SHOW_AVATAR_PAGE
    AvatarPage * ap = new AvatarPage(this);
    ap->hide();
    connect(ui->avatar,&QPushButton::clicked,this,[=](){
       //设置出现的位置
       ap->move(30,50);
       ap->show();
    });
#endif
}

ArchPage::~ArchPage() {
    for(auto it : delVec){
        delete it;
    }
    delete ui;
}
