#include "mainwindow.h"

#include <QAbstractItemView>
#include <QHBoxLayout>
#include <QItemSelectionModel>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

#include "stampdelegate.h"
#include "stampdialog.h"
#include "stampmodel.h"
#include "stamptableview.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , m_model(new StampModel(this))
    , m_view(new StampTableView(this))
    , m_delegate(new StampDelegate(this))
    , m_addButton(new QPushButton(QStringLiteral("Добавить"), this))
    , m_editButton(new QPushButton(QStringLiteral("Редактировать"), this))
    , m_removeButton(new QPushButton(QStringLiteral("Удалить"), this))
    , m_closeButton(new QPushButton(QStringLiteral("Закрыть"), this))
{
    setWindowTitle(QStringLiteral("Справочник филателиста"));
    resize(860, 420);

    auto *titleLabel = new QLabel(QStringLiteral("Коллекция почтовых марок"), this);
    titleLabel->setStyleSheet(QStringLiteral("font-size: 18px; font-weight: 600;"));

    auto *hintLabel = new QLabel(QStringLiteral("Двойной щелчок по строке или кнопка \"Редактировать\" открывает диалог изменения записи."), this);
    hintLabel->setWordWrap(true);

    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->selectRow(0);

    auto *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_editButton);
    buttonLayout->addWidget(m_removeButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_closeButton);

    auto *contentLayout = new QHBoxLayout;
    contentLayout->addWidget(m_view, 1);
    contentLayout->addLayout(buttonLayout);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(hintLabel);
    mainLayout->addLayout(contentLayout);

    connect(m_addButton, &QPushButton::clicked, this, &MainWindow::addStamp);
    connect(m_editButton, &QPushButton::clicked, this, &MainWindow::editStamp);
    connect(m_removeButton, &QPushButton::clicked, this, &MainWindow::removeStamp);
    connect(m_closeButton, &QPushButton::clicked, this, &QWidget::close);
    connect(m_view, &QTableView::doubleClicked, this, [this]() {
        editStamp();
    });
    connect(m_view->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::updateActions);

    updateActions();
}

void MainWindow::addStamp()
{
    StampDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("Добавление марки"));

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    m_model->addStamp(dialog.stamp());
    selectRow(m_model->rowCount() - 1);
}

void MainWindow::editStamp()
{
    const int row = m_view->currentRow();
    if (row < 0) {
        return;
    }

    StampDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("Редактирование марки"));
    dialog.setStamp(m_model->stampAt(row));

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    m_model->updateStamp(row, dialog.stamp());
    selectRow(row);
}

void MainWindow::removeStamp()
{
    const int row = m_view->currentRow();
    if (row < 0) {
        return;
    }

    const auto answer = QMessageBox::question(
        this,
        QStringLiteral("Удаление записи"),
        QStringLiteral("Удалить выбранную марку из справочника?")
    );

    if (answer != QMessageBox::Yes) {
        return;
    }

    if (!m_model->removeStampAt(row)) {
        return;
    }

    QTimer::singleShot(0, this, [this, row]() {
        if (m_model->rowCount() > 0) {
            selectRow(qMin(row, m_model->rowCount() - 1));
        }

        updateActions();
    });
}

void MainWindow::updateActions()
{
    const bool hasSelection = m_view->currentRow() >= 0;
    m_editButton->setEnabled(hasSelection);
    m_removeButton->setEnabled(hasSelection);
}

void MainWindow::selectRow(int row)
{
    if (row < 0 || row >= m_model->rowCount()) {
        return;
    }

    m_view->selectRow(row);
    m_view->setFocus();
}
