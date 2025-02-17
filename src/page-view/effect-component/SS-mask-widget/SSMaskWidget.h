//
// Created by FUQAQ on 2025/2/13.
//

#ifndef SSMASKWIDGET_H
#define SSMASKWIDGET_H

#include <QWidget>

#include "help.h"

class SSMaskWidget : public QWidget{
    Q_OBJECT
    Q_PROPERTY(int pMaskAlpha READ getMaskAlpha WRITE setMaskAlpha NOTIFY maskAlphaChanged)
    Q_SIGNAL void maskAlphaChanged();
public:
    explicit SSMaskWidget(QWidget* parent = nullptr);
    ~SSMaskWidget();

    void startMaskAnimation(int end);

    void setMaskAlpha(int alpha) {
        _pMaskAlpha = alpha;
        Q_EMIT maskAlphaChanged();
    }

    int getMaskAlpha() {
        return _pMaskAlpha;
    }

protected:
    virtual void paintEvent(QPaintEvent* event) override;
private:
    int _pMaskAlpha;
};



#endif //SSMASKWIDGET_H
