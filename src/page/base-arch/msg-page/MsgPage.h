//
// Created by FU-QAQ on 2024/2/10.
//

#ifndef OCHAT_MSGPAGE_H
#define OCHAT_MSGPAGE_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui {
    class MsgPage;
}
QT_END_NAMESPACE

class MsgPage : public QWidget {
    Q_OBJECT

public:
    explicit MsgPage(QWidget *parent = nullptr);
    ~MsgPage() override;

private:
    Ui::MsgPage *ui;
};


#endif//OCHAT_MSGPAGE_H
