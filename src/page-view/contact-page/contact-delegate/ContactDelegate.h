//
// Created by FU-QAQ on 2024/12/17.
//

#ifndef SYNERGYSPOT_CONTACTDELEGATE_H
#define SYNERGYSPOT_CONTACTDELEGATE_H

#include <QStyledItemDelegate>



class ContactDelegate : public QStyledItemDelegate {
public:
    ContactDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    enum CustomRoles {
        StatusRole = Qt::UserRole + 1,
        SSIDRole   = Qt::UserRole + 2
    };
};


#endif//SYNERGYSPOT_CONTACTDELEGATE_H
