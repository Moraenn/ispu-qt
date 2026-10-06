#include "mainwindow.h"
#include "stampmodel.h"
#include "stamptableview.h"
#include "stampdelegate.h"
#include "stampdialog.h"
#include <QHBoxLayout>
#include <QItemSelectionModel>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
MainWindow::MainWindow(QWidget *parent) : QWidget(parent){
    setWindowTitle("Справочник филателиста");
    resize(860, 420);
    m_model = new StampModel(this);
    m_view = new StampTableView(this);
    m_view->setModel(m_model);
    m_view->setItemDelegate(new StampDelegate(this));
    auto *add = new QPushButton("Добавить", this);
    m_editButton = new QPushButton("Редактировать", this);
    m_removeButton = new QPushButton("Удалить", this);
    auto *close = new QPushButton("Закрыть", this);
    auto *buttons = new QVBoxLayout;
    buttons->addWidget(add);
    buttons->addWidget(m_editButton);
    buttons->addWidget(m_removeButton);
    buttons->addStretch();
    buttons->addWidget(close);
    auto *content = new QHBoxLayout;
    content->addWidget(m_view, 1);
    content->addLayout(buttons);
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Коллекция почтовых марок", this));
    layout->addWidget(new QLabel("Двойной щелчок — диалог. F2 — редактирование ячейки.", this));
    layout->addLayout(content);
    connect(add, &QPushButton::clicked, this, &MainWindow::addStamp);
    connect(m_editButton, &QPushButton::clicked, this, &MainWindow::editStamp);
    connect(m_removeButton, &QPushButton::clicked, this, &MainWindow::removeStamp);
    connect(close, &QPushButton::clicked, this, &QWidget::close);
    connect(m_view, &QTableView::doubleClicked, this, [this](){ editStamp(); });
    connect(m_view->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::updateActions);
    selectRow(0);
}
void MainWindow::addStamp(){
    StampDialog dialog(this);
    dialog.setWindowTitle("Добавление марки");
    if (dialog.exec() != QDialog::Accepted) return;
    if (!m_model->addStamp(dialog.stamp())){
        QMessageBox::warning(this, "Ошибка", "Не удалось добавить запись. Максимум — 10000 марок.");
        return;
    }
    selectRow(m_model->rowCount() - 1);
}
void MainWindow::editStamp(){
    const int row = m_view->currentRow();
    if (row < 0) return;
    StampDialog dialog(this);
    dialog.setWindowTitle("Редактирование марки");
    dialog.setStamp(m_model->stampAt(row));
    if (dialog.exec() != QDialog::Accepted) return;
    m_model->updateStamp(row, dialog.stamp());
    selectRow(row);
}
void MainWindow::removeStamp(){
    const int row = m_view->currentRow();
    if (row < 0) return;
    if (QMessageBox::question(this, "Удаление", "Удалить выбранную марку?") != QMessageBox::Yes) return;
    if (m_model->removeStampAt(row)) selectRow(qMin(row, m_model->rowCount() - 1));
}
void MainWindow::updateActions(){
    const bool selected = m_view->currentRow() >= 0;
    m_editButton->setEnabled(selected);
    m_removeButton->setEnabled(selected);
}
void MainWindow::selectRow(int row){
    if (row >= 0 && row < m_model->rowCount()) m_view->selectRow(row);
    else{
        m_view->clearSelection();
        m_view->setCurrentIndex({});
    }
    updateActions();
}
