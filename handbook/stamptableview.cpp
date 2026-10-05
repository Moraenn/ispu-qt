#include "stamptableview.h"

#include <QHeaderView>
#include <QModelIndex>
#include <QItemSelectionModel>

StampTableView::StampTableView(QWidget *parent)
    : QTableView(parent)
{
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setAlternatingRowColors(true);
    setSortingEnabled(false);
    setShowGrid(true);
    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    verticalHeader()->setVisible(false);
}

int StampTableView::currentRow() const
{
    if (selectionModel()) {
        const QModelIndexList selectedRows = selectionModel()->selectedRows();
        if (!selectedRows.isEmpty()) {
            return selectedRows.constFirst().row();
        }
    }

    return currentIndex().isValid() ? currentIndex().row() : -1;
}

void StampTableView::editCurrentStamp()
{
    if (!model()) {
        return;
    }

    const int row = currentRow();
    if (row >= 0) {
        edit(model()->index(row, 0));
    }
}

