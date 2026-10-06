#include <QApplication>
#include "alarmwindow.h"
#include "../common/capture.h"
int main(int argc, char *argv[]){
    QApplication app(argc, argv);
    AlarmWindow window;
    window.show();
    captureIfRequested(&window);
    return app.exec();
}
