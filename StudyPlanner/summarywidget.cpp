#include <QStringList>
#include "summarywidget.h"
#include "plannercontroller.h"
#include "qtutils.h"
#include <QHeaderView>
#include <QLabel>
#include <QProgressBar>
#include <QTableWidget>
#include <QVBoxLayout>
SummaryWidget::SummaryWidget(PlannerController *c, QWidget *parent) : QWidget(parent), m_controller(c){
    m_counts = new QLabel(this);
    m_time = new QLabel(this);
    m_progress = new QProgressBar(this);
    m_progress->setRange(0, 100);
    m_subjects = new QTableWidget(this);
    m_subjects->setColumnCount(6);
    m_subjects->setHorizontalHeaderLabels({"Предмет", "Заданий", "Выполнено", "Просрочено", "План, мин", "Факт, мин"});
    m_subjects->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_subjects->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_subjects->setAlternatingRowColors(true);
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Сводка по всем заданиям, независимо от фильтров", this));
    layout->addWidget(m_counts);
    layout->addWidget(new QLabel("Доля выполненных заданий", this));
    layout->addWidget(m_progress);
    layout->addWidget(m_time);
    layout->addWidget(m_subjects);
    connect(c, &PlannerController::changed, this, &SummaryWidget::refresh);
    refresh();
}
void SummaryWidget::refresh(){
    const auto s = m_controller->plan().summary(todayISO());
    m_counts->setText(QString("Всего: %1     Выполнено: %2     Просрочено: %3").arg(s.tasks).arg(s.completed).arg(s.overdue));
    m_progress->setValue(s.tasks ? 100 * s.completed / s.tasks : 0);
    m_time->setText(QString("План: %1     Затрачено: %2").arg(minutesText(s.plannedMinutes), minutesText(s.actualMinutes)));
    const auto subjects = m_controller->plan().bySubject(todayISO());
    m_subjects->setRowCount(int(subjects.size()));
    for (int row = 0; row < int(subjects.size()); ++row){
        const auto &item = subjects.at(row);
        const auto &v = item.totals;
        const QStringList cells = {text(item.subject), QString::number(v.tasks), QString::number(v.completed),
                                   QString::number(v.overdue), QString::number(v.plannedMinutes), QString::number(v.actualMinutes)};
        for (int col = 0; col < cells.size(); ++col) m_subjects->setItem(row, col, new QTableWidgetItem(cells.at(col)));
    }
}
