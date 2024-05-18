//
// Created by FU-QAQ on 2024/5/3.
//

#ifndef SYNERGYSPOT_FRIENDNOTICEPAGE_H
#define SYNERGYSPOT_FRIENDNOTICEPAGE_H

#include <QWidget>
#include "notice-form/NoticeForm.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class FriendNoticePage;
}
QT_END_NAMESPACE

class FriendNoticePage : public QWidget {
    Q_OBJECT

public:
    explicit FriendNoticePage(QWidget *parent = nullptr);
    ~FriendNoticePage() override;
public slots:
    void AddNewNotice(const std::string& outdto);
private:
    Ui::FriendNoticePage *ui;

    std::vector<QWidget*> noticeArr;
};


#endif//SYNERGYSPOT_FRIENDNOTICEPAGE_H
