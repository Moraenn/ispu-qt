#ifndef PLANNERMODELS_H
#define PLANNERMODELS_H
#include <QAbstractTableModel>
#include <QVector>
#include <map>
#include "core/studyplan.h"
class PlannerController;
class TaskModel : public QAbstractTableModel{
    Q_OBJECT
public:
    enum Column { Subject, Title, Deadline, Priority, Status, Planned, Actual, ColumnCount };
    enum Role { IdRole = Qt::UserRole + 1, OverdueRole };
    explicit TaskModel(PlannerController *controller, QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    planner::Task taskAt(int row) const;
public slots:
    void reload();
private:
    PlannerController *m_controller;
    std::vector<planner::Task> m_tasks;
    std::map<int, int> m_spent;
};
class SessionModel : public QAbstractTableModel{
    Q_OBJECT
public:
    explicit SessionModel(PlannerController *controller, QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int sessionId(int row) const;
private slots:
    void reload();
private:
    PlannerController *m_controller;
    std::vector<planner::StudySession> m_sessions;
    std::map<int, planner::Task> m_tasks;
};
#endif
