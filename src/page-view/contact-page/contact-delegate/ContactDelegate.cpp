//
// Created by FU-QAQ on 2024/12/17.
//

#include "ContactDelegate.h"

#include <QPainter>
#include <QPainterPath>

void ContactDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    // 父节点则PASS
    if(!index.parent().isValid()){
        QStyledItemDelegate::paint(painter, option, index);
        return ;
    }

    QStyleOptionViewItem myOption = option;
    initStyleOption(&myOption, index);

    // 绘制背景
    if (myOption.state & QStyle::State_Selected) {
        painter->fillRect(myOption.rect, QColor(  0, 153, 255));
    } else if(myOption.state & QStyle::State_MouseOver) {
        painter->fillRect(myOption.rect, QColor(  235, 235, 235));
    }
    else {
        painter->fillRect(myOption.rect, Qt::white);
    }

    int avatarSize = 50;
    QRect avatarRect(myOption.rect.left() + 5, myOption.rect.top() + (myOption.rect.height() - avatarSize) / 2, avatarSize, avatarSize);

    // 绘制圆角头像
    QIcon avatar = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
    QPixmap pixmap = avatar.pixmap(avatarSize, avatarSize);
    painter->setRenderHint(QPainter::Antialiasing); // 开启抗锯齿
    QPainterPath path;
    path.addEllipse(avatarRect);
    painter->setClipPath(path);
    painter->drawPixmap(avatarRect.topLeft(), pixmap);
    painter->setClipping(false);

    // 绘制用户名
    QString username = index.data(Qt::DisplayRole).toString();
    painter->setPen(Qt::black);
    QFont fontOld = painter->font();
    QFont fontForName = painter->font();
    fontForName.setPointSize(12);
    fontForName.setBold(true);
    painter->setFont(fontForName);
    QFontMetrics fm(painter->font());
    QRect usernameRect(avatarRect.right() + 10, myOption.rect.top() + 10, myOption.rect.width() - avatarRect.width() - 20, fm.height());
    painter->drawText(usernameRect, Qt::AlignLeft | Qt::AlignVCenter, username);
    painter->setFont(fontOld);
//    QString alias = "";
//    painter->setPen(Qt::black);
//    painter->drawText(usernameRect, Qt::AlignLeft | Qt::AlignVCenter, alias);

    // 绘制状态
    QString status = index.data(ContactDelegate::StatusRole).toString();
    if (status.contains("在线")) {
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setBrush(QColor(33, 228, 137));
        painter->setPen(Qt::NoPen);
        QRect circleRect(usernameRect.right() - 10, usernameRect.center().y() - 5, 10, 10);
        painter->drawEllipse(circleRect);
    }

//    QString description = "";
//    painter->setPen(Qt::black);
//    QRect descriptionRect(myOption.rect.left() + 5, myOption.rect.bottom() - fm.height() - 5, myOption.rect.width() - 10, fm.height());
//    painter->drawText(descriptionRect, Qt::AlignRight | Qt::AlignBottom, description);
}


QSize ContactDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if(!index.parent().isValid()){
        return QStyledItemDelegate::sizeHint(option,index);
    }
    QFontMetrics fm(option.font);
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(70); // 设置固定高度
    return size;
}
