#ifndef QTUTILS_H
#define QTUTILS_H
#include <QDate>
#include <QString>
#include "core/studyplan.h"
inline QString text(const std::string &value){ return QString::fromStdString(value); }
inline QString statusText(planner::Status value){
    switch (value){
    case planner::Status::Planned: return "Запланировано";
    case planner::Status::InProgress: return "В работе";
    case planner::Status::Done: return "Выполнено";
    }
    return {};
}
inline QString priorityText(planner::Priority value){
    switch (value){
    case planner::Priority::Low: return "Низкий";
    case planner::Priority::Normal: return "Обычный";
    case planner::Priority::High: return "Высокий";
    }
    return {};
}
inline std::string todayISO(){ return QDate::currentDate().toString(Qt::ISODate).toStdString(); }
inline QString minutesText(int value){ return QString("%1 ч %2 мин").arg(value / 60).arg(value % 60); }
#endif
