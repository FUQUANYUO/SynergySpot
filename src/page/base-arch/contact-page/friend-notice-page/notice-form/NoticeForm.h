//
// Created by FU-QAQ on 2024/5/3.
//

#ifndef SYNERGYSPOT_NOTICEFORM_H
#define SYNERGYSPOT_NOTICEFORM_H

#include <QWidget>
#include "help.h"

struct NoticeFormMeb{
    std::string nickname;
    std::string ssid;
    std::string status;
    std::string msg;
    std::string time;
    std::string senderSSid;
    bool isGroup;
};


QT_BEGIN_NAMESPACE
namespace Ui {
    class NoticeForm;
}
QT_END_NAMESPACE

class NoticeForm : public QWidget {
    Q_OBJECT

public:
    explicit NoticeForm(QWidget *parent = nullptr);
    ~NoticeForm() override;
    void setNoticeFormDisplay(NoticeFormMeb& nfm);
private:
    Ui::NoticeForm *ui;
};


#endif//SYNERGYSPOT_NOTICEFORM_H
