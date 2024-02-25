//
// Created by FU-QAQ on 2024/2/10.
//

#ifndef OCHAT_LANDPAGE_H
#define OCHAT_LANDPAGE_H

#include <QMainWindow>

//-----------      core      -----------//
// land-check
#include "land/land-check/LoginVerify.h"

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
private:
    Ui::LandPage *ui;
    LoginVerify lver;
};


#endif//OCHAT_LANDPAGE_H
