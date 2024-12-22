//
// Created by FU-QAQ on 2024/12/7.
//

#include "MaterialEffect.h"
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <mutex>

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

CommonEffect * CommonEffect::_obj = nullptr;
std::mutex m;

CommonEffect *CommonEffect::getInstance() {
    if(!_obj){
        m.lock();
        if(!_obj){
            _obj = new CommonEffect();
        }
        m.unlock();
    }
    return _obj;
}
void CommonEffect::destroyInstance() {
    if(_obj){
        m.lock();
        if(_obj){
            delete _obj;
        }
        m.unlock();
    }
}
CommonEffect::CommonEffect(QWidget *parent) {
}
CommonEffect::~CommonEffect() {
    delete borderEffect;
}
QGraphicsDropShadowEffect *CommonEffect::setShadowForWidgetBorder(int radius) {
    borderEffect = new QGraphicsDropShadowEffect;
    borderEffect->setBlurRadius(radius);
    borderEffect->setOffset(0, 0);
    return borderEffect;
}
