#include "plannercontroller.h"
#include "plannerstorage.h"
#include "qtutils.h"
#include <QDir>
#include <QFileInfo>
#include <utility>
PlannerController::PlannerController(QString path, QObject *parent) : QObject(parent), m_path(std::move(path)){}
bool PlannerController::initialize(QString &error){
    error.clear();
    if (!QDir().mkpath(QFileInfo(m_path).absolutePath())){ error = "Не удалось создать папку данных."; return false; }
    if (QFileInfo::exists(m_path)) m_ready = PlannerStorage::load(m_path, m_plan, error);
    else m_ready = PlannerStorage::save(m_path, m_plan, error);
    if (m_ready) emit changed();
    return m_ready;
}
bool PlannerController::apply(const std::function<bool(planner::StudyPlan &, std::string &)> &action, QString &error){
    error.clear();
    if (!m_ready){ error = "Хранилище не открыто."; return false; }
    planner::StudyPlan candidate = m_plan;
    std::string message;
    if (!action(candidate, message)){ error = text(message); return false; }
    // При неудачной записи модель и интерфейс остаются в прежнем состоянии.
    if (!PlannerStorage::save(m_path, candidate, error)) return false;
    m_plan = std::move(candidate);
    emit changed();
    return true;
}
bool PlannerController::addTask(const planner::Task &t, QString &e){
    return apply([&t](planner::StudyPlan &p, std::string &s){ return p.addTask(t, s); }, e);
}
bool PlannerController::updateTask(const planner::Task &t, QString &e){
    return apply([&t](planner::StudyPlan &p, std::string &s){ return p.updateTask(t, s); }, e);
}
bool PlannerController::removeTask(int id, QString &e){
    return apply([id](planner::StudyPlan &p, std::string &s){ return p.removeTask(id, s); }, e);
}
bool PlannerController::addSession(const planner::StudySession &s, QString &e){
    return apply([&s](planner::StudyPlan &p, std::string &m){ return p.addSession(s, m); }, e);
}
bool PlannerController::removeSession(int id, QString &e){
    return apply([id](planner::StudyPlan &p, std::string &s){ return p.removeSession(id, s); }, e);
}
bool PlannerController::importData(const QString &path, QString &error){
    if (!m_ready){ error = "Хранилище не открыто."; return false; }
    planner::StudyPlan candidate;
    if (!PlannerStorage::load(path, candidate, error)) return false;
    if (!PlannerStorage::save(m_path, candidate, error)) return false;
    m_plan = std::move(candidate);
    emit changed();
    return true;
}
bool PlannerController::backup(const QString &path, QString &e) const { return PlannerStorage::save(path, m_plan, e); }
bool PlannerController::exportCsv(const QString &path, QString &e) const { return PlannerStorage::exportCsv(path, m_plan, e); }
