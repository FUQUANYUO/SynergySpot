//
// Created by FUQAQ on 2025/2/18.
//

#ifndef LOADINGDIALOG_H
#define LOADINGDIALOG_H

#include <QDialog>
#include <QTimer>
#include <QPainter>

class LoadingDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoadingDialog(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void showEvent(QShowEvent* event) override;

    bool eventFilter(QObject *obj, QEvent *event);
private:
    void updatePosition();
private:
    struct DotState {
        qreal baseAngle;  // 基础角度
        qreal phase;      // 相位偏移
    };

    QTimer m_animTimer;
    qreal m_angle = 0.0;     // 当前旋转角度
    QList<DotState> m_dots;

    qreal m_spacingFactor = 1.0;
    qreal m_spacingPhase = 0.0;

    static constexpr int DOT_COUNT = 7;       // 圆点数量
    static constexpr qreal RADIUS = 30.0;      // 旋转半径
    static constexpr qreal BASE_SIZE = 5.0;    // 基础半径
    static constexpr qreal MAX_SCALE = 1.8;    // 最大缩放系数
};

#endif //LOADINGDIALOG_H
