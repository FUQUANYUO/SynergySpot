//
// Created by FU-QAQ on 2024/5/18.
//

#ifndef SYNERGYSPOT_CREATEGROUPPAGE_H
#define SYNERGYSPOT_CREATEGROUPPAGE_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui {
    class CreateGroupPage;
}
QT_END_NAMESPACE

class CreateGroupPage : public QWidget {
    Q_OBJECT

public:
    explicit CreateGroupPage(QWidget *parent = nullptr);
    ~CreateGroupPage() override;

private:
    Ui::CreateGroupPage *ui;
};


#endif//SYNERGYSPOT_CREATEGROUPPAGE_H
