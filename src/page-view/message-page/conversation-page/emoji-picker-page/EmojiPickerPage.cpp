//
// Created by FU-QAQ on 2025/5/21.
//

#include "EmojiPickerPage.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPainter>
#include <QPainterPath>

EmojiPickerPage::EmojiPickerPage(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);

    setFixedSize(340,150);
    table = new QTableWidget(this);
    table->setColumnCount(8);
    table->horizontalHeader()->setVisible(false);
    table->verticalHeader()->setVisible(false);
    table->setShowGrid(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->horizontalHeader()->setDefaultSectionSize(40);  // 固定列宽
    table->verticalHeader()->setDefaultSectionSize(40);    // 固定行高
    table->setMouseTracking(true);  // 启用鼠标跟踪

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(table);
    setContentsMargins(10,10,10,10);

    connect(table, &QTableWidget::cellClicked, this, &EmojiPickerPage::sltCellClicked);

    loadEmojis();

    setStyleSheet(R"(
        QTableWidget {
            background: transparent;
            border: none;
        }

        QTableWidget::item {
            background-color: #F0F0F0;
            border-radius: 3px;
            margin: 2px;
            min-width: 40px;
            min-height: 40px;
            text-align: center;
            font-size: 18px;
        }

        QTableWidget::item:hover {
            background-color: #C8E6FF;
            border: 1px solid #80B3FF;
        }

        QScrollBar:vertical {
            width: 8px;
            background: #F0F0F0;
        }

        QScrollBar::handle:vertical {
            background: #C0C0C0;
            border-radius: 4px;
        }
    )");
}

void EmojiPickerPage::sltCellClicked(int row, int col) {
    if (auto item = table->item(row, col)) {
        emit sigEmojiSelected(item->text());
        close();
    }
}

void EmojiPickerPage::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    // 绘制自定义圆角矩形
    QPainterPath path;
    path.addRoundedRect(rect(), 20, 20);
    painter.fillPath(path, QColor(255, 255, 255, 230)); // 半透明白色背景
}

void EmojiPickerPage::loadEmojis(){
    int rowCount = emojis.size() / 8 + (emojis.size() % 8 ? 1 : 0);
    table->setRowCount(rowCount);

    for (int i = 0; i < emojis.size(); ++i) {
        int row = i / 8;
        int col = i % 8;
        QString emoji = QString::fromUcs4(&emojis[i], 1);
        QTableWidgetItem *item = new QTableWidgetItem(emoji);
        item->setTextAlignment(Qt::AlignCenter);
        table->setItem(row, col, item);
    }
}