//
// Created by FU-QAQ on 2024/9/10.
//

#ifndef SYNERGYSPOT_ARCHPAGE_H
#define SYNERGYSPOT_ARCHPAGE_H

#include "define.h"
#include "help.h"
#include "ela-widget-tools/ElaWindow.h"
#include "ela-widget-tools/ElaWidget.h"

class QMovie;
class QVBoxLayout;

namespace SSUi {
    class SS_API_OUT ArchPageWindow : public ElaWindow {
    public:
        virtual void initWindow() = 0;
        virtual void initEdgeLayout() = 0;
        virtual void initContent() = 0;
    protected:
        virtual void initConnectFunc() = 0;

        explicit ArchPageWindow(QWidget *parent = nullptr);
        ~ArchPageWindow() override;

        // you must set strategy for free movie sc when you didn't focus it
        virtual void setGifForBackground(QWidget* target, QMovie* m);
        void paintEvent(QPaintEvent *event) override;
        void focusInEvent(QFocusEvent *event) override;
        void focusOutEvent(QFocusEvent *event) override;

        void moveEvent(QMoveEvent* event) override;
        void resizeEvent(QResizeEvent* event) override;
        bool eventFilter(QObject* watched, QEvent* event) override;
        QMenu* createPopupMenu() override;
    private:
        // if you want to have a gif as BG, that variable can help you storage sc of gif
        QMovie* _m = nullptr;
        QTimer* _t = nullptr;
    protected:

    };

    class SS_API_OUT ArchPageWidget : public ElaWidget{
    public:
        virtual void initWindow() = 0;
        virtual void initEdgeLayout() = 0;
        virtual void initContent() = 0;
    protected:
        virtual void initConnectFunc() = 0;

        explicit ArchPageWidget(QWidget *parent = nullptr);
        ~ArchPageWidget() override;

        // you must set strategy for free movie sc when you didn't focus it
        virtual void setGifForBackground(QWidget* target, QMovie* m);
        void paintEvent(QPaintEvent *event) override;
        void focusInEvent(QFocusEvent *event) override;
        void focusOutEvent(QFocusEvent *event) override;

        // set an Acrylic style mask for BG
        void setAcrylicMaskAboveBackground(QWidget * target);

        void moveEvent(QMoveEvent* event) override;
        void resizeEvent(QResizeEvent* event) override;
        bool eventFilter(QObject* watched, QEvent* event) override;
        virtual QMenu* createPopupMenu();
    private:
        // if you want to have a gif as BG, that variable can help you storage sc of gif
        QMovie* _m = nullptr;
        QTimer* _t = nullptr;
        QVBoxLayout* _layout = nullptr;
    };

} // SSUi

#endif //SYNERGYSPOT_ARCHPAGE_H
