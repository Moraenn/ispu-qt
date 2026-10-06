#ifndef SUMMARYWIDGET_H
#define SUMMARYWIDGET_H
#include <QWidget>
class PlannerController;
class QLabel;
class QProgressBar;
class QTableWidget;
class SummaryWidget : public QWidget{
    Q_OBJECT
public:
    explicit SummaryWidget(PlannerController *controller, QWidget *parent = nullptr);
public slots:
    void refresh();
private:
    PlannerController *m_controller;
    QLabel *m_counts;
    QLabel *m_time;
    QProgressBar *m_progress;
    QTableWidget *m_subjects;
};
#endif
