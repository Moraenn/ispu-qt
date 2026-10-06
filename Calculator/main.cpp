#include <QApplication>
#include "calculator.h"
#include "../common/capture.h"
int main(int argc, char *argv[]){
    QApplication app(argc, argv);
    Calculator window;
    if (app.arguments().contains("--demo")){
        for (const QString &s : {QString("1"), QString("2"), QString("+"), QString("3"), QString("=")}) window.input(s);
    }
    window.show();
    captureIfRequested(&window);
    return app.exec();
}
