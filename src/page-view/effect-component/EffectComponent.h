//
// Created by FU-QAQ on 2024/9/12.
//

#ifndef SYNERGYSPOT_EFFECT_COMPONENT_H
#define SYNERGYSPOT_EFFECT_COMPONENT_H

#include "define.h"
#include <QGraphicsBlurEffect>
#include <QGraphicsWidget>

// acrylic style
class AcrylicEffect : public QGraphicsBlurEffect{
public:
    explicit AcrylicEffect(QObject *parent = nullptr);
    ~AcrylicEffect() = default;
protected:
    void draw(QPainter *painter);
};

class AcrylicWidget : public QGraphicsWidget{
public:
    explicit SS_API_OUT AcrylicWidget(QGraphicsItem * parent = nullptr);
    ~AcrylicWidget() = default;
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget) override;
};


#endif//SYNERGYSPOT_EFFECT_COMPONENT_H
