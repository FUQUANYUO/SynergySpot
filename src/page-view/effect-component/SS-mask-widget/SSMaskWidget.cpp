//
// Created by FUQAQ on 2025/2/13.
//

#include "SSMaskWidget.h"

#include <QPainter>
#include <QPropertyAnimation>

SSMaskWidget::SSMaskWidget(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("SSMaskWidget");
    setStyleSheet("#SSMaskWidget { background-color: transparent; }");
}

SSMaskWidget::~SSMaskWidget() {}

void SSMaskWidget::startMaskAnimation(int end) {
    QPropertyAnimation* opacityAnimation = new QPropertyAnimation(this, "pMaskAlpha");
    connect(opacityAnimation, &QPropertyAnimation::valueChanged, this, [=](const QVariant& value) {
        update();
    });
    connect(opacityAnimation, &QPropertyAnimation::finished, this, [=]() {
        if (end == 0)
        {
            setVisible(false);
        }
    });
    opacityAnimation->setEasingCurve(QEasingCurve::InOutSine);
    opacityAnimation->setDuration(250);
    opacityAnimation->setStartValue(_pMaskAlpha);
    opacityAnimation->setEndValue(end);
    opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void SSMaskWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor(0x00, 0x00, 0x00, _pMaskAlpha));
    painter.drawRect(rect());
    painter.restore();
}