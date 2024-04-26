//
// Created by FU-QAQ on 2024/3/13.
//

#ifndef SYNERGYSPOT_MSGCONTENTSHOW_H
#define SYNERGYSPOT_MSGCONTENTSHOW_H

#include <QObject>
#include <QListView>

class MsgContentShow : public QObject {
    Q_OBJECT
public:
    MsgContentShow() = default;
    MsgContentShow(QListView *lv,QObject *obl = nullptr);

    // 添加新的项
    void addNewItem(const std::string& ssid,const std::string& content);

    ~MsgContentShow();
signals:
    void PRESENT_SENDCONTENT(QString sendStr);
private:
    // ui view
    QListView * _lv;
};


#endif//SYNERGYSPOT_MSGCONTENTSHOW_H
