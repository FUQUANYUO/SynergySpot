//
// Created by FU-QAQ on 2024/12/21.
//

#ifndef MSGBUBBLEVIEW_H
#define MSGBUBBLEVIEW_H

#include "ela-widget-tools/ElaListView.h"

class MsgBubbleView : public ElaListView{
    Q_OBJECT
public:
    explicit MsgBubbleView(QWidget *parent = nullptr);
    ~MsgBubbleView() override = default;
protected:
    void resizeEvent(QResizeEvent *event) override;
};



#endif //MSGBUBBLEVIEW_H
