#include <QApplication>
#include "mainwindow.h"
#include "../common/capture.h"
int main(int argc, char *argv[]){
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    captureIfRequested(&window);
    return app.exec();
}
