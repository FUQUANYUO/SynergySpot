//
// Created by FU-QAQ on 2024/9/22.
//
#include <QPainter>
#include "EffectComponent.h"

AcrylicEffect::AcrylicEffect(QObject *parent)
    : QGraphicsBlurEffect(parent)
{
    setBlurRadius(20);
}
void AcrylicEffect::draw(QPainter *painter) {
    QGraphicsBlurEffect::draw(painter);
}
AcrylicWidget::AcrylicWidget(QGraphicsItem *parent)
    : QGraphicsWidget(parent)
{
    setGraphicsEffect(new AcrylicEffect(this));
}
void AcrylicWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QGraphicsWidget::paint(painter, option, widget);
    QPainterPath path;
    path.addRect(rect());

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setClipPath(path);
    painter->setBrush(Qt::transparent);
    painter->setPen(Qt::NoPen);

    // 绘制亚克力效果
    QColor acrylicColor(255, 255, 255, 150);
    painter->setBrush(acrylicColor);
    painter->drawPath(path);
}
