//
// Created by FU-QAQ on 2024/2/28.
//

#ifndef SYNERGYSPOT_MESSAGELIST_H
#define SYNERGYSPOT_MESSAGELIST_H
#include <QListView>

class MessageList : public QObject{
    Q_OBJECT
public:
    MessageList() = default;
    MessageList(QListView &lv);
    ~MessageList() = default;
signals:
    void SelectedMsgItem(std::string ssid);
};


#endif//SYNERGYSPOT_MESSAGELIST_H
