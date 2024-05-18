//
// Created by FU-QAQ on 2024/5/5.
//

#ifndef SYNERGYSPOT_ADDBUTTONPAGE_H
#define SYNERGYSPOT_ADDBUTTONPAGE_H

#include <QWidget>

#include "search-friend-page/SearchFriendPage.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class AddButtonPage;
}
QT_END_NAMESPACE

class AddButtonPage : public QWidget {
    Q_OBJECT

public:
    explicit AddButtonPage(QWidget *parent = nullptr);
    ~AddButtonPage() override;
    SearchFriendPage* getSearchFriendPageObj();
private:
    Ui::AddButtonPage *ui;

    SearchFriendPage *sfp;
};


#endif//SYNERGYSPOT_ADDBUTTONPAGE_H
