//
// Created by FU-QAQ on 2024/2/9.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ContactPage.h" resolved
#include "ContactPage.h"
#include "ui_ContactPage.h"

#include "base/contact-list/ContactList.h"          // 联系人列表

ContactPage::ContactPage(QWidget *parent) : QWidget(parent), ui(new Ui::ContactPage) {
    ui->setupUi(this);
    fnp = new FriendNoticePage(this);
    cl = new ContactList({{"好友",ui->ftv},
                              {"群组",ui->gtv}},
                          parent);

    connect(ui->friendNotice,&QPushButton::clicked,this,[=](){
       fnp->show();
    });
}

ContactPage::~ContactPage() {
    cl->deleteLater();
    fnp->deleteLater();
    delete ui;
}

FriendNoticePage *ContactPage::getFriendNoticePage() {
    return fnp;
}

ContactList *ContactPage::getContactList() {
    return cl;
}
