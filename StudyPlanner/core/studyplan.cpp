#include "studyplan.h"
#include <algorithm>
#include <cctype>
#include <set>
namespace planner{
namespace{
bool fail(std::string &error, const std::string &message){ error = message; return false; }
bool meaningful(const std::string &value, std::size_t limit){
    return !value.empty() && value.size() <= limit && value.find('\0') == std::string::npos &&
           std::any_of(value.begin(), value.end(), [](unsigned char c){ return !std::isspace(c); });
}
bool validTask(const Task &t){
    const int priority = int(t.priority), status = int(t.status);
    return t.id > 0 && t.id <= StudyPlan::MaxId && meaningful(t.subject, 300) && meaningful(t.title, 500) &&
           validDate(t.deadline) && priority >= 0 && priority <= 2 && status >= 0 && status <= 2 &&
           t.plannedMinutes >= 1 && t.plannedMinutes <= 6000;
}
bool validSession(const StudySession &s){
    return s.id > 0 && s.id <= StudyPlan::MaxId && s.taskId > 0 && validDate(s.date) &&
           s.minutes >= 1 && s.minutes <= 720 && s.note.size() <= 2000 && s.note.find('\0') == std::string::npos;
}
}
bool validDate(const std::string &d){
    if (d.size() != 10 || d[4] != '-' || d[7] != '-') return false;
    for (std::size_t i = 0; i < d.size(); ++i){
        if (i != 4 && i != 7 && (d[i] < '0' || d[i] > '9')) return false;
    }
    const int year = (d[0] - '0') * 1000 + (d[1] - '0') * 100 + (d[2] - '0') * 10 + d[3] - '0';
    const int month = (d[5] - '0') * 10 + d[6] - '0', day = (d[8] - '0') * 10 + d[9] - '0';
    if (year < 1900 || year > 2100 || month < 1 || month > 12) return false;
    const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int limit = days[month - 1];
    if (month == 2 && year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ++limit;
    return day >= 1 && day <= limit;
}
bool overdue(const Task &t, const std::string &today){
    return validDate(today) && t.status != Status::Done && t.deadline < today;
}
const Task *StudyPlan::task(int id) const{
    auto it = std::find_if(m_tasks.begin(), m_tasks.end(), [id](const Task &t){ return t.id == id; });
    return it == m_tasks.end() ? nullptr : &*it;
}
bool StudyPlan::addTask(Task t, std::string &error){
    error.clear();
    t.id = m_nextTaskId;
    if (m_tasks.size() >= MaxTasks || !validTask(t)) return fail(error, "Некорректное задание или превышен лимит записей.");
    m_tasks.push_back(t);
    ++m_nextTaskId;
    return true;
}
bool StudyPlan::updateTask(const Task &t, std::string &error){
    error.clear();
    if (!validTask(t)) return fail(error, "Проверьте предмет, название, срок и плановое время.");
    auto it = std::find_if(m_tasks.begin(), m_tasks.end(), [&t](const Task &old){ return old.id == t.id; });
    if (it == m_tasks.end()) return fail(error, "Задание не найдено.");
    *it = t;
    return true;
}
bool StudyPlan::removeTask(int id, std::string &error){
    error.clear();
    if (!task(id)) return fail(error, "Задание не найдено.");
    m_sessions.erase(std::remove_if(m_sessions.begin(), m_sessions.end(), [id](const StudySession &s){ return s.taskId == id; }), m_sessions.end());
    m_tasks.erase(std::remove_if(m_tasks.begin(), m_tasks.end(), [id](const Task &t){ return t.id == id; }), m_tasks.end());
    return true;
}
bool StudyPlan::addSession(StudySession s, std::string &error){
    error.clear();
    s.id = m_nextSessionId;
    if (m_sessions.size() >= MaxSessions || !validSession(s)) return fail(error, "Длительность занятия должна быть от 1 до 720 минут. Проверьте дату и примечание.");
    if (!task(s.taskId)) return fail(error, "У занятия должно быть существующее задание.");
    m_sessions.push_back(s);
    ++m_nextSessionId;
    for (Task &t : m_tasks){
        if (t.id == s.taskId && t.status == Status::Planned) t.status = Status::InProgress;
    }
    return true;
}
bool StudyPlan::removeSession(int id, std::string &error){
    error.clear();
    auto it = std::find_if(m_sessions.begin(), m_sessions.end(), [id](const StudySession &s){ return s.id == id; });
    if (it == m_sessions.end()) return fail(error, "Занятие не найдено.");
    m_sessions.erase(it);
    // Статус задания не откатываем: он описывает работу, а не наличие записей.
    return true;
}
bool StudyPlan::replaceData(const std::vector<Task> &tasks, const std::vector<StudySession> &sessions, std::string &error){
    error.clear();
    if (tasks.size() > MaxTasks || sessions.size() > MaxSessions) return fail(error, "Превышен лимит записей.");
    std::set<int> taskIds, sessionIds;
    int nextTask = 1, nextSession = 1;
    for (const Task &t : tasks){
        if (!validTask(t) || !taskIds.insert(t.id).second) return fail(error, "Неверные поля задания или повторяющийся идентификатор.");
        nextTask = std::max(nextTask, t.id + 1);
    }
    for (const StudySession &s : sessions){
        if (!validSession(s) || !sessionIds.insert(s.id).second || !taskIds.count(s.taskId)) return fail(error, "Неверное занятие, повторяющийся идентификатор или отсутствующее задание.");
        nextSession = std::max(nextSession, s.id + 1);
    }
    m_tasks = tasks;
    m_sessions = sessions;
    m_nextTaskId = nextTask;
    m_nextSessionId = nextSession;
    return true;
}
std::map<int, int> StudyPlan::spentByTask() const{
    std::map<int, int> result;
    for (const StudySession &s : m_sessions) result[s.taskId] += s.minutes;
    return result;
}
int StudyPlan::spentMinutes(int taskId) const{
    int result = 0;
    for (const StudySession &s : m_sessions) if (s.taskId == taskId) result += s.minutes;
    return result;
}
Summary StudyPlan::summary(const std::string &today) const{
    Summary result;
    result.tasks = int(m_tasks.size());
    for (const Task &t : m_tasks){
        result.completed += t.status == Status::Done;
        result.overdue += overdue(t, today);
        result.plannedMinutes += t.plannedMinutes;
    }
    for (const StudySession &s : m_sessions) result.actualMinutes += s.minutes;
    return result;
}
std::vector<SubjectSummary> StudyPlan::bySubject(const std::string &today) const{
    std::map<std::string, Summary> groups;
    const auto spent = spentByTask();
    for (const Task &t : m_tasks){
        Summary &s = groups[t.subject];
        ++s.tasks;
        s.completed += t.status == Status::Done;
        s.overdue += overdue(t, today);
        s.plannedMinutes += t.plannedMinutes;
        const auto it = spent.find(t.id);
        if (it != spent.end()) s.actualMinutes += it->second;
    }
    std::vector<SubjectSummary> result;
    for (const auto &item : groups) result.push_back({item.first, item.second});
    return result;
}
}
