//
// Created by FU-QAQ on 2024/2/9.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ArchPage.h" resolved
#include "ArchPage.h"
#include "ui_ArchPage.h"
#include "help.h"

#include "../contact-page/ContactPage.h"                            // 联系人模块
#include "../more-option-page/MoreOptionPage.h"                     // 更多选项模块
#include "../avatar-page/AvatarPage.h"                              // 头像模块
#include "../add-button-page/AddButtonPage.h"                       // 添加按钮

#include "base/message-list/MessageList.h"                          // 消息列表

#define SHOW_CONTACT_PAGE                                           // 测试联系人模块
#define SHOW_MSG_PAGE                                               // 测试消息模块
#define SHOW_MORE_OPTION_PAGE                                       // 测试更多设置模块
#define SHOW_AVATAR_PAGE                                            // 测试头像模块

#include "friend/FriendDTO.pb.h"

ArchPage::ArchPage(QObject * obj,QWidget *parent) : QMainWindow(parent), ui(new Ui::ArchPage) {
    ui->setupUi(this);
    bl = dynamic_cast<BusinessListen*>(obj);
    setMouseTracking(true);

    AddButtonPage * abp = new AddButtonPage(this);
    // 设为模态
    abp->setWindowFlags(Qt::Popup);
    delVec.push_back(abp);
    // 隐藏
    abp->hide();
    // 添加按钮槽函数
    connect(ui->add,&QPushButton::clicked,this,[=](){
        // 设置出现的位置（经验值）
        QPoint targetP = this->pos() + QPoint(ui->add->x()+40,(ui->add->y())+30);
        abp->move(targetP);
        abp->show();
    });

    // 搜索结果响应
    connect(bl,&BusinessListen::GET_SEARCH_RES,abp->getSearchFriendPageObj(),&SearchFriendPage::UpdateSearchRes);
#ifdef SHOW_MSG_PAGE
    // 消息列表
    QListView * lv = new QListView(this);
    lv->setMouseTracking(true);
    lv->installEventFilter(this);

    MessageList * ml = new MessageList(*lv);
    delVec.push_back(reinterpret_cast<QObject*>(ml));
    ui->Vlayout->addWidget(lv);

    #ifdef SHOW_CONTACT_PAGE
        ContactPage * cp = new ContactPage(this);
        ui->Vlayout->addWidget(cp);
        cp->hide();

        // 联系人激活
        connect(cp->getContactList(),&ContactList::SELECTED_CONTACTITEM,ml,&MessageList::Add_ContactItem);
        // 好友通知
        connect(bl,&BusinessListen::ADD_FRIEND,cp->getFriendNoticePage(),&FriendNoticePage::AddNewNotice);
        connect(bl,&BusinessListen::RECV_FRIEND_QUEST,cp->getFriendNoticePage(),&FriendNoticePage::AddNewNotice);
    #endif

    // 当点击后获取待发送的消息对象
    connect(ml,&MessageList::SELECTED_MSGITEM,this,[=](const std::string &ssid){
        // 外部独立消息窗口（两种方式不能共存）
        QLayoutItem * qit = ui->Hlayout->itemAt(2);
        QWidget * lastP = nullptr;
        if(qit != nullptr){
           lastP = qit->widget();
        }
        auto res = lmp.find(ssid);
        if(res == lmp.end()){// 不存在这个好友的聊天页面
            auto * p = new MsgPage(this);
            p->hide();
            delVec.push_back(p);
            p->setSendTo(ssid);// 消息包中需要sendToSSID

            if(lastP != nullptr){
                ui->Hlayout->removeWidget(lastP);
                lastP->hide();
            }
            // 内部独立消息窗口（两种方式不能共存） 注释下方语句即可完成隔离
            ui->Hlayout->addWidget(p);
            p->show();

            lmp.insert({ssid,p});
        }else{
            if(lastP == nullptr){
                // 内部独立消息窗口（两种方式不能共存） 注释下方语句即可完成隔离
                ui->Hlayout->addWidget(res->second);
                res->second->show();
            }else{
                ui->Hlayout->removeWidget(lastP);
                lastP->hide();

                // 内部独立消息窗口（两种方式不能共存） 注释下方语句即可完成隔离
                ui->Hlayout->addWidget(res->second);
                res->second->show();
            }
        }
    });
#endif

#ifdef SHOW_MORE_OPTION_PAGE
    MoreOptionPage * mop = new MoreOptionPage(this);
    // 设为模态
    mop->setWindowFlags(Qt::Popup);
    // 隐藏
    mop->hide();
    // 更多设置按钮槽函数
    connect(ui->option,&QPushButton::clicked,this,[=](){
        // 设置出现的位置（经验值）
        QPoint targetP = this->pos() + QPoint(95,(ui->option->y())-(mop->height())+30);
        mop->move(targetP);
        mop->show();
    });
#endif

#ifdef SHOW_AVATAR_PAGE
    AvatarPage * ap = new AvatarPage(this);
    // 设为模态
    ap->setWindowFlags(Qt::Popup);
    ap->hide();
    connect(ui->avatar,&QPushButton::clicked,this,[=](){
       // 设置出现的位置
       QPoint targetP = this->pos() + QPoint(30,50);
       ap->move(targetP);
       ap->show();
    });
#endif

    // 左侧 消息区域/联系人区域 切换
    connect(ui->contact,&QPushButton::clicked,this,[=](){
        cp->show();
    // 有其他的冲突功能页都需要隐藏
#ifdef SHOW_CONTACT_PAGE
        lv->hide();
#endif
    });

    connect(ui->msg,&QPushButton::clicked,this,[=](){
        lv->show();
    // 有其他的冲突功能页都需要隐藏
#ifdef SHOW_MSG_PAGE
        cp->hide();
#endif
    });

    // 关闭信号重载
    connect(this,&ArchPage::over,this,[&](){
        delVec.push_back(bl);
        this->deleteLater();
    });
}

ArchPage::~ArchPage() {
    for(auto it : delVec){
        it->deleteLater();
    }
    delete ui;
}

BusinessListen* ArchPage::getBusinessObj() {
    return bl;
}

std::unordered_map<std::string, MsgPage *> &ArchPage::getMsgPageMap() {
    return lmp;
}

void ArchPage::addDelWidget(QWidget * wid){
    delVec.push_back(wid);
}
