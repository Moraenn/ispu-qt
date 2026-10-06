#include "stampdelegate.h"
#include "stampmodel.h"
#include <QLineEdit>
#include <QSpinBox>
StampDelegate::StampDelegate(QObject *parent) : QStyledItemDelegate(parent){}
QWidget *StampDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const{
    if (index.column() == StampModel::TitleColumn || index.column() == StampModel::CountryColumn){
        auto *edit = new QLineEdit(parent);
        edit->setMaxLength(200);
        return edit;
    }
    if (index.column() == StampModel::YearColumn || index.column() == StampModel::CirculationColumn){
        auto *edit = new QSpinBox(parent);
        if (index.column() == StampModel::YearColumn) edit->setRange(1840, 2100);
        else edit->setRange(1, 1000000000);
        edit->setGroupSeparatorShown(true);
        return edit;
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}
void StampDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const{
    const QVariant value = index.data(Qt::EditRole);
    if (auto *edit = qobject_cast<QLineEdit *>(editor)) edit->setText(value.toString());
    else if (auto *spin = qobject_cast<QSpinBox *>(editor)) spin->setValue(value.toInt());
    else QStyledItemDelegate::setEditorData(editor, index);
}
void StampDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const{
    if (auto *edit = qobject_cast<QLineEdit *>(editor)) model->setData(index, edit->text().trimmed());
    else if (auto *spin = qobject_cast<QSpinBox *>(editor)) model->setData(index, spin->value());
    else QStyledItemDelegate::setModelData(editor, model, index);
}
void StampDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const{
    editor->setGeometry(option.rect);
}
