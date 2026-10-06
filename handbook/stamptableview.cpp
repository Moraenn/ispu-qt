#include "stamptableview.h"
#include <QHeaderView>
#include <QItemSelectionModel>
StampTableView::StampTableView(QWidget *parent) : QTableView(parent){
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setAlternatingRowColors(true);
    setEditTriggers(QAbstractItemView::EditKeyPressed);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}
int StampTableView::currentRow() const{
    if (!selectionModel()) return -1;
    const auto rows = selectionModel()->selectedRows();
    return rows.isEmpty() ? -1 : rows.first().row();
}
