#ifndef PLANNERWINDOW_H
#define PLANNERWINDOW_H
#include <QMainWindow>
#include <QStringList>
class PlannerController;
class TaskModel;
class TaskFilter;
class SessionModel;
class QSortFilterProxyModel;
class QTableView;
class QTabWidget;
class QPushButton;
class SummaryWidget;
class PlannerWindow : public QMainWindow{
    Q_OBJECT
public:
    explicit PlannerWindow(PlannerController *controller, QWidget *parent = nullptr);
    void selectTab(int index);
private slots:
    void addTask();
    void editTask();
    void removeTask();
    void completeTask();
    void addSession();
    void removeSession();
    void backup();
    void restore();
    void exportCsv();
    void updateActions();
private:
    int selectedTaskId() const;
    QStringList subjects() const;
    void selectTask(int id);
    void showError(const QString &error);
    PlannerController *m_controller;
    TaskModel *m_tasks;
    TaskFilter *m_filter;
    SessionModel *m_sessions;
    QSortFilterProxyModel *m_sessionSort;
    QTableView *m_taskView;
    QTableView *m_sessionView;
    QTabWidget *m_tabs;
    QPushButton *m_edit;
    QPushButton *m_remove;
    QPushButton *m_complete;
    QPushButton *m_addSession;
    QPushButton *m_removeSession;
    SummaryWidget *m_summary;
};
#endif
