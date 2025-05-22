//
// Created by FU-QAQ on 2025/5/21.
//

#ifndef EMOJIPICKERPAGE_H
#define EMOJIPICKERPAGE_H

#include <QWidget>
#include <QTableWidget>

class EmojiPickerPage : public QWidget  {
    Q_OBJECT
signals:
    void sigEmojiSelected(const QString &emoji);
public:
    EmojiPickerPage(QWidget *parent = nullptr);

private slots:
    void sltCellClicked(int row, int col);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    void loadEmojis();

    QTableWidget *table;

    QList<uint> emojis = {
        0x1F600, 0x1F601, 0x1F602, 0x1F603, 0x1F604, 0x1F605,
        0x1F606, 0x1F609, 0x1F60A, 0x1F60B, 0x1F60C, 0x1F60D,
        0x1F60E, 0x1F60F, 0x1F610, 0x1F611, 0x1F612, 0x1F613,
        0x1F614, 0x1F615, 0x1F616, 0x1F617, 0x1F618, 0x1F619
    };
};



#endif //EMOJIPICKERPAGE_H
