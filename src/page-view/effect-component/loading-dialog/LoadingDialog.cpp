//
// Created by FUQAQ on 2025/2/18.
//

#include "LoadingDiaLog.h"
#include <QBoxLayout>
#include <QDateTime>
#include <QPaintEvent>
#include <QLabel>
#include <QGraphicsDropShadowEffect>

LoadingDialog::LoadingDialog(QWidget *parent)
    : QDialog(parent)
{
    setFixedSize(150, 150);
    // 窗口属性设置
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background:transparent;");

    // 初始化圆点状态
    for(int i=0; i<DOT_COUNT; ++i){
        DotState dot;
        dot.baseAngle = (360.0/DOT_COUNT)*i;
        dot.phase = (2*M_PI/DOT_COUNT)*i;
        m_dots.append(dot);
    }

    // 动画定时器
    connect(&m_animTimer, &QTimer::timeout, this, [this]{
        m_angle += 1.0 + 0.5*qSin(qDegreesToRadians(m_angle * 5));
        if(m_angle >= 360) m_angle -= 360;
        update();
    });

    if (parent) {
        parent->installEventFilter(this);
        updatePosition();
    }
}

void LoadingDialog::paintEvent(QPaintEvent*event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setClipRect(event->rect());

    const QPoint center = rect().center();
    const qreal time = QDateTime::currentMSecsSinceEpoch() / 1000.0;

    m_spacingFactor = 0.5 * (qSin(time * 2 + m_spacingPhase) + 1.5);
    m_spacingPhase += 0.02;

    for (int i = 0; i < DOT_COUNT; ++i) {
        qreal angleStep = (360.0 / DOT_COUNT) * m_spacingFactor;
        m_dots[i].baseAngle = angleStep * i + m_angle;
    }

    // 绘制所有圆点
    foreach(const DotState& dot, m_dots){
        // 计算当前角度（带全局旋转）
        qreal angle = qDegreesToRadians(dot.baseAngle + m_angle);

        // 计算位置
        qreal x = center.x() + RADIUS * qCos(angle);
        qreal y = center.y() + RADIUS * qSin(angle);
        x = qBound(0.0, x, width() - 1.0);
        y = qBound(0.0, y, height() - 1.0);
        QPointF pos(x, y);

        // 计算动态半径（sin波动）
        qreal scale = 0.5*(qSin(time*2 + dot.phase) + 1); // [0,1]
        qreal radius = BASE_SIZE * (1 + scale*(MAX_SCALE-1));

        // 设置透明度（根据半径变化）
        painter.setPen(Qt::NoPen);
        QRadialGradient grad(pos, radius);
        grad.setColorAt(0, QColor(200,200,200,200));
        grad.setColorAt(1, QColor(200,200,200,50));
        painter.setBrush(grad);

        painter.drawEllipse(pos, radius, radius);
    }
}

void LoadingDialog::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);
    m_animTimer.start(16); // ~60fps

    updatePosition();
}

bool LoadingDialog::eventFilter(QObject *obj, QEvent *event) {
    if (obj == parent() && event->type() == QEvent::Move) {
        updatePosition();
    }
    return QDialog::eventFilter(obj, event);
}

void LoadingDialog::updatePosition() {
    if (parentWidget()) {
        QPoint parentCenter = parentWidget()->geometry().center();

        QPoint dialogPos = parentCenter - QPoint(width() / 2, height() / 2);

        move(dialogPos);
    }
}