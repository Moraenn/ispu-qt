#include "studyplan.h"
#include <iostream>
#include <string>
#include <vector>
using namespace planner;
int passed = 0, failed = 0;
void check(bool value, const std::string &name){
    std::cout << (value ? "PASS " : "FAIL ") << name << '\n';
    value ? ++passed : ++failed;
}
Task sample(std::string title = "Лабораторная работа"){
    return {0, "Кроссплатформенные приложения", title, "2026-10-10", Priority::Normal, Status::Planned, 120};
}
int main(){
    check(validDate("2024-02-29"), "leap year");
    check(validDate("2000-02-29"), "leap century");
    check(!validDate("1900-02-29"), "non-leap century");
    check(!validDate("2025-02-29"), "non-leap year");
    check(!validDate("2026-04-31"), "invalid day");
    check(!validDate("2026-00-10"), "invalid month");
    check(!validDate("2026-13-10"), "month overflow");
    check(!validDate("2026-10-00"), "zero day");
    check(!validDate("2026-1-10"), "non-ISO date");
    check(!validDate("abcd-10-10"), "non-numeric year");
    check(!validDate("1899-12-31"), "date below range");
    check(validDate("2100-12-31"), "upper date boundary");
    StudyPlan plan;
    std::string error;
    check(plan.addTask(sample(), error), "add valid task");
    check(plan.tasks().size() == 1 && plan.tasks()[0].id == 1, "id assigned");
    Task invalid = sample(); invalid.title = "  ";
    check(!plan.addTask(invalid, error) && plan.tasks().size() == 1, "empty title rejected without mutation");
    invalid = sample(); invalid.subject.clear();
    check(!plan.addTask(invalid, error), "empty subject rejected");
    invalid = sample(); invalid.plannedMinutes = 0;
    check(!plan.addTask(invalid, error), "zero plan rejected");
    invalid.plannedMinutes = 6001;
    check(!plan.addTask(invalid, error), "plan above limit rejected");
    invalid = sample(); invalid.status = Status(3);
    check(!plan.addTask(invalid, error), "invalid status rejected");
    invalid = sample(); invalid.priority = Priority(-1);
    check(!plan.addTask(invalid, error), "invalid priority rejected");
    invalid = sample(); invalid.title = std::string(501, 'x');
    check(!plan.addTask(invalid, error), "long title rejected");
    invalid = sample(); invalid.title = std::string("x\0y", 3);
    check(!plan.addTask(invalid, error), "embedded NUL rejected");
    check(plan.addTask(sample("Второе задание"), error) && plan.tasks()[1].id == 2, "failed additions do not consume ids");
    check(plan.addSession({0, 1, "2026-10-06", 45, "Начало работы"}, error), "add session");
    check(plan.task(1)->status == Status::InProgress, "first session starts task");
    check(plan.spentMinutes(1) == 45, "actual time calculated");
    check(!plan.addSession({0, 99, "2026-10-06", 30, ""}, error), "unknown task rejected");
    check(!plan.addSession({0, 1, "2026-10-06", 0, ""}, error), "zero session rejected");
    check(!plan.addSession({0, 1, "2026-10-06", 721, ""}, error), "long session rejected");
    check(!plan.addSession({0, 1, "2026-02-30", 30, ""}, error), "bad session date rejected");
    check(plan.addSession({0, 1, "2026-10-06", 90, "Продолжение"}, error), "second session");
    check(plan.spentMinutes(1) == 135, "actual time can exceed plan");
    Task edited = *plan.task(1); edited.status = Status::Done;
    check(plan.updateTask(edited, error), "complete task");
    check(!overdue(*plan.task(1), "2026-10-12"), "done task is not overdue");
    check(overdue(*plan.task(2), "2026-10-12"), "unfinished task overdue");
    check(!overdue(*plan.task(2), "2026-10-10"), "deadline today is not overdue");
    check(!overdue(*plan.task(2), "bad date"), "invalid reference date rejected");
    const Summary sum = plan.summary("2026-10-12");
    check(sum.tasks == 2 && sum.completed == 1 && sum.overdue == 1, "summary counts");
    check(sum.plannedMinutes == 240 && sum.actualMinutes == 135, "summary time");
    auto subjects = plan.bySubject("2026-10-12");
    check(subjects.size() == 1 && subjects[0].totals.actualMinutes == 135, "subject summary");
    edited = *plan.task(2); edited.subject = "Базы данных";
    check(plan.updateTask(edited, error) && plan.bySubject("2026-10-12").size() == 2, "subject grouping updated");
    const auto savedTasks = plan.tasks(); const auto savedSessions = plan.sessions();
    StudyPlan restored;
    check(restored.replaceData(savedTasks, savedSessions, error), "restore valid snapshot");
    check(restored.summary("2026-10-12").actualMinutes == 135, "restored relations");
    auto duplicates = savedTasks; duplicates.push_back(savedTasks[0]);
    check(!restored.replaceData(duplicates, savedSessions, error), "duplicate task ids rejected");
    check(restored.tasks().size() == 2 && restored.sessions().size() == 2, "invalid restore preserves old data");
    auto orphan = savedSessions; orphan[0].taskId = 88;
    check(!restored.replaceData(savedTasks, orphan, error), "orphan session rejected");
    auto repeated = savedSessions; repeated.push_back(repeated[0]);
    check(!restored.replaceData(savedTasks, repeated, error), "duplicate session ids rejected");
    auto bad = savedTasks; bad[0].id = 0;
    check(!restored.replaceData(bad, {}, error), "invalid imported id rejected");
    check(restored.addTask(sample("Третье"), error) && restored.tasks().back().id == 3, "id sequence restored");
    check(restored.removeSession(1, error) && restored.spentMinutes(1) == 90, "remove session recalculates time");
    check(!restored.removeSession(1, error), "remove missing session");
    check(restored.removeTask(1, error), "remove task");
    check(restored.sessions().empty() && restored.tasks().size() == 2, "cascade removes related sessions only");
    check(!restored.removeTask(1, error), "remove missing task");
    edited.id = 999;
    check(!restored.updateTask(edited, error), "update missing task");
    check(restored.replaceData({}, {}, error) && restored.summary("2026-10-06").tasks == 0, "empty snapshot supported");
    std::vector<Task> maximum(StudyPlan::MaxTasks, sample());
    for (int i = 0; i < StudyPlan::MaxTasks; ++i) maximum[i].id = i + 1;
    check(restored.replaceData(maximum, {}, error), "maximum task count supported");
    check(!restored.addTask(sample(), error), "task count limit");
    auto edge = sample(); edge.id = StudyPlan::MaxId;
    check(restored.replaceData({edge}, {}, error) && !restored.addTask(sample(), error), "id overflow prevented");
    std::cout << "TOTAL " << passed << " passed, " << failed << " failed\n";
    return failed ? 1 : 0;
}
