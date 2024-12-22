//
// Created by FU-QAQ on 2024/12/19.
//

#ifndef MSGBUBBLEMODEL_H
#define MSGBUBBLEMODEL_H

#include <QStandardItemModel>

struct ChatMessage{
    QString ssid;           // id
    QString name;           // name
    QString content;        // html rich text
    QString avatarPath;     // pic path
    bool isOwnMessage;      // distinguish between sender and receiver
};

Q_DECLARE_METATYPE(ChatMessage)

class MsgBubbleModel : public QStandardItemModel{
public:
    explicit MsgBubbleModel(QObject *parent = nullptr);
    ~MsgBubbleModel();

    void addMsg(const ChatMessage& msg);
};

#endif //MSGBUBBLEMODEL_H
