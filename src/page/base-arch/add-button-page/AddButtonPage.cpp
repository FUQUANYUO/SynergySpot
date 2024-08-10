//
// Created by FU-QAQ on 2024/5/5.
//

// You may need to build the project (run Qt uic code generator) to get "ui_AddButtonPage.h" resolved

#include "AddButtonPage.h"
#include "ui_AddButtonPage.h"

#include <QMouseEvent>

AddButtonPage::AddButtonPage(QWidget *parent) : QWidget(parent), ui(new Ui::AddButtonPage) {
    ui->setupUi(this);
    sfp = new SearchFriendPage(this);
    sfp->hide();
    setMouseTracking(true);
    connect(ui->search,&QPushButton::clicked,this,[=](){
        sfp->show();
    });
}

AddButtonPage::~AddButtonPage() {
    delete ui;
}

SearchFriendPage *AddButtonPage::getSearchFriendPageObj() {
    return sfp;
}
