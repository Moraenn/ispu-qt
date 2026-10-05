#include "timer.h"
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTimer qtimer;
    qtimer.singleShot(3000, &a, &QCoreApplication::quit);

    Timer timer;
    QObject::connect(&timer, &Timer::test_signal, &a, &QCoreApplication::quit, Qt::QueuedConnection);


    timer.test_slot();



    auto val = a.exec();

    return val;
}
