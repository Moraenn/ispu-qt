#ifndef STUDYPLAN_H
#define STUDYPLAN_H
#include <map>
#include <string>
#include <vector>
namespace planner{
enum class Priority { Low, Normal, High };
enum class Status { Planned, InProgress, Done };
struct Task{
    int id = 0;
    std::string subject;
    std::string title;
    std::string deadline; // YYYY-MM-DD; строки корректных дат можно сравнивать.
    Priority priority = Priority::Normal;
    Status status = Status::Planned;
    int plannedMinutes = 60;
};
struct StudySession{
    int id = 0;
    int taskId = 0;
    std::string date;
    int minutes = 30;
    std::string note;
};
struct Summary{
    int tasks = 0;
    int completed = 0;
    int overdue = 0;
    int plannedMinutes = 0;
    int actualMinutes = 0;
};
struct SubjectSummary{
    std::string subject;
    Summary totals;
};
bool validDate(const std::string &date);
bool overdue(const Task &task, const std::string &today);
class StudyPlan{
public:
    static constexpr int MaxTasks = 2000;
    static constexpr int MaxSessions = 10000;
    static constexpr int MaxId = 1000000000;
    const std::vector<Task> &tasks() const { return m_tasks; }
    const std::vector<StudySession> &sessions() const { return m_sessions; }
    const Task *task(int id) const;
    bool addTask(Task task, std::string &error);
    bool updateTask(const Task &task, std::string &error);
    bool removeTask(int id, std::string &error);
    bool addSession(StudySession session, std::string &error);
    bool removeSession(int id, std::string &error);
    bool replaceData(const std::vector<Task> &tasks, const std::vector<StudySession> &sessions, std::string &error);
    int spentMinutes(int taskId) const;
    std::map<int, int> spentByTask() const;
    Summary summary(const std::string &today) const;
    std::vector<SubjectSummary> bySubject(const std::string &today) const;
private:
    std::vector<Task> m_tasks;
    std::vector<StudySession> m_sessions;
    int m_nextTaskId = 1;
    int m_nextSessionId = 1;
};
}
#endif
