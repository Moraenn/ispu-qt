#include "plannermodels.h"
#include "plannercontroller.h"
#include "qtutils.h"
#include <QStringList>
TaskModel::TaskModel(PlannerController *c, QObject *parent) : QAbstractTableModel(parent), m_controller(c){
    connect(c, &PlannerController::changed, this, &TaskModel::reload);
    reload();
}
void TaskModel::reload(){
    beginResetModel();
    m_tasks = m_controller->plan().tasks();
    m_spent = m_controller->plan().spentByTask();
    endResetModel();
}
int TaskModel::rowCount(const QModelIndex &p) const { return p.isValid() ? 0 : int(m_tasks.size()); }
int TaskModel::columnCount(const QModelIndex &p) const { return p.isValid() ? 0 : ColumnCount; }
planner::Task TaskModel::taskAt(int row) const { return row >= 0 && row < rowCount() ? m_tasks.at(row) : planner::Task(); }
QVariant TaskModel::data(const QModelIndex &i, int role) const{
    if (!i.isValid() || i.model() != this || i.row() < 0 || i.row() >= rowCount() || i.column() < 0 || i.column() >= ColumnCount) return {};
    const auto &t = m_tasks.at(i.row());
    const auto found = m_spent.find(t.id);
    const int actual = found == m_spent.end() ? 0 : found->second;
    if (role == IdRole) return t.id;
    if (role == OverdueRole) return planner::overdue(t, todayISO());
    if (role == Qt::ToolTipRole) return QString("%1\n%2\nСрок: %3\nПлан: %4; факт: %5")
        .arg(text(t.subject), text(t.title), text(t.deadline), minutesText(t.plannedMinutes), minutesText(actual));
    if (role != Qt::DisplayRole && role != Qt::EditRole) return {};
    switch (i.column()){
    case Subject: return text(t.subject);
    case Title: return text(t.title);
    case Deadline:
        if (role == Qt::EditRole) return QDate::fromString(text(t.deadline), Qt::ISODate);
        return QDate::fromString(text(t.deadline), Qt::ISODate).toString("dd.MM.yyyy");
    case Priority:
        if (role == Qt::EditRole) return int(t.priority);
        return priorityText(t.priority);
    case Status:
        if (role == Qt::EditRole) return int(t.status);
        return statusText(t.status);
    case Planned: return t.plannedMinutes;
    case Actual: return actual;
    default: return {};
    }
}
QVariant TaskModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if (role != Qt::DisplayRole) return {};
    if (orientation == Qt::Vertical) return section >= 0 && section < rowCount() ? QVariant(section + 1) : QVariant();
    const QStringList headers = {"Предмет", "Задание", "Срок", "Приоритет", "Статус", "План, мин", "Факт, мин"};
    return section >= 0 && section < headers.size() ? QVariant(headers.at(section)) : QVariant();
}
SessionModel::SessionModel(PlannerController *c, QObject *parent) : QAbstractTableModel(parent), m_controller(c){
    connect(c, &PlannerController::changed, this, &SessionModel::reload);
    reload();
}
void SessionModel::reload(){
    beginResetModel();
    m_sessions = m_controller->plan().sessions();
    m_tasks.clear();
    for (const auto &t : m_controller->plan().tasks()) m_tasks[t.id] = t;
    endResetModel();
}
int SessionModel::rowCount(const QModelIndex &p) const { return p.isValid() ? 0 : int(m_sessions.size()); }
int SessionModel::columnCount(const QModelIndex &p) const { return p.isValid() ? 0 : 5; }
int SessionModel::sessionId(int row) const { return row >= 0 && row < rowCount() ? m_sessions.at(row).id : 0; }
QVariant SessionModel::data(const QModelIndex &i, int role) const{
    if (!i.isValid() || i.model() != this || i.row() < 0 || i.row() >= rowCount() || i.column() < 0 || i.column() >= 5) return {};
    if (role != Qt::DisplayRole && role != Qt::EditRole && role != Qt::ToolTipRole) return {};
    const auto &s = m_sessions.at(i.row());
    const auto found = m_tasks.find(s.taskId);
    if (found == m_tasks.end()) return {};
    const auto &t = found->second;
    switch (i.column()){
    case 0:
        if (role == Qt::EditRole) return QDate::fromString(text(s.date), Qt::ISODate);
        return QDate::fromString(text(s.date), Qt::ISODate).toString("dd.MM.yyyy");
    case 1: return text(t.subject);
    case 2: return text(t.title);
    case 3: return s.minutes;
    case 4: return text(s.note);
    default: return {};
    }
}
QVariant SessionModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if (role != Qt::DisplayRole) return {};
    if (orientation == Qt::Vertical) return section >= 0 && section < rowCount() ? QVariant(section + 1) : QVariant();
    const QStringList headers = {"Дата", "Предмет", "Задание", "Минуты", "Примечание"};
    return section >= 0 && section < headers.size() ? QVariant(headers.at(section)) : QVariant();
}
