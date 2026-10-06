#include "mainwindow.h"
#include "stampstorage.h"
#include <QCloseEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QKeySequence>
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
    auto *files = new QHBoxLayout;
    auto *newButton = new QPushButton("Новый", this);
    auto *openButton = new QPushButton("Открыть...", this);
    auto *saveButton = new QPushButton("Сохранить", this);
    auto *saveAsButton = new QPushButton("Сохранить как...", this);
    newButton->setShortcut(QKeySequence::New);
    openButton->setShortcut(QKeySequence::Open);
    saveButton->setShortcut(QKeySequence::Save);
    saveAsButton->setShortcut(QKeySequence::SaveAs);
    files->addWidget(newButton);
    files->addWidget(openButton);
    files->addWidget(saveButton);
    files->addWidget(saveAsButton);
    files->addStretch();
    layout->addLayout(files);
    layout->addLayout(content);
    connect(newButton, &QPushButton::clicked, this, &MainWindow::newFile);
    connect(openButton, &QPushButton::clicked, this, &MainWindow::openFile);
    connect(saveButton, &QPushButton::clicked, this, [this](){ saveFile(); });
    connect(saveAsButton, &QPushButton::clicked, this, [this](){ saveAs(); });
    connect(m_model, &QAbstractItemModel::dataChanged, this, [this](){ setDirty(true); });
    connect(m_model, &QAbstractItemModel::rowsInserted, this, [this](){ setDirty(true); });
    connect(m_model, &QAbstractItemModel::rowsRemoved, this, [this](){ setDirty(true); });
    setDirty(false);
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

void MainWindow::setDirty(bool dirty){
    m_dirty = dirty;
    const QString name = m_fileName.isEmpty() ? "Новая коллекция" : QFileInfo(m_fileName).fileName();
    setWindowTitle(name + "[*] — Справочник филателиста");
    setWindowModified(dirty);
}
bool MainWindow::maybeSave(){
    if (!m_dirty) return true;
    const auto result = QMessageBox::warning(this, "Несохранённые изменения", "Сохранить изменения коллекции?",
                                            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (result == QMessageBox::Cancel) return false;
    return result == QMessageBox::Discard || saveFile();
}
void MainWindow::newFile(){
    if (!maybeSave()) return;
    m_model->replaceStamps({});
    m_fileName.clear();
    setDirty(false);
    selectRow(-1);
}
void MainWindow::openFile(){
    const QString path = QFileDialog::getOpenFileName(this, "Открыть коллекцию", {}, "Коллекции (*.stamps);;Все файлы (*)");
    if (path.isEmpty() || !maybeSave()) return;
    QVector<Stamp> stamps;
    QString error;
    if (!StampStorage::load(path, stamps, error)){
        QMessageBox::warning(this, "Ошибка чтения", error);
        return;
    }
    if (!m_model->replaceStamps(stamps)) return;
    m_fileName = path;
    setDirty(false);
    selectRow(m_model->rowCount() ? 0 : -1);
}
bool MainWindow::saveTo(const QString &path){
    QString error;
    if (!StampStorage::save(path, m_model->stamps(), error)){
        QMessageBox::warning(this, "Ошибка сохранения", error);
        return false;
    }
    m_fileName = path;
    setDirty(false);
    return true;
}
bool MainWindow::saveFile(){ return m_fileName.isEmpty() ? saveAs() : saveTo(m_fileName); }
bool MainWindow::saveAs(){
    QString path = QFileDialog::getSaveFileName(this, "Сохранить коллекцию", m_fileName.isEmpty() ? "collection.stamps" : m_fileName,
                                              "Коллекции (*.stamps);;Все файлы (*)");
    if (path.isEmpty()) return false;
    if (QFileInfo(path).suffix().isEmpty()){
        path += ".stamps";
        if (QFileInfo::exists(path) && QMessageBox::question(this, "Замена файла", "Файл уже существует. Заменить его?") != QMessageBox::Yes) return false;
    }
    return saveTo(path);
}
void MainWindow::closeEvent(QCloseEvent *event){
    if (maybeSave()) event->accept();
    else event->ignore();
}
