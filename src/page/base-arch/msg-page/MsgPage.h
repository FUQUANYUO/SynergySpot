//
// Created by FU-QAQ on 2024/2/10.
//

#ifndef OCHAT_MSGPAGE_H
#define OCHAT_MSGPAGE_H

#include <QWidget>
#include <QThread>

#include "base/message-list/MessageList.h"
#include "base/msgcontent-show/MsgContentShow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class MsgPage;
}
QT_END_NAMESPACE

class MsgPage : public QWidget {
    Q_OBJECT
public:
    explicit MsgPage(QWidget *parent = nullptr,QObject * obj = nullptr);
    void setSendTo(std::string sendTo);

    // 添加新的信息
    // args[0] ssid, args[1] content
    void addNewInfo(std::vector<std::string> args);

    ~MsgPage() override;
private:
    Ui::MsgPage *ui;
    MsgContentShow * mcs;
    std::string _sendTo = "";
};


#endif//OCHAT_MSGPAGE_H
