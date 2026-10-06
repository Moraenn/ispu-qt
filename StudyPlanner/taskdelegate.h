#ifndef TASKDELEGATE_H
#define TASKDELEGATE_H
#include <QStyledItemDelegate>
class TaskDelegate : public QStyledItemDelegate{
    Q_OBJECT
public:
    explicit TaskDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent){}
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
#endif
