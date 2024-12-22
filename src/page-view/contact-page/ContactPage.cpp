//
// Created by FU-QAQ on 2024/12/11.
//

#include "ContactPage.h"
#include "contact-delegate/ContactDelegate.h"
#include "contact-model/ContactModel.h"

#include "ela-widget-tools/ElaTreeView.h"
#include "ela-widget-tools/ElaToolButton.h"
#include "ela-widget-tools/ElaPivot.h"

#include <QVBoxLayout>
#include <QStandardItem>
#include <mutex>

ContactPage * ContactPage::_contactPage = nullptr;
static std::mutex m;

ContactPage *ContactPage::getInstance() {
    if(_contactPage == nullptr){
        m.lock();
        if(_contactPage == nullptr){
            _contactPage = new ContactPage();
        }
        m.unlock();
    }
    return _contactPage;
}

void ContactPage::destroyContactPage() {
    if(_contactPage == nullptr)
        return ;
    else{
        m.lock();
        delete _contactPage;
        _contactPage = nullptr;
        m.unlock();
    }
}

ContactPage::ContactPage(QWidget *parent) : ElaScrollPage(parent) {
    initWindow();

    initEdgeLayout();

    initContent();

    initConnectFunc();
}
ContactPage::~ContactPage() {
}

void ContactPage::initWindow() {
    _centralWidget         = new QWidget(this);
    _centralWidLayout      = new QVBoxLayout;
    _friendMangerContainer = new ElaToolButton(this);
    _friendNoticeButton    = new ElaToolButton(this);
    _groupNoticeButton     = new ElaToolButton(this);
    _friendOrGroupPivot    = new ElaPivot(this);
    _friendTree            = new ElaTreeView(this);
    _groupTree             = new ElaTreeView(this);
    _scrollTreeLayout      = new QVBoxLayout;
}

void ContactPage::initEdgeLayout() {
    _centralWidLayout->addWidget(_friendMangerContainer);
    _centralWidLayout->addWidget(_friendNoticeButton);
    _centralWidLayout->addWidget(_groupNoticeButton);
    _centralWidLayout->addWidget(_friendOrGroupPivot);
    _centralWidLayout->setContentsMargins(0,0,0,0);

    _scrollTreeLayout->addWidget(_friendTree);
    _scrollTreeLayout->addWidget(_groupTree);
    _scrollTreeLayout->setContentsMargins(0,0,0,0);

    _centralWidLayout->addItem(_scrollTreeLayout);

    _centralWidget->setLayout(_centralWidLayout);
    _centralWidget->setContentsMargins(0,0,20,0);
    addCentralWidget(_centralWidget,true,true,0);
}

void ContactPage::initContent() {
    setTitleVisible(false);

    _friendMangerContainer->setIsTransparent(false);
    _friendMangerContainer->setElaIcon(ElaIconType::UserGear);
    _friendMangerContainer->setText("好友管理器");
    _friendMangerContainer->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _friendMangerContainer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    _friendMangerContainer->setFixedHeight(40);

    _friendNoticeButton->setElaIcon(ElaIconType::AngleRight);
    _friendNoticeButton->setText("好友通知");
    _friendNoticeButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _friendNoticeButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    _friendNoticeButton->setStyleSheet("ElaToolButton");
    _friendNoticeButton->setFixedHeight(40);

    _groupNoticeButton->setElaIcon(ElaIconType::AngleRight);
    _groupNoticeButton->setText("群通知");
    _groupNoticeButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _groupNoticeButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    _groupNoticeButton->setStyleSheet("ElaToolButton");
    _groupNoticeButton->setFixedHeight(40);

    _friendOrGroupPivot->setTextPixelSize(18);
    _friendOrGroupPivot->setPivotSpacing(10);
    _friendOrGroupPivot->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    _friendOrGroupPivot->appendPivot("好友");
    _friendOrGroupPivot->appendPivot("群组");
    _friendOrGroupPivot->setPivotSpacing(150);
    _friendOrGroupPivot->setFixedHeight(40);
    _friendOrGroupPivot->setCurrentIndex(0);

    // init model
    // grouping
    ContactModel * friendModel = new ContactModel(this);
    ContactModel * groupModel = new ContactModel(this);
    ContactDelegate * cDelegate = new ContactDelegate(this);

    friendModel->addGrouping("测试好友分组一");
    friendModel->addGrouping("测试好友分组二");
    friendModel->addGroupingItem("测试好友分组一",
                                 {"","小柴","","","在线",
                                  QPixmap(":/contact-page/rc-page/img/default-avatar-1.jpg")});
    friendModel->addGroupingItem("测试好友分组一",
                                 {"","五花","","","在线",
                                  QPixmap(":/contact-page/rc-page/img/default-avatar-2.jpg")});
    friendModel->addGroupingItem("测试好友分组二",
                                 {"","大黄","","","离线",
                                  QPixmap(":/contact-page/rc-page/img/default-avatar-3.jpg")});

    groupModel->addGrouping("测试群组分组一");
    groupModel->addGrouping("测试群组分组二");
    groupModel->addGroupingItem("测试群组分组一",
                                {"","Apollo交流群","","","",
                                 QPixmap(":/contact-page/rc-page/img/default-avatar-4.jpg")});
    groupModel->addGroupingItem("测试群组分组二",
                                {"","Github交流群","","","",
                                 QPixmap(":/contact-page/rc-page/img/default-avatar-5.jpg")});
    groupModel->addGroupingItem("测试群组分组二",
                                {"","铲屎官交流群","","","",
                                 QPixmap(":/contact-page/rc-page/img/default-avatar-6.jpg")});

    _friendTree->setModel(friendModel);
    _friendTree->setItemDelegate(cDelegate);
    _friendTree->setEditTriggers(QTreeView::NoEditTriggers);

    _groupTree->hide();
    _groupTree->setModel(groupModel);
    _groupTree->setItemDelegate(cDelegate);
    _groupTree->setEditTriggers(QTreeView::NoEditTriggers);
}

void ContactPage::initConnectFunc() {
    // Pivot change
    connect(_friendOrGroupPivot,&ElaPivot::pivotClicked,[=](int index){
        // friend or group
        if(index == 0){
            _groupTree->hide();
            _friendTree->show();
        }else{
            _friendTree->hide();
            _groupTree->show();
        }
    });
}
