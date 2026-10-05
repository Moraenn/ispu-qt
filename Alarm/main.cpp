#include <QApplication>

#include "alarmwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    AlarmWindow window;
    window.show();

    return app.exec();
}
