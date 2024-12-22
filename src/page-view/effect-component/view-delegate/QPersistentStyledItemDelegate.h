//
// Created by FU-QAQ on 2024/12/17 Copy from https://blog.csdn.net/qq_34305316/article/details/90298426
// the editor is H&A (CSDN Name)
//

#ifndef SYNERGYSPOT_QPERSISTENTSTYLEDITEMDELEGATE_H
#define SYNERGYSPOT_QPERSISTENTSTYLEDITEMDELEGATE_H

/* 持久代理类 by pp.xue
 *
 * 这是该需求的简易实现，使用时需注意：
 * 1、View需要将EditTrigger设置为QAbstractItemView::NoEditTriggers，关闭默认的Delegate逻辑
 * 2、继承这个类，实现自定义的 持久代理
 *
 *  继承时：不能也不必重写paint、updateEditorGeometry函数：
 *      前者是因为paint本应该做的绘制工作已没有意义（被编辑器覆盖，绘制了也看不到），
 *              且在本类中用paint实现了根本逻辑，不能被覆盖。
 *      后者是因为本类的paint中会一直重设编辑器的尺寸
 *  其他函数可随需要重写，不做要求。
 *
 * 3、如果需要将数据从编辑器保存到model中，那么需要连接信号槽 到本类的updateModelData，它会调用子类的
 *
 *  void QStyledItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
 *
 * 因此，此时我们也需要实现这个函数。这也是最重要的额外步骤。
*/

#include <QStyledItemDelegate>
#include <QWidget>
#include <QMap>
#include <QPersistentModelIndex>

class QPersistentStyledItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    QPersistentStyledItemDelegate(QObject *parent = Q_NULLPTR);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const Q_DECL_OVERRIDE final;

public slots:
    void updateModelData();
private slots:
    void clearWidget();
    void updateWidget(QModelIndex,QModelIndex);
private:
    mutable QMap<QPersistentModelIndex,QWidget *> m_iWidgets;
};

#endif//SYNERGYSPOT_QPERSISTENTSTYLEDITEMDELEGATE_H
