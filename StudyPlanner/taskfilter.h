#ifndef TASKFILTER_H
#define TASKFILTER_H
#include <QSortFilterProxyModel>
class TaskFilter : public QSortFilterProxyModel{
    Q_OBJECT
public:
    enum DateMode { AllDates, Overdue, Today, NextWeek };
    explicit TaskFilter(QObject *parent = nullptr);
    void setSearch(const QString &search);
    void setStatus(int status);
    void setDateMode(int mode);
    void refreshDate();
protected:
    bool filterAcceptsRow(int row, const QModelIndex &parent) const override;
private:
    QString m_search;
    int m_status = -1;
    int m_dateMode = AllDates;
};
#endif
