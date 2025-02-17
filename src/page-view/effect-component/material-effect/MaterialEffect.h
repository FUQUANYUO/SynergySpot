//
// Created by FU-QAQ on 2024/12/7.
//

#ifndef SYNERGYSPOT_MATERIALEFFECT_H
#define SYNERGYSPOT_MATERIALEFFECT_H

#include "../EffectComponentPlugin.h"
#include <QGraphicsBlurEffect>
#include <QGraphicsWidget>

class QGraphicsDropShadowEffect;

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
    explicit SS_API AcrylicWidget(QGraphicsItem * parent = nullptr);
    ~AcrylicWidget() = default;
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget) override;
};

#define g_pCommonEffect CommonEffect::getInstance()

class SS_API CommonEffect{
public:
    static CommonEffect * getInstance();
    static void destroyInstance();

    // set radius shadow for the widget
    QGraphicsDropShadowEffect * setShadowForWidgetBorder(int radius);

private:
    explicit CommonEffect(QWidget * parent = nullptr);
    ~CommonEffect();

private:
    static CommonEffect * _obj;

    QGraphicsDropShadowEffect * borderEffect = nullptr;
};


#endif//SYNERGYSPOT_MATERIALEFFECT_H
