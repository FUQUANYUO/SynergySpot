//
// Created by FU-QAQ on 2024/2/10.
//

#ifndef OCHAT_MSGPAGE_H
#define OCHAT_MSGPAGE_H

#include <QWidget>
#include <QThread>

#include "base/message-list/MessageList.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class MsgPage;
}
QT_END_NAMESPACE

class MsgPage : public QWidget {
    Q_OBJECT
public:
    explicit MsgPage(MessageList * ml,QWidget *parent = nullptr);
    ~MsgPage() override;
private:
    Ui::MsgPage *ui;
    std::string _sendTo = "";
};


#endif//OCHAT_MSGPAGE_H
