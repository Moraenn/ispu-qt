#include "taskdelegate.h"
#include "plannermodels.h"
#include <QApplication>
#include <QWidget>
#include <QStyleOptionViewItem>
#include <QBrush>
#include <QColor>
#include <QStyle>
void TaskDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const{
    QStyleOptionViewItem view(option);
    initStyleOption(&view, index);
    if (index.data(TaskModel::OverdueRole).toBool() && !(view.state & QStyle::State_Selected)){
        view.backgroundBrush = QBrush(QColor(255, 230, 230));
        view.palette.setColor(QPalette::Text, QColor(110, 25, 25));
    }
    const QWidget *widget = view.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &view, painter, widget);
}
