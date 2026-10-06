#ifndef PLANNERCONTROLLER_H
#define PLANNERCONTROLLER_H
#include <QObject>
#include <QString>
#include <functional>
#include "core/studyplan.h"
class PlannerController : public QObject{
    Q_OBJECT
public:
    explicit PlannerController(QString path, QObject *parent = nullptr);
    bool initialize(QString &error);
    const planner::StudyPlan &plan() const { return m_plan; }
    const QString &path() const { return m_path; }
    bool addTask(const planner::Task &task, QString &error);
    bool updateTask(const planner::Task &task, QString &error);
    bool removeTask(int id, QString &error);
    bool addSession(const planner::StudySession &session, QString &error);
    bool removeSession(int id, QString &error);
    bool importData(const QString &path, QString &error);
    bool backup(const QString &path, QString &error) const;
    bool exportCsv(const QString &path, QString &error) const;
signals:
    void changed();
private:
    bool apply(const std::function<bool(planner::StudyPlan &, std::string &)> &action, QString &error);
    planner::StudyPlan m_plan;
    QString m_path;
    bool m_ready = false;
};
#endif
