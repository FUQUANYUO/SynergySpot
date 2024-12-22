//
// Created by FU-QAQ on 2024/12/19.
//

#ifndef SYNERGYSPOT_MSGBUBBLEDELEGATE_H
#define SYNERGYSPOT_MSGBUBBLEDELEGATE_H

#include "define.h"

#include <QStyledItemDelegate>
#include <QTextDocument>

class ChatMessage;

class MsgBubbleDelegate : public QStyledItemDelegate {
public:
    explicit MsgBubbleDelegate(QObject* parent = nullptr);
    ~MsgBubbleDelegate() override = default;

    void paint(QPainter * painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    // enable mode which show send and receiver name
    void setGroupMode(bool isEnable);

    enum CustomRoles {
        MsgRole = Qt::UserRole + 1
    };

private:
    void drawBubble(QPainter *painter, const QRect &rect, bool isOwnMessage) const;
    void drawAvatar(QPainter *painter, const QRect &rect, const QString &avatarPath) const;
    void drawName(QPainter *painter, const QPoint &startPoint, const QString &name) const;
private:
    const int padding = 5;         // bubble padding
    const int avatarSize = 40;     // avatar size
    const int imageWidth = 150;    // fixed size img in the rich text
    const int imageHeight = 90;    // fixed size img in the rich text
    const int fontSize = 12;       // plain text size
    const int namePadding = 20;    // name height
    bool isGroupMode = false;
};


#endif//SYNERGYSPOT_MSGBUBBLEDELEGATE_H
