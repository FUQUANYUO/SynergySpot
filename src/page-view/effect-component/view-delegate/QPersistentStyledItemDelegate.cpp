//
// Created by FU-QAQ on 2024/12/17 Copy from https://blog.csdn.net/qq_34305316/article/details/90298426
// the editor is H&A (CSDN Name)
//

#include "QPersistentStyledItemDelegate.h"
#include <QDebug>
#include <QPainter>
#include <QItemSelection>

QPersistentStyledItemDelegate::QPersistentStyledItemDelegate(QObject *parent)
    :QStyledItemDelegate(parent)
{

}

/*
 * 本类的本质是为每个item（对应QPersistentModelIndex）保存一个widget，且维护Model，view与之相关的关系。
 *
*/
void QPersistentStyledItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QPersistentModelIndex perIndex(index);

    QAbstractItemModel *model = const_cast<QAbstractItemModel *>(index.model());
    //如果model有删除、重置，则删除对应的widget
    //如果model有新增，view会调用paint绘制，因此不需做额外工作
    connect(model,SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this,SLOT(clearWidget()),Qt::UniqueConnection);
    connect(model,SIGNAL(columnsRemoved(QModelIndex,int,int)),
            this,SLOT(clearWidget()),Qt::UniqueConnection);
    connect(model,SIGNAL(destroyed(QObject*)),
            this,SLOT(clearWidget()),Qt::UniqueConnection);
    connect(model,SIGNAL(modelReset()),this,SLOT(clearWidget()),Qt::UniqueConnection);
    //如果model有数据变化，更新变化到编辑器
    connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),this,SLOT(updateWidget(QModelIndex,QModelIndex)));

    if(!m_iWidgets.contains(perIndex))
    {
        QWidget *parentWidget = static_cast<QWidget *>(painter->device());
        if(nullptr == parentWidget)
            return;

        QWidget *tempWidget = this->createEditor(parentWidget,option,index);
        this->setEditorData(tempWidget,index);
        tempWidget->setGeometry(option.rect);
        tempWidget->setVisible(true);
        m_iWidgets.insert(perIndex,tempWidget);
    }
    else
    {
        QWidget *tempWidget = m_iWidgets.value(perIndex);
        if(tempWidget)
        {
            tempWidget->setGeometry(option.rect);
        }
    }
}

//如果子类的编辑器需要将数据回写到model，则需要连接到这个槽
//它的主要作用是：调用了子类重写的setModelData
void QPersistentStyledItemDelegate::updateModelData()
{
    QObject *sender = this->sender();
    if(nullptr == sender)
        return;

    QWidget *editor = static_cast<QWidget *>(sender);
    if(nullptr == editor)
        return;

    if(!m_iWidgets.values().contains(editor))
        return;

    QPersistentModelIndex perIndex = m_iWidgets.key(editor);
    if(!perIndex.isValid())
        return;

    QModelIndex index = static_cast<QModelIndex>(perIndex);
    if(!index.isValid())
        return;

    QAbstractItemModel *model = const_cast<QAbstractItemModel *>(index.model());
    this->setModelData(editor,model,index);

    emit model->dataChanged(index,index);
}

//清理已无用的代理Widget
void QPersistentStyledItemDelegate::clearWidget()
{
    auto i = m_iWidgets.begin();
    while (i != m_iWidgets.end()) {
        if(!i.key().isValid())
        {
            i.value()->setParent(nullptr);
            i.value()->deleteLater();
            i = m_iWidgets.erase(i);

        }
        else
        {
            ++i;
        }
    }
}

//更新数据到delegate
void QPersistentStyledItemDelegate::updateWidget(QModelIndex begin, QModelIndex end)
{
    QItemSelection selection(begin,end);
    QModelIndexList list = selection.indexes();

    foreach (QModelIndex index, list) {
        QPersistentModelIndex perIndex(index);
        if(m_iWidgets.contains(perIndex))
        {
            QWidget *tempWidget = m_iWidgets.value(perIndex);
            if(tempWidget)
            {
                this->setEditorData(tempWidget,index);
            }
        }
    }
}