//
// Created by FU-QAQ on 2024/4/2.
//

#ifndef SYNERGYSPOT_CONTACTLIST_H
#define SYNERGYSPOT_CONTACTLIST_H
#include <QTreeView>
#include "help.h"


class ContactList : public QObject{
    Q_OBJECT
public:
    ContactList() = default;
    
    explicit ContactList(QHash<QString,QTreeView*> ltv,QObject * obj);

    ~ContactList();
signals:
    void SELECTED_CONTACTITEM(std::string ssid);
private:
    void getContactListRequest(QWidget* obj);
private:
    QHash<QString,QTreeView*> _ltv;

    std::vector<QObject*> delVec;
};

#endif//SYNERGYSPOT_CONTACTLIST_H
