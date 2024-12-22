//
// Created by FU-QAQ on 2024/12/19.
//

#include "MsgBubbleModel.h"

#include "../msg-bubble-delegate/MsgBubbleDelegate.h"

MsgBubbleModel::MsgBubbleModel(QObject *parent) {
}

MsgBubbleModel::~MsgBubbleModel() {
}

void MsgBubbleModel::addMsg(const ChatMessage &msg) {
    QStandardItem *item = new QStandardItem();
    item->setData(QVariant::fromValue(msg), MsgBubbleDelegate::MsgRole);
    appendRow(item);
}
