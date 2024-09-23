//
// Created by FU-QAQ on 2024/9/10.
//
#include <QMovie>
#include <QPainter>
#include <QVBoxLayout>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsView>

#include "ArchPage.h"
#include "../effect-component/EffectComponent.h"

namespace SSUi {
    ArchPageWindow::ArchPageWindow(QWidget *parent)
        : ElaWindow(parent)
    {
    }
    ArchPageWindow::~ArchPageWindow()
    {
    }
    void ArchPageWindow::setGifForBackground(QWidget *target, QMovie *m) {
        if(m->isValid()){
            _m = m;
        }
        else {
            LOG("setGifForBackground : " << m->lastErrorString().toStdString())
            return;
        }
        _m->setCacheMode(QMovie::CacheAll);
        _m->start();

        _t = new QTimer(this);
        connect(_t,&QTimer::timeout,this,[=](){
            update();
        });
        _t->start(16);

        // 云母材质
        setIsEnableMica(true);
    }
    void ArchPageWindow::paintEvent(QPaintEvent *event) {
        ElaWindow::paintEvent(event);
        QPainter painter(this);
        if (_m && _m->isValid()) {
            QPixmap pixmap = _m->currentPixmap();

            painter.drawPixmap(0, 0, width(), height(), pixmap);
        }
    }
    void ArchPageWindow::focusInEvent(QFocusEvent *event)  {
        QWidget::focusInEvent(event);
        if (_m && _m->state() != QMovie::Running) {
            _m->start();
            _t->start(16);
        }
    }
    void ArchPageWindow::focusOutEvent(QFocusEvent *event)  {
        QWidget::focusOutEvent(event);
        if (_m && _m->state() == QMovie::Running) {
            _m->stop();
            _t->stop();
        }
    }
    bool ArchPageWindow::eventFilter(QObject *watched, QEvent *event) {
        return ElaWindow::eventFilter(watched, event);
    }
    void ArchPageWindow::resizeEvent(QResizeEvent *event) {
        ElaWindow::resizeEvent(event);
    }
    void ArchPageWindow::moveEvent(QMoveEvent *event) {
        ElaWindow::moveEvent(event);
    }
    QMenu *ArchPageWindow::createPopupMenu() {
        return ElaWindow::createPopupMenu();
    }


    ArchPageWidget::ArchPageWidget(QWidget *parent)
        : ElaWidget(parent)
    {
    }
    ArchPageWidget::~ArchPageWidget()
    {
    }
    void ArchPageWidget::setGifForBackground(QWidget *target, QMovie *m) {
        if(m->isValid()){
            _m = m;
        }
        else {
            LOG("setGifForBackground : " << m->lastErrorString().toStdString())
            return;
        }
        _m->setCacheMode(QMovie::CacheAll);
        _m->start();

        _t = new QTimer(this);
        connect(_t,&QTimer::timeout,this,[=](){
            update();
        });
        _t->start(16);
    }
    void ArchPageWidget::paintEvent(QPaintEvent *event) {
        ElaWidget::paintEvent(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        if (_m && _m->isValid()) {
            QPixmap pixmap = _m->currentPixmap();

            painter.drawPixmap(0, 0, width(), height(), pixmap);
        }
    }

    void ArchPageWidget::focusInEvent(QFocusEvent *event) {
        QWidget::focusInEvent(event);
        if (_m && _m->state() != QMovie::Running) {
            _m->start();
            _t->start(16);
        }
    }
    void ArchPageWidget::focusOutEvent(QFocusEvent *event) {
        QWidget::focusOutEvent(event);
        if (_m && _m->state() == QMovie::Running) {
            _m->stop();
            _t->stop();
        }
    }
    void ArchPageWidget::moveEvent(QMoveEvent *event) {
        QWidget::moveEvent(event);
    }
    void ArchPageWidget::resizeEvent(QResizeEvent *event) {
        QWidget::resizeEvent(event);
    }
    bool ArchPageWidget::eventFilter(QObject *watched, QEvent *event) {
        return QObject::eventFilter(watched, event);
    }
    QMenu *ArchPageWidget::createPopupMenu() {
        return nullptr;
    }
    void ArchPageWidget::setAcrylicMaskAboveBackground(QWidget * target) {
        if(_layout == nullptr)
            _layout = new QVBoxLayout(target);
        auto scene = new QGraphicsScene();
        auto view = new QGraphicsView(scene);
        auto acyWidget = new AcrylicWidget();
        acyWidget->setMinimumSize(0,0);
        acyWidget->setMaximumSize(target->width(),target->height());
        acyWidget->resize(target->width(),target->height()-55);
        scene->addItem(acyWidget);

        view->setObjectName("view");
        view->setStyleSheet("QGraphicsView#view{background-color: rgba(0,0,0,0);border: none;}");
        view->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
        view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        _layout->addWidget(view);
        _layout->setContentsMargins(0, 0, 0, 0);
        target->setLayout(_layout);
    }

} // SSUi
