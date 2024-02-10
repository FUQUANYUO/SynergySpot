//
// Created by FU-QAQ on 2024/2/9.
//

#ifndef OCHAT_MOREOPTIONPAGE_H
#define OCHAT_MOREOPTIONPAGE_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui {
    class MoreOptionPage;
}
QT_END_NAMESPACE

class MoreOptionPage : public QWidget {
    Q_OBJECT

public:
    explicit MoreOptionPage(QWidget *parent = nullptr);
    ~MoreOptionPage() override;

private:
    Ui::MoreOptionPage *ui;
};


#endif//OCHAT_MOREOPTIONPAGE_H
