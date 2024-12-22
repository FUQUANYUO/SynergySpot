//
// Created by FU-QAQ on 2024/12/17.
//

#ifndef SYNERGYSPOT_CONTACTMODEL_H
#define SYNERGYSPOT_CONTACTMODEL_H

#include <QStandardItemModel>

struct GroupingItem{
    QString ssid;
    QString name;
    QString mark;
    QString details;
    QString status;
    QPixmap pic;
};

class ContactModel : public QStandardItemModel{
public:
    explicit ContactModel(QObject * parent = nullptr);
    ~ContactModel() = default;

    // add grouping
    void addGrouping(const QString &groupingName);

    // add item
    void addGroupingItem(const QString &groupingName,const GroupingItem &item);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
private:
    QStandardItem * getGrouping(const QString &groupingName);
private:
    QMap<QPair<QString,QStandardItem*>,
         QList<QPair<GroupingItem,QStandardItem*>>> _itemMap;
};


#endif//SYNERGYSPOT_CONTACTMODEL_H
