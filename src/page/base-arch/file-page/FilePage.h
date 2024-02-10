//
// Created by FU-QAQ on 2024/2/9.
//

#ifndef OCHAT_FILEPAGE_H
#define OCHAT_FILEPAGE_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui {
    class FilePage;
}
QT_END_NAMESPACE

class FilePage : public QMainWindow {
    Q_OBJECT

public:
    explicit FilePage(QWidget *parent = nullptr);
    ~FilePage() override;

private:
    Ui::FilePage *ui;
};


#endif//OCHAT_FILEPAGE_H
