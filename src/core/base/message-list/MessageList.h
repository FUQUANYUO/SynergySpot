//
// Created by FU-QAQ on 2024/2/28.
//

#ifndef SYNERGYSPOT_MESSAGELIST_H
#define SYNERGYSPOT_MESSAGELIST_H

#include <QStyledItemDelegate>
#include <QStandardItemModel>

// Listview Model
class MessageListModel : QStandardItemModel
{
public:
    MessageListModel(QObject *parent = nullptr);
};

// delegate
class MessageListDelegate : public QStyledItemDelegate
{
public:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};


class MessageList {
public:
    MessageList();
    ~MessageList() = default;
};


#endif//SYNERGYSPOT_MESSAGELIST_H
