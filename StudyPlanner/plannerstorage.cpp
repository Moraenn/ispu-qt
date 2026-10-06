#include "plannerstorage.h"
#include "qtutils.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <QStringList>
#include <cmath>
namespace{
bool integer(const QJsonObject &o, const QString &key, int low, int high, int &result){
    const QJsonValue value = o.value(key);
    if (!value.isDouble()) return false;
    const double n = value.toDouble();
    if (!std::isfinite(n) || std::floor(n) != n || n < low || n > high) return false;
    result = int(n);
    return true;
}
bool stringValue(const QJsonObject &o, const QString &key, std::string &result){
    if (!o.value(key).isString()) return false;
    result = o.value(key).toString().toStdString();
    return true;
}
bool writeBytes(const QString &path, const QByteArray &bytes, QString &error){
    error.clear();
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)){ error = file.errorString(); return false; }
    if (file.write(bytes) != bytes.size()){
        error = file.errorString();
        file.cancelWriting();
        return false;
    }
    if (!file.commit()){ error = file.errorString(); return false; }
    return true;
}
QString csvCell(QString value){
    // Защита текстовых ячеек от интерпретации как формул в табличном редакторе.
    if (!value.isEmpty() && QString("=+-@\t\r\n").contains(value.front())) value.prepend('\'');
    value.replace('"', "\"\"");
    return '"' + value + '"';
}
}
bool PlannerStorage::save(const QString &path, const planner::StudyPlan &plan, QString &error){
    QJsonArray tasks, sessions;
    for (const auto &t : plan.tasks()){
        tasks.append(QJsonObject{{"id", t.id}, {"subject", text(t.subject)}, {"title", text(t.title)},
                                 {"deadline", text(t.deadline)}, {"priority", int(t.priority)},
                                 {"status", int(t.status)}, {"plannedMinutes", t.plannedMinutes}});
    }
    for (const auto &s : plan.sessions()){
        sessions.append(QJsonObject{{"id", s.id}, {"taskId", s.taskId}, {"date", text(s.date)},
                                    {"minutes", s.minutes}, {"note", text(s.note)}});
    }
    const QJsonObject root{{"schemaVersion", 1}, {"tasks", tasks}, {"sessions", sessions}};
    return writeBytes(path, QJsonDocument(root).toJson(QJsonDocument::Indented), error);
}
bool PlannerStorage::load(const QString &path, planner::StudyPlan &plan, QString &error){
    error.clear();
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)){ error = file.errorString(); return false; }
    if (file.size() <= 0 || file.size() > 64 * 1024 * 1024){ error = "Недопустимый размер файла."; return false; }
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()){
        error = "Некорректный JSON: " + parseError.errorString();
        return false;
    }
    const QJsonObject root = doc.object();
    int version = 0;
    if (!integer(root, "schemaVersion", 1, 1, version) || !root.value("tasks").isArray() || !root.value("sessions").isArray()){
        error = "Неподдерживаемая версия или структура файла.";
        return false;
    }
    const QJsonArray taskArray = root.value("tasks").toArray(), sessionArray = root.value("sessions").toArray();
    if (taskArray.size() > planner::StudyPlan::MaxTasks || sessionArray.size() > planner::StudyPlan::MaxSessions){
        error = "Превышен лимит записей.";
        return false;
    }
    std::vector<planner::Task> tasks;
    std::vector<planner::StudySession> sessions;
    for (const QJsonValue &value : taskArray){
        if (!value.isObject()){ error = "Задание должно быть объектом JSON."; return false; }
        const QJsonObject o = value.toObject();
        planner::Task t;
        int priority = 0, status = 0;
        if (!integer(o, "id", 1, planner::StudyPlan::MaxId, t.id) || !stringValue(o, "subject", t.subject) ||
            !stringValue(o, "title", t.title) || !stringValue(o, "deadline", t.deadline) ||
            !integer(o, "priority", 0, 2, priority) || !integer(o, "status", 0, 2, status) ||
            !integer(o, "plannedMinutes", 1, 6000, t.plannedMinutes)){
            error = "Некорректные поля задания.";
            return false;
        }
        t.priority = planner::Priority(priority);
        t.status = planner::Status(status);
        tasks.push_back(t);
    }
    for (const QJsonValue &value : sessionArray){
        if (!value.isObject()){ error = "Занятие должно быть объектом JSON."; return false; }
        const QJsonObject o = value.toObject();
        planner::StudySession s;
        if (!integer(o, "id", 1, planner::StudyPlan::MaxId, s.id) ||
            !integer(o, "taskId", 1, planner::StudyPlan::MaxId, s.taskId) ||
            !stringValue(o, "date", s.date) || !integer(o, "minutes", 1, 720, s.minutes) || !stringValue(o, "note", s.note)){
            error = "Некорректные поля занятия.";
            return false;
        }
        sessions.push_back(s);
    }
    std::string validationError;
    if (!plan.replaceData(tasks, sessions, validationError)){ error = text(validationError); return false; }
    return true;
}
bool PlannerStorage::exportCsv(const QString &path, const planner::StudyPlan &plan, QString &error){
    QString csv = "ID;Предмет;Задание;Срок;Приоритет;Статус;План, мин;Факт, мин\r\n";
    const auto spent = plan.spentByTask();
    for (const auto &t : plan.tasks()){
        const auto found = spent.find(t.id);
        const int actual = found == spent.end() ? 0 : found->second;
        QStringList cells = {QString::number(t.id), csvCell(text(t.subject)), csvCell(text(t.title)), text(t.deadline),
                             csvCell(priorityText(t.priority)), csvCell(statusText(t.status)), QString::number(t.plannedMinutes), QString::number(actual)};
        csv += cells.join(';') + "\r\n";
    }
    return writeBytes(path, QByteArray("\xEF\xBB\xBF") + csv.toUtf8(), error);
}
