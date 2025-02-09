//
// Created by FU-QAQ on 2024/12/19.
//

#ifndef MSGBUBBLEMODEL_H
#define MSGBUBBLEMODEL_H

#include <QStandardItemModel>
#include "define.h"

class MsgBubbleModel : public QStandardItemModel{
public:
    explicit MsgBubbleModel(QObject *parent = nullptr);
    ~MsgBubbleModel();

    void addMsg(const ChatMessage& msg);
};

#endif //MSGBUBBLEMODEL_H
