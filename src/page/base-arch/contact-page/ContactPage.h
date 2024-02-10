//
// Created by FU-QAQ on 2024/2/9.
//

#ifndef OCHAT_CONTACTPAGE_H
#define OCHAT_CONTACTPAGE_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui {
    class ContactPage;
}
QT_END_NAMESPACE

class ContactPage : public QWidget {
    Q_OBJECT

public:
    explicit ContactPage(QWidget *parent = nullptr);
    ~ContactPage() override;

private:
    Ui::ContactPage *ui;
};


#endif//OCHAT_CONTACTPAGE_H
