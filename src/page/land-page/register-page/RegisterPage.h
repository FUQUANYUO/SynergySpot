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
signals:
    // 传递验证请求时刻、有效时间、验证码
    void TRANSVERIFYDATA(std::array<std::string,3> data);
private:
    Ui::RegisterPage *ui;
    std::array<std::string,3> _data;
};


#endif//SYNERGYSPOT_REGISTERPAGE_H
