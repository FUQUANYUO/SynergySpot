//
// Created by FU-QAQ on 2024/2/24.
//

#ifndef SYNERGYSPOT_REGISTERPAGE_H
#define SYNERGYSPOT_REGISTERPAGE_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
    class RegisterPage;
}
QT_END_NAMESPACE

class RegisterPage : public QMainWindow {
    Q_OBJECT
public:
    explicit RegisterPage(QWidget *parent = nullptr);
    ~RegisterPage() override;

private:
    Ui::RegisterPage *ui;
};


#endif//SYNERGYSPOT_REGISTERPAGE_H
