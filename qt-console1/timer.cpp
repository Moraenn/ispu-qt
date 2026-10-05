#include "timer.h"
#include <QDebug>


Timer::Timer(QObject *parent)
    : QObject{parent}
{}

void Timer::test_slot()
{
    qInfo() << "[INFO] test_slot() was invoked";
    qInfo() << "[INFO] emitting 'test_signal()'";

    emit test_signal();
}
