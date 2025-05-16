//
// Created by FU-QAQ on 2024/12/19.
//

#include "MsgBubbleDelegate.h"

#include "../msg-bubble-model/MsgBubbleModel.h"

#include <QTextDocument>
#include <QPainter>
#include <QPainterPath>

MsgBubbleDelegate::MsgBubbleDelegate(QObject *parent) : QStyledItemDelegate(parent) {
    connect(eTheme,&ElaTheme::themeModeChanged,this,[=](ElaThemeType::ThemeMode mode) {
        _theme = mode;
    });
}

void MsgBubbleDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto msg = index.data(MsgRole).value<ChatMessage>();
    // 填充默认头像
    if (msg.avatarPath.isEmpty() || msg.avatarPath == "-1") {
        msg.avatarPath = ":/message-page/rc-page/img/SS-default-icon.jpg";
    }
    // 计算可用宽度
    int availableWidth = option.rect.width() - avatarSize * 2 - padding * 6;

    // 修改样式并计算内容大小
    QTextDocument doc;
    QString coloredContent = QString("<span style='color: %1;'>%2</span>").arg(msg.isOwnMessage ? "#FFFFFF" : "#000000").arg(msg.content);
    doc.setDefaultFont(QFont("微软雅黑", fontSize));
    QString modifiedContent = coloredContent;
    modifiedContent.replace(QRegularExpression("<img"), QString("<img style='vertical-align: text-bottom;max-width: %1px;'").arg(availableWidth - padding*2));
    doc.setHtml(modifiedContent);

    qreal idealWidth = doc.idealWidth();
    int finalWidth = static_cast<int>(std::min(idealWidth, static_cast<qreal>(availableWidth)));
    doc.setTextWidth(finalWidth + 1);

    // 计算实际高度
    QSizeF docSize = doc.size();
    QSize contentSize = {finalWidth,static_cast<int>(std::ceil(docSize.height()))};

    // 计算气泡区域
    QRect bubbleRect;
    if (isGroupMode) {
        QFont font = painter->font();
        font.setPointSize(10);
        font.setFamily("微软雅黑");
        QFontMetrics ms(font);
        int nameWidth = ms.horizontalAdvance(msg.name);
        int nameHeight = ms.height();

        if (msg.isOwnMessage) {
            // 绘制名字
            QPoint pos(option.rect.right() - nameWidth - padding * 2 - avatarSize,
                option.rect.top() + nameHeight);
            drawName(painter,pos,msg.name);

            bubbleRect = QRect(
                option.rect.right() - contentSize.width() - padding * 4 - avatarSize,
                option.rect.top() + padding + namePadding,
                contentSize.width() + padding * 2,
                contentSize.height() + padding * 2
            );
        } else {
            // 绘制名字
            QPoint pos(option.rect.left() + avatarSize + padding * 2,
                    option.rect.top() + nameHeight);
            drawName(painter,pos,msg.name);

            bubbleRect = QRect(
                option.rect.left() + avatarSize + padding * 2,
                option.rect.top() + padding + namePadding,
                contentSize.width() + padding * 2,
                contentSize.height() + padding * 2
            );
        }
    }else {
        if (msg.isOwnMessage) {
            bubbleRect = QRect(
                option.rect.right() - contentSize.width() - padding * 4 - avatarSize,
                option.rect.top() + padding,
                contentSize.width() + padding * 2,
                contentSize.height() + padding * 2
            );
        } else {
            bubbleRect = QRect(
                option.rect.left() + avatarSize + padding * 2,
                option.rect.top() + padding,
                contentSize.width() + padding * 2,
                contentSize.height() + padding * 2
            );
        }
    }
    // 绘制气泡
    drawBubble(painter, bubbleRect, msg.isOwnMessage);

    // 绘制头像
    QRect avatarRect = msg.isOwnMessage
        ? QRect(option.rect.right() - avatarSize - padding, option.rect.top() + padding, avatarSize, avatarSize)
        : QRect(option.rect.left() + padding, option.rect.top() + padding, avatarSize, avatarSize);
    drawAvatar(painter, avatarRect, msg.avatarPath);

    // 绘制内容
    QRect contentRect = bubbleRect.adjusted(padding, padding, -padding, -padding);
    painter->save();
    painter->translate(contentRect.topLeft());
    doc.setTextWidth(contentRect.width() + 1);
    doc.drawContents(painter);
    painter->restore();
}

QSize MsgBubbleDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto msg = index.data(MsgRole).value<ChatMessage>();

    // 计算可用宽度
    int availableWidth = option.rect.width() - avatarSize * 2 - padding * 6;

    QTextDocument doc;
    QString coloredContent = QString("<span style='color: %1;'>%2</span>").arg(msg.isOwnMessage ? "#FFFFFF" : "#000000").arg(msg.content);
    doc.setDefaultFont(QFont("微软雅黑", fontSize));
    QString modifiedContent = coloredContent;
    modifiedContent.replace(QRegularExpression("<img"), QString("<img style='vertical-align: text-bottom;max-width: %1px;'").arg(availableWidth - padding*2));
    doc.setHtml(modifiedContent);

    qreal idealWidth = doc.idealWidth();
    int finalWidth = static_cast<int>(std::min(idealWidth, static_cast<qreal>(availableWidth)));
    doc.setTextWidth(finalWidth + 1);

    // 计算实际高度
    QSizeF docSize = doc.size();
    QSize contentSize = {finalWidth,static_cast<int>(std::ceil(docSize.height()))};

    // 计算整个气泡所需的总高度
    // 内容高度 + 上下内边距 + 气泡边距(20)
    int totalHeight = 0;
    if (isGroupMode)
        totalHeight = contentSize.height() + padding * 3 + 20;
    else
        totalHeight = contentSize.height() + padding * 3 + 20 + namePadding;

    return QSize(option.rect.width(), std::max(totalHeight + padding * 2, avatarSize + padding * 4));
}

void MsgBubbleDelegate::setGroupMode(bool isEnable) {
    isGroupMode = isEnable;
}

void MsgBubbleDelegate::drawBubble(QPainter *painter, const QRect &rect, bool isOwnMessage) const {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);
    painter->setBrush(isOwnMessage ? QColor(0, 153, 255) : QColor(Qt::white));
    painter->drawRoundedRect(rect, 10, 10);
    painter->restore();
}

void MsgBubbleDelegate::drawAvatar(QPainter *painter, const QRect &rect, const QString &avatarPath) const {
    QPixmap avatar(avatarPath);
    if (!avatar.isNull()) {
        painter->setRenderHint(QPainter::Antialiasing);
        QPainterPath path;
        path.addEllipse(rect);
        painter->setClipPath(path);
        painter->drawPixmap(rect,avatar.scaled(rect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        painter->setClipping(false);
    }
}

void MsgBubbleDelegate::drawName(QPainter *painter, const QPoint &startPoint,const QString &name) const {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // 设置文字样式
    QFont font = painter->font();
    font.setPointSize(10); // 设置字体大小
    font.setFamily("微软雅黑");
    painter->setFont(font);
    painter->setPen(Qt::gray);

    painter->drawText(startPoint, name);
    painter->restore();
}
