#ifndef CAPTURE_H
#define CAPTURE_H
#include <QStringList>
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QTimer>
#include <QWidget>
#include <QPixmap>
// Снимок создаётся только запущенным приложением. Обычный запуск не меняется.
inline void captureIfRequested(QWidget *window){
    const QStringList args = QCoreApplication::arguments();
    const int pos = args.indexOf("--screenshot");
    if (pos < 0) return;
    if (pos + 1 >= args.size()){
        QTimer::singleShot(0, [](){ QCoreApplication::exit(2); });
        return;
    }
    const QString path = args.at(pos + 1);
    QTimer::singleShot(400, window, [window, path](){
        QDir().mkpath(QFileInfo(path).absolutePath());
        const bool ok = window->grab().save(path, "PNG");
        QCoreApplication::exit(ok ? 0 : 2);
    });
}
#endif
