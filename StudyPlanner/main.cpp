#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QMessageBox>
#include <QStandardPaths>
#include <QTemporaryDir>
#include "plannercontroller.h"
#include "plannerwindow.h"
#include "qtutils.h"
#include "../common/capture.h"
namespace{
bool fillDemo(PlannerController &controller, QString &error){
    const QDate today = QDate::currentDate();
    const auto date = [today](int shift){ return today.addDays(shift).toString(Qt::ISODate).toStdString(); };
    const std::vector<planner::Task> tasks = {
        {0, "Кроссплатформенные приложения", "Сериализация справочника", date(3), planner::Priority::High, planner::Status::Planned, 120},
        {0, "Базы данных", "Подготовить запросы", date(-2), planner::Priority::Normal, planner::Status::Planned, 90},
        {0, "Методы принятия решений", "Решить задачу оптимизации", date(0), planner::Priority::Normal, planner::Status::Done, 60},
        {0, "Кроссплатформенные приложения", "Подготовка к защите", date(7), planner::Priority::Low, planner::Status::Planned, 60}
    };
    for (const auto &task : tasks) if (!controller.addTask(task, error)) return false;
    if (!controller.addSession({0, 1, date(-1), 45, "Запись и чтение коллекции"}, error)) return false;
    if (!controller.addSession({0, 3, date(-1), 60, "Решение и проверка"}, error)) return false;
    return true;
}
}
int main(int argc, char *argv[]){
    QApplication app(argc, argv);
    app.setOrganizationName("ISPU");
    app.setApplicationName("StudyPlanner");
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addOption({"data", "Путь к файлу данных", "path"});
    parser.addOption({"demo", "Демонстрационные данные во временной папке"});
    parser.addOption({"screenshot", "Сохранить снимок запущенного окна", "path"});
    parser.addOption({"tab", "Вкладка: 0, 1 или 2", "index", "0"});
    parser.process(app);
    QTemporaryDir demoDirectory;
    if (parser.isSet("demo") && !demoDirectory.isValid()) return 2;
    QString path = parser.isSet("data") ? parser.value("data") : QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).filePath("study-plan.json");
    if (parser.isSet("demo")) path = demoDirectory.filePath("demo.json");
    PlannerController controller(path);
    QString error;
    if (!controller.initialize(error) || (parser.isSet("demo") && !fillDemo(controller, error))){
        QMessageBox::critical(nullptr, "Не удалось открыть данные", error + "\nИсходный файл не заменён. Используйте другой путь --data или резервную копию.");
        return 1;
    }
    PlannerWindow window(&controller);
    if (parser.isSet("demo")) window.setWindowTitle("Планировщик учебных заданий — демонстрация");
    window.selectTab(parser.value("tab").toInt());
    window.show();
    captureIfRequested(&window);
    return app.exec();
}
