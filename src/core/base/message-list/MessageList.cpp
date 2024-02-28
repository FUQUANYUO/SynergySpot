//
// Created by FU-QAQ on 2024/2/28.
//
#include <QListView>
#include "MessageList.h"

MessageList::MessageList() {

}

QSize MessageListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    return {213, 80}; // 设置每个项的大小
}

void MessageListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyledItemDelegate::paint(painter, option, index);

}
