//
// Created by FU-QAQ on 2024/2/9.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ContactPage.h" resolved

#include "ContactPage.h"
#include "ui_ContactPage.h"

#include "base/contact-list/ContactList.h"          // 联系人列表

ContactPage::ContactPage(QWidget *parent) : QWidget(parent), ui(new Ui::ContactPage) {
    ui->setupUi(this);
    cl = new ContactList({{"好友",ui->ftv},
                              {"群组",ui->gtv}},
                          parent);
}

ContactPage::~ContactPage() {
    cl->deleteLater();
    delete ui;
}

ContactList *ContactPage::getContactList() {
    return cl;
}
