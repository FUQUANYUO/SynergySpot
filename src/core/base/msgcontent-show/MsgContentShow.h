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

    ~MsgContentShow();
signals:
    void PRESENT_SENDCOTENT(QString sendStr);
private:
    // ui view
    QListView * _lv;
};


#endif//SYNERGYSPOT_MSGCONTENTSHOW_H
