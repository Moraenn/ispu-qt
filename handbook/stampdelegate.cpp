#include "stampdelegate.h"

#include <QLineEdit>
#include <QSpinBox>

#include "stampmodel.h"

StampDelegate::StampDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QWidget *StampDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &,
                                     const QModelIndex &index) const
{
    switch (index.column()) {
    case StampModel::TitleColumn:
    case StampModel::CountryColumn:
        return new QLineEdit(parent);
    case StampModel::YearColumn: {
        auto *editor = new QSpinBox(parent);
        editor->setRange(1840, 2100);
        return editor;
    }
    case StampModel::CirculationColumn: {
        auto *editor = new QSpinBox(parent);
        editor->setRange(1, 1000000000);
        editor->setGroupSeparatorShown(true);
        return editor;
    }
    default:
        return QStyledItemDelegate::createEditor(parent, {}, index);
    }
}

void StampDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    const QVariant value = index.model()->data(index, Qt::EditRole);

    if (auto *lineEdit = qobject_cast<QLineEdit *>(editor)) {
        lineEdit->setText(value.toString());
        return;
    }

    if (auto *spinBox = qobject_cast<QSpinBox *>(editor)) {
        spinBox->setValue(value.toInt());
        return;
    }

    QStyledItemDelegate::setEditorData(editor, index);
}

void StampDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (auto *lineEdit = qobject_cast<QLineEdit *>(editor)) {
        model->setData(index, lineEdit->text(), Qt::EditRole);
        return;
    }

    if (auto *spinBox = qobject_cast<QSpinBox *>(editor)) {
        model->setData(index, spinBox->value(), Qt::EditRole);
        return;
    }

    QStyledItemDelegate::setModelData(editor, model, index);
}

void StampDelegate::updateEditorGeometry(QWidget *editor,
                                         const QStyleOptionViewItem &option,
                                         const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}
