//
// Created by FU-QAQ on 2024/2/28.
//

#ifndef SYNERGYSPOT_MESSAGELIST_H
#define SYNERGYSPOT_MESSAGELIST_H
#include <QListView>

#include "help.h"

class MessageList : public QObject{
    Q_OBJECT
public:
    MessageList() = default;
    explicit MessageList(QListView &lv);
    ~MessageList();
signals:
    void SELECTED_MSGITEM(std::string ssid);
public slots:
    void Add_ContactItem(std::string ssid);
};

#endif//SYNERGYSPOT_MESSAGELIST_H
