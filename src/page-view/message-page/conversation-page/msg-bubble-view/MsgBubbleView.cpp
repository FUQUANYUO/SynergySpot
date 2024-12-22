//
// Created by FU-QAQ on 2024/12/21.
//

#include "MsgBubbleView.h"

#include <QScrollBar>

MsgBubbleView::MsgBubbleView(QWidget *parent) : ElaListView(parent){
}

void MsgBubbleView::resizeEvent(QResizeEvent *event) {
    QListView::resizeEvent(event);
    this->viewport()->update();
}