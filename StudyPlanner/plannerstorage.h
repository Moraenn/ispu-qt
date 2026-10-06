#ifndef PLANNERSTORAGE_H
#define PLANNERSTORAGE_H
#include <QString>
#include "core/studyplan.h"
class PlannerStorage{
public:
    static bool save(const QString &path, const planner::StudyPlan &plan, QString &error);
    static bool load(const QString &path, planner::StudyPlan &plan, QString &error);
    static bool exportCsv(const QString &path, const planner::StudyPlan &plan, QString &error);
};
#endif
