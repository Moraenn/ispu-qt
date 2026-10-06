#include "taskfilter.h"
#include "plannermodels.h"
#include <QDate>
TaskFilter::TaskFilter(QObject *parent) : QSortFilterProxyModel(parent){
    setDynamicSortFilter(true);
    setSortRole(Qt::EditRole);
    setSortCaseSensitivity(Qt::CaseInsensitive);
}
void TaskFilter::setSearch(const QString &s){ m_search = s.trimmed(); invalidateFilter(); }
void TaskFilter::setStatus(int value){ m_status = value; invalidateFilter(); }
void TaskFilter::setDateMode(int mode){ m_dateMode = mode; invalidateFilter(); }
void TaskFilter::refreshDate(){ invalidateFilter(); }
bool TaskFilter::filterAcceptsRow(int row, const QModelIndex &parent) const{
    if (!sourceModel()) return false;
    const auto value = [this, row, parent](int column){ return sourceModel()->index(row, column, parent).data(Qt::EditRole); };
    const QString searchable = value(TaskModel::Subject).toString() + " " + value(TaskModel::Title).toString();
    if (!m_search.isEmpty() && !searchable.contains(m_search, Qt::CaseInsensitive)) return false;
    const int status = value(TaskModel::Status).toInt();
    if (m_status >= 0 && status != m_status) return false;
    const QDate due = value(TaskModel::Deadline).toDate(), today = QDate::currentDate();
    if (m_dateMode == Overdue) return status != int(planner::Status::Done) && due < today;
    if (m_dateMode == Today) return due == today;
    if (m_dateMode == NextWeek) return due >= today && due <= today.addDays(6);
    return true;
}
