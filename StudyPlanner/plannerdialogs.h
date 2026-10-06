#ifndef PLANNERDIALOGS_H
#define PLANNERDIALOGS_H
#include <QDialog>
#include <QStringList>
#include "core/studyplan.h"
class QLineEdit;
class QComboBox;
class QDateEdit;
class QSpinBox;
class TaskDialog : public QDialog{
    Q_OBJECT
public:
    explicit TaskDialog(const QStringList &subjects, QWidget *parent = nullptr);
    void setTask(const planner::Task &task);
    planner::Task task() const;
private:
    int m_id = 0;
    QLineEdit *m_title;
    QComboBox *m_subject;
    QDateEdit *m_deadline;
    QComboBox *m_priority;
    QComboBox *m_status;
    QSpinBox *m_minutes;
};
class SessionDialog : public QDialog{
    Q_OBJECT
public:
    explicit SessionDialog(const std::vector<planner::Task> &tasks, int selectedId, QWidget *parent = nullptr);
    planner::StudySession session() const;
private:
    QComboBox *m_task;
    QDateEdit *m_date;
    QSpinBox *m_minutes;
    QLineEdit *m_note;
};
#endif
