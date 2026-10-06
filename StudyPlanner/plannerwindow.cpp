#include "plannerwindow.h"
#include "plannercontroller.h"
#include "plannermodels.h"
#include "plannerdialogs.h"
#include "taskfilter.h"
#include "taskdelegate.h"
#include "summarywidget.h"
#include "qtutils.h"
#include <QAction>
#include <QComboBox>
#include <QFileDialog>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QItemSelectionModel>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QStatusBar>
#include <QTableView>
#include <QTabWidget>
#include <QTimer>
#include <QVBoxLayout>
namespace{
void configureTable(QTableView *view){
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->setSortingEnabled(true);
    view->setAlternatingRowColors(true);
    view->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}
}
PlannerWindow::PlannerWindow(PlannerController *controller, QWidget *parent) : QMainWindow(parent), m_controller(controller){
    setWindowTitle("Планировщик учебных заданий");
    resize(1120, 650);
    m_tasks = new TaskModel(controller, this);
    m_filter = new TaskFilter(this);
    m_filter->setSourceModel(m_tasks);
    m_sessions = new SessionModel(controller, this);
    m_sessionSort = new QSortFilterProxyModel(this);
    m_sessionSort->setSourceModel(m_sessions);
    m_sessionSort->setSortRole(Qt::EditRole);
    m_tabs = new QTabWidget(this);
    m_tabs->setObjectName("pages");
    setCentralWidget(m_tabs);
    auto *taskPage = new QWidget(this);
    auto *taskLayout = new QVBoxLayout(taskPage);
    auto *search = new QLineEdit(this);
    search->setObjectName("search");
    search->setPlaceholderText("Поиск по предмету или заданию");
    auto *status = new QComboBox(this);
    status->addItem("Все статусы", -1);
    for (int i = 0; i < 3; ++i) status->addItem(statusText(planner::Status(i)), i);
    auto *dates = new QComboBox(this);
    dates->addItems({"Все сроки", "Просроченные", "Сегодня", "Ближайшие 7 дней"});
    auto *filters = new QHBoxLayout;
    filters->addWidget(search, 1);
    filters->addWidget(status);
    filters->addWidget(dates);
    taskLayout->addLayout(filters);
    m_taskView = new QTableView(this);
    m_taskView->setObjectName("taskTable");
    m_taskView->setModel(m_filter);
    m_taskView->setItemDelegate(new TaskDelegate(this));
    configureTable(m_taskView);
    m_taskView->horizontalHeader()->setSectionResizeMode(TaskModel::Title, QHeaderView::Stretch);
    m_taskView->sortByColumn(TaskModel::Deadline, Qt::AscendingOrder);
    taskLayout->addWidget(m_taskView);
    auto *add = new QPushButton("Добавить", this);
    add->setShortcut(QKeySequence::New);
    m_edit = new QPushButton("Изменить", this);
    m_remove = new QPushButton("Удалить", this);
    m_complete = new QPushButton("Отметить выполненным", this);
    auto *buttons = new QHBoxLayout;
    for (auto *button : {add, m_edit, m_remove, m_complete}) buttons->addWidget(button);
    buttons->addStretch();
    taskLayout->addLayout(buttons);
    taskLayout->addWidget(new QLabel("Красным выделены просроченные незавершённые задания. Время указывается в минутах.", this));
    auto *sessionPage = new QWidget(this);
    auto *sessionLayout = new QVBoxLayout(sessionPage);
    m_sessionView = new QTableView(this);
    m_sessionView->setModel(m_sessionSort);
    configureTable(m_sessionView);
    m_sessionView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_sessionView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    m_sessionView->sortByColumn(0, Qt::DescendingOrder);
    sessionLayout->addWidget(m_sessionView);
    m_addSession = new QPushButton("Записать занятие", this);
    m_removeSession = new QPushButton("Удалить запись", this);
    auto *sessionButtons = new QHBoxLayout;
    sessionButtons->addWidget(m_addSession);
    sessionButtons->addWidget(m_removeSession);
    sessionButtons->addStretch();
    sessionLayout->addLayout(sessionButtons);
    m_summary = new SummaryWidget(controller, this);
    m_tabs->addTab(taskPage, "Задания");
    m_tabs->addTab(sessionPage, "Журнал занятий");
    m_tabs->addTab(m_summary, "Сводка");
    auto *fileMenu = menuBar()->addMenu("Файл");
    auto *backupAction = fileMenu->addAction("Резервная копия...");
    auto *restoreAction = fileMenu->addAction("Восстановить копию...");
    auto *csvAction = fileMenu->addAction("Экспорт заданий в CSV...");
    fileMenu->addSeparator();
    auto *quitAction = fileMenu->addAction("Выход");
    connect(backupAction, &QAction::triggered, this, &PlannerWindow::backup);
    connect(restoreAction, &QAction::triggered, this, &PlannerWindow::restore);
    connect(csvAction, &QAction::triggered, this, &PlannerWindow::exportCsv);
    connect(quitAction, &QAction::triggered, this, &QWidget::close);
    connect(search, &QLineEdit::textChanged, m_filter, &TaskFilter::setSearch);
    connect(status, qOverload<int>(&QComboBox::currentIndexChanged), this, [this, status](){ m_filter->setStatus(status->currentData().toInt()); });
    connect(dates, qOverload<int>(&QComboBox::currentIndexChanged), m_filter, &TaskFilter::setDateMode);
    connect(add, &QPushButton::clicked, this, &PlannerWindow::addTask);
    connect(m_edit, &QPushButton::clicked, this, &PlannerWindow::editTask);
    connect(m_remove, &QPushButton::clicked, this, &PlannerWindow::removeTask);
    connect(m_complete, &QPushButton::clicked, this, &PlannerWindow::completeTask);
    connect(m_addSession, &QPushButton::clicked, this, &PlannerWindow::addSession);
    connect(m_removeSession, &QPushButton::clicked, this, &PlannerWindow::removeSession);
    connect(m_taskView, &QTableView::doubleClicked, this, [this](){ editTask(); });
    connect(m_taskView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &PlannerWindow::updateActions);
    connect(m_sessionView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &PlannerWindow::updateActions);
    connect(controller, &PlannerController::changed, this, [this](){
        updateActions();
        statusBar()->showMessage("Изменения сохранены: " + m_controller->path());
    });
    auto *timer = new QTimer(this);
    timer->setInterval(60000);
    connect(timer, &QTimer::timeout, this, [this](){
        m_filter->refreshDate();
        m_taskView->viewport()->update();
        m_summary->refresh();
    });
    timer->start();
    statusBar()->showMessage("Данные: " + controller->path());
    updateActions();
}
void PlannerWindow::selectTab(int index){ if (index >= 0 && index < m_tabs->count()) m_tabs->setCurrentIndex(index); }
int PlannerWindow::selectedTaskId() const{
    const auto rows = m_taskView->selectionModel()->selectedRows();
    if (rows.isEmpty()) return 0;
    const QModelIndex source = m_filter->mapToSource(rows.first());
    return m_tasks->taskAt(source.row()).id;
}
QStringList PlannerWindow::subjects() const{
    QStringList result;
    for (const auto &t : m_controller->plan().tasks()) result.append(text(t.subject));
    result.removeDuplicates();
    result.sort(Qt::CaseInsensitive);
    return result;
}
void PlannerWindow::selectTask(int id){
    for (int row = 0; row < m_tasks->rowCount(); ++row){
        if (m_tasks->taskAt(row).id != id) continue;
        const auto visible = m_filter->mapFromSource(m_tasks->index(row, 0));
        if (visible.isValid()) m_taskView->selectRow(visible.row());
        break;
    }
    updateActions();
}
void PlannerWindow::showError(const QString &error){ QMessageBox::warning(this, "Ошибка", error); }
void PlannerWindow::addTask(){
    TaskDialog dialog(subjects(), this);
    dialog.setWindowTitle("Новое задание");
    if (dialog.exec() != QDialog::Accepted) return;
    QString error;
    if (!m_controller->addTask(dialog.task(), error)){ showError(error); return; }
    selectTask(m_controller->plan().tasks().back().id);
}
void PlannerWindow::editTask(){
    const int id = selectedTaskId();
    const auto *task = m_controller->plan().task(id);
    if (!task) return;
    TaskDialog dialog(subjects(), this);
    dialog.setWindowTitle("Изменение задания");
    dialog.setTask(*task);
    if (dialog.exec() != QDialog::Accepted) return;
    QString error;
    if (!m_controller->updateTask(dialog.task(), error)){ showError(error); return; }
    selectTask(id);
}
void PlannerWindow::removeTask(){
    const int id = selectedTaskId();
    if (!id) return;
    if (QMessageBox::question(this, "Удаление задания", "Удалить задание и все связанные записи занятий?") != QMessageBox::Yes) return;
    QString error;
    if (!m_controller->removeTask(id, error)) showError(error);
}
void PlannerWindow::completeTask(){
    const auto *selected = m_controller->plan().task(selectedTaskId());
    if (!selected) return;
    auto task = *selected;
    task.status = planner::Status::Done;
    QString error;
    if (!m_controller->updateTask(task, error)){ showError(error); return; }
    selectTask(task.id);
}
void PlannerWindow::addSession(){
    SessionDialog dialog(m_controller->plan().tasks(), selectedTaskId(), this);
    if (dialog.exec() != QDialog::Accepted) return;
    QString error;
    if (!m_controller->addSession(dialog.session(), error)) showError(error);
}
void PlannerWindow::removeSession(){
    const auto rows = m_sessionView->selectionModel()->selectedRows();
    if (rows.isEmpty()) return;
    const int id = m_sessions->sessionId(m_sessionSort->mapToSource(rows.first()).row());
    if (QMessageBox::question(this, "Удаление занятия", "Удалить запись о затраченном времени?") != QMessageBox::Yes) return;
    QString error;
    if (!m_controller->removeSession(id, error)) showError(error);
}
void PlannerWindow::backup(){
    const QString path = QFileDialog::getSaveFileName(this, "Резервная копия", "study-backup.json", "JSON (*.json)");
    if (path.isEmpty()) return;
    QString error;
    if (!m_controller->backup(path, error)) showError(error);
    else statusBar()->showMessage("Резервная копия: " + path);
}
void PlannerWindow::restore(){
    const QString path = QFileDialog::getOpenFileName(this, "Восстановить копию", {}, "JSON (*.json)");
    if (path.isEmpty()) return;
    if (QMessageBox::question(this, "Восстановление", "Заменить текущие задания и занятия данными из файла?") != QMessageBox::Yes) return;
    QString error;
    if (!m_controller->importData(path, error)) showError(error);
}
void PlannerWindow::exportCsv(){
    const QString path = QFileDialog::getSaveFileName(this, "Экспорт всех заданий", "tasks.csv", "CSV (*.csv)");
    if (path.isEmpty()) return;
    QString error;
    if (!m_controller->exportCsv(path, error)) showError(error);
    else statusBar()->showMessage("Экспортировано: " + path);
}
void PlannerWindow::updateActions(){
    const auto *task = m_controller->plan().task(selectedTaskId());
    m_edit->setEnabled(task != nullptr);
    m_remove->setEnabled(task != nullptr);
    m_complete->setEnabled(task && task->status != planner::Status::Done);
    m_addSession->setEnabled(!m_controller->plan().tasks().empty());
    m_removeSession->setEnabled(!m_sessionView->selectionModel()->selectedRows().isEmpty());
}
