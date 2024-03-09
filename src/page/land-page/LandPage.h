//
// Created by FU-QAQ on 2024/2/10.
//

#ifndef OCHAT_LANDPAGE_H
#define OCHAT_LANDPAGE_H

#include <QMainWindow>

//-----------      core      -----------//
// business-listen
#include "base/business-listen/BusinessListen.h"


QT_BEGIN_NAMESPACE
namespace Ui {
    class LandPage;
}
QT_END_NAMESPACE

class LandPage : public QMainWindow {
    Q_OBJECT
public:
    explicit LandPage(QWidget *parent = nullptr);
    ~LandPage() override;
signals:
private:
    Ui::LandPage *ui;
    // 向服务端提交验证信息
    LoginVerify* lver;
    // 监听服务端返回的信息
    BusinessListen *bl;
};


#endif//OCHAT_LANDPAGE_H
