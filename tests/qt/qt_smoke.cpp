#include <QtTest>
#include <QAbstractItemModelTester>
#include <QCheckBox>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QSpinBox>
#include <QTemporaryDir>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QTableView>
#include <QTimer>
#include "../../Calculator/calculator.h"
#include "../../Alarm/alarmwindow.h"
#include "../../handbook/stampmodel.h"
#include "../../handbook/stampstorage.h"
#include "../../handbook/mainwindow.h"
#include "../../handbook/stampdialog.h"
#include "../../StudyPlanner/plannercontroller.h"
#include "../../StudyPlanner/plannerstorage.h"
#include "../../StudyPlanner/plannermodels.h"
#include "../../StudyPlanner/taskfilter.h"
#include "../../StudyPlanner/qtutils.h"
#include "../../StudyPlanner/plannerwindow.h"
#include "../../StudyPlanner/plannerdialogs.h"
namespace{
planner::Task sample(){ return {0, "Предмет", "Лабораторная", "2026-10-10", planner::Priority::Normal, planner::Status::Planned, 120}; }
void type(Calculator &c, const QString &value){ for (const QChar ch : value) c.input(QString(ch)); }
bool snapshot(QWidget *window, const QString &name){
    const QString directory = qEnvironmentVariable("ISPU_SCREENSHOT_DIR");
    if (directory.isEmpty()) return true;
    QDir().mkpath(directory);
    QTest::qWait(80);
    return window->grab().save(QDir(directory).filePath(name + ".png"));
}
void acceptDialog(QDialog *dialog){
    auto *buttons = dialog->findChild<QDialogButtonBox *>();
    QTest::mouseClick(buttons->button(QDialogButtonBox::Ok), Qt::LeftButton);
}
void chooseFileSoon(const QString &path){
    QTimer::singleShot(80, [path](){
        auto *dialog = qobject_cast<QFileDialog *>(QApplication::activeModalWidget());
        QVERIFY(dialog);
        dialog->setDirectory(QFileInfo(path).absolutePath());
        QTest::qWait(150);
        auto *fileName = dialog->findChild<QLineEdit *>("fileNameEdit");
        QVERIFY(fileName);
        fileName->setText(QFileInfo(path).fileName());
        QVERIFY(QMetaObject::invokeMethod(dialog, "accept", Qt::DirectConnection));
    });
}
}
class QtSmoke : public QObject{
    Q_OBJECT
private slots:
    void initTestCase(){ QApplication::setAttribute(Qt::AA_DontUseNativeDialogs); }
    void calculatorArithmetic(){
        Calculator c;
        auto *display = c.findChild<QLabel *>("display");
        QVERIFY(display);
        type(c, "12+3="); QCOMPARE(display->text(), QString("15"));
        c.clearAll(); type(c, "8/0="); QCOMPARE(display->text(), QString("Ошибка"));
        type(c, "2+2="); QCOMPARE(display->text(), QString("4"));
        c.clearAll(); type(c, "-3*-2="); QCOMPARE(display->text(), QString("6"));
        c.clearAll(); type(c, "1.5+2.5="); QCOMPARE(display->text(), QString("4"));
    }
    void calculatorScientificAndKeyboard(){
        Calculator c;
        auto *display = c.findChild<QLabel *>("display");
        type(c, "1/1000000000=*2=");
        QCOMPARE(display->text().toDouble(), 2e-9);
        c.clearAll(); QTest::keyClicks(&c, "2+3=");
        QCOMPARE(display->text(), QString("5"));
    }
    void alarmControls(){
        AlarmWindow a;
        auto *hour = a.findChild<QSpinBox *>("alarmHour");
        auto *minute = a.findChild<QSpinBox *>("alarmMinute");
        auto *enabled = a.findChild<QCheckBox *>("alarmEnabled");
        QVERIFY(hour && minute && enabled);
        QCOMPARE(hour->minimum(), 0); QCOMPARE(hour->maximum(), 23);
        QCOMPARE(minute->minimum(), 0); QCOMPARE(minute->maximum(), 59);
        QVERIFY(!enabled->isChecked());
    }
    void stampModelValidation(){
        StampModel model;
        QAbstractItemModelTester tester(&model, QAbstractItemModelTester::FailureReportingMode::QtTest);
        QVERIFY(!model.setData(model.index(0, 0), "  "));
        QVERIFY(!model.setData(model.index(0, 2), "oops"));
        QVERIFY(!model.setData(model.index(0, 3), -1));
        QVERIFY(model.setData(model.index(0, 0), "Новая марка"));
        QCOMPARE(model.stampAt(0).title, QString("Новая марка"));
        QVERIFY(model.replaceStamps({}));
        QCOMPARE(model.rowCount(), 0);
        QVERIFY(!model.removeStampAt(0));
    }
    void stampRoundTrip(){
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath("collection.stamps");
        QVector<Stamp> source = {{"Космос", "Россия", 2001, 3000}};
        QVector<Stamp> loaded;
        QString error;
        QVERIFY2(StampStorage::save(path, source, error), qPrintable(error));
        QVERIFY2(StampStorage::load(path, loaded, error), qPrintable(error));
        QCOMPARE(loaded.size(), source.size());
        QCOMPARE(loaded[0].title, source[0].title);
        QCOMPARE(loaded[0].country, source[0].country);
        QCOMPARE(loaded[0].year, source[0].year);
        QCOMPARE(loaded[0].circulation, source[0].circulation);
        QVERIFY(StampStorage::save(path, {}, error));
        QVERIFY(StampStorage::load(path, loaded, error));
        QVERIFY(loaded.isEmpty());
    }
    void damagedStampKeepsCollection(){
        QTemporaryDir dir;
        QFile file(dir.filePath("broken.stamps"));
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write("bad file contents"); file.close();
        QVector<Stamp> old = {{"Старая марка", "Страна", 2000, 1}};
        QString error;
        QVERIFY(!StampStorage::load(file.fileName(), old, error));
        QCOMPARE(old.size(), 1); QCOMPARE(old[0].title, QString("Старая марка"));
        QVERIFY(!StampStorage::save(dir.path(), old, error));
    }
    void plannerRoundTrip(){
        QTemporaryDir dir;
        const QString path = dir.filePath("planner.json");
        planner::StudyPlan plan, restored;
        std::string e; QString error;
        QVERIFY(plan.addTask(sample(), e));
        QVERIFY(plan.addSession({0, 1, "2026-10-06", 45, "Проверка UTF-8"}, e));
        QVERIFY(PlannerStorage::save(path, plan, error));
        QVERIFY(PlannerStorage::load(path, restored, error));
        QCOMPARE(restored.tasks().size(), std::size_t(1));
        QCOMPARE(text(restored.task(1)->title), QString("Лабораторная"));
        QCOMPARE(restored.spentMinutes(1), 45);
    }
    void invalidJsonPreservesData(){
        QTemporaryDir dir;
        planner::StudyPlan plan; std::string e; QString error;
        QVERIFY(plan.addTask(sample(), e));
        const QString path = dir.filePath("bad.json");
        QFile file(path); QVERIFY(file.open(QIODevice::WriteOnly));
        file.write("{\"schemaVersion\":999,\"tasks\":[],\"sessions\":[]}"); file.close();
        QVERIFY(!PlannerStorage::load(path, plan, error));
        QCOMPARE(plan.tasks().size(), std::size_t(1));
    }
    void failedWriteDoesNotCommit(){
        QTemporaryDir dir;
        const QString path = dir.filePath("data.json");
        PlannerController controller(path); QString error;
        QVERIFY(controller.initialize(error));
        QVERIFY(QFile::remove(path));
        QVERIFY(QDir().mkdir(path)); // Вместо файла — каталог: запись должна завершиться ошибкой.
        QSignalSpy changed(&controller, &PlannerController::changed);
        QVERIFY(!controller.addTask(sample(), error));
        QVERIFY(controller.plan().tasks().empty());
        QCOMPARE(changed.count(), 0);
    }
    void filteringAndIds(){
        QTemporaryDir dir; QString error;
        PlannerController c(dir.filePath("data.json"));
        QVERIFY(c.initialize(error));
        auto a = sample(); a.title = "Alpha"; a.deadline = QDate::currentDate().addDays(-1).toString(Qt::ISODate).toStdString();
        auto b = sample(); b.title = "Beta"; b.status = planner::Status::Done;
        QVERIFY(c.addTask(a, error)); QVERIFY(c.addTask(b, error));
        TaskModel model(&c);
        QAbstractItemModelTester tester(&model, QAbstractItemModelTester::FailureReportingMode::QtTest);
        TaskFilter filter; filter.setSourceModel(&model);
        filter.sort(TaskModel::Title, Qt::DescendingOrder);
        QCOMPARE(filter.index(0, 0).data(TaskModel::IdRole).toInt(), 2);
        filter.setStatus(int(planner::Status::Planned)); QCOMPARE(filter.rowCount(), 1);
        QCOMPARE(model.taskAt(filter.mapToSource(filter.index(0, 0)).row()).id, 1);
        filter.setStatus(-1); filter.setDateMode(TaskFilter::Overdue); QCOMPARE(filter.rowCount(), 1);
        filter.setSearch("не существует"); QCOMPARE(filter.rowCount(), 0);
    }
    void csvEscaping(){
        QTemporaryDir dir;
        planner::StudyPlan plan; std::string e; QString error;
        auto t = sample(); t.title = "=SUM(1;2)";
        QVERIFY(plan.addTask(t, e));
        const QString path = dir.filePath("tasks.csv");
        QVERIFY(PlannerStorage::exportCsv(path, plan, error));
        QFile file(path); QVERIFY(file.open(QIODevice::ReadOnly));
        QVERIFY(file.readAll().contains("\"'=SUM(1;2)\""));
    }
    void alarmSingleTrigger(){
        AlarmWindow window;
        window.show();
        // Stop the periodic timer so the test controls exactly which ticks occur.
        for (auto *timer : window.findChildren<QTimer *>()) timer->stop();
        const QTime now = QTime::currentTime();
        window.findChild<QSpinBox *>("alarmHour")->setValue(now.hour());
        window.findChild<QSpinBox *>("alarmMinute")->setValue(now.minute());
        window.findChild<QCheckBox *>("alarmEnabled")->setChecked(true);
        int messages = 0;
        QTimer::singleShot(80, [&](){
            auto *message = qobject_cast<QMessageBox *>(QApplication::activeModalWidget());
            QVERIFY(message);
            QCOMPARE(message->text(), QString("Пора вставать!"));
            ++messages;
            QVERIFY(snapshot(message, "lab2_signal"));
            message->accept();
        });
        QVERIFY(QMetaObject::invokeMethod(&window, "updateCurrentTime"));
        QCOMPARE(messages, 1);
        QVERIFY(QMetaObject::invokeMethod(&window, "updateCurrentTime"));
        QCOMPARE(messages, 1);
    }
    void stampDialogSaveReopenAndCancel(){
        QTemporaryDir directory;
        MainWindow window;
        window.show();
        auto *model = window.findChild<StampModel *>();
        QVERIFY(model);
        const int initial = model->rowCount();
        QTimer::singleShot(80, [&](){
            auto *dialog = qobject_cast<StampDialog *>(QApplication::activeModalWidget());
            QVERIFY(dialog);
            dialog->setStamp({"Исследование космоса", "Россия", 2026, 3000});
            QVERIFY(snapshot(dialog, "lab4_dialog"));
            acceptDialog(dialog);
        });
        QVERIFY(QMetaObject::invokeMethod(&window, "addStamp"));
        QCOMPARE(model->rowCount(), initial + 1);
        QVERIFY(window.isWindowModified());
        QTimer::singleShot(80, [](){
            auto *message = qobject_cast<QMessageBox *>(QApplication::activeModalWidget());
            QVERIFY(message);
            QTest::mouseClick(message->button(QMessageBox::Cancel), Qt::LeftButton);
        });
        QVERIFY(!window.close());
        QVERIFY(window.isVisible());
        const QString path = directory.filePath("collection.stamps");
        chooseFileSoon(path);
        bool saved = false;
        QVERIFY(QMetaObject::invokeMethod(&window, "saveFile", Q_RETURN_ARG(bool, saved)));
        QVERIFY(saved);
        QVERIFY(QFile::exists(path));
        QVERIFY(!window.isWindowModified());
        QVERIFY(window.close());
        MainWindow reopened;
        reopened.show();
        chooseFileSoon(path);
        QVERIFY(QMetaObject::invokeMethod(&reopened, "openFile"));
        auto *restored = reopened.findChild<StampModel *>();
        QCOMPARE(restored->rowCount(), initial + 1);
        QCOMPARE(restored->stampAt(initial).title, QString("Исследование космоса"));
        QVERIFY(snapshot(&reopened, "lab4_saved"));
        QVERIFY(reopened.close());
    }
    void plannerDialogWorkflowAndRestart(){
        QTemporaryDir directory;
        const QString path = directory.filePath("study-plan.json");
        QString error;
        {
            PlannerController controller(path);
            QVERIFY(controller.initialize(error));
            PlannerWindow window(&controller);
            window.show();
            QTimer::singleShot(80, [&](){
                auto *dialog = qobject_cast<TaskDialog *>(QApplication::activeModalWidget());
                QVERIFY(dialog);
                auto task = sample();
                task.subject = "Кроссплатформенные приложения";
                task.title = "Подготовка к защите";
                task.deadline = QDate::currentDate().addDays(7).toString(Qt::ISODate).toStdString();
                dialog->setTask(task);
                QVERIFY(snapshot(dialog, "lab5_dialog"));
                acceptDialog(dialog);
            });
            QVERIFY(QMetaObject::invokeMethod(&window, "addTask"));
            QCOMPARE(controller.plan().tasks().size(), std::size_t(1));
            auto *view = window.findChild<QTableView *>("taskTable");
            QVERIFY(view);
            view->selectRow(0);
            QTimer::singleShot(80, [](){
                auto *dialog = qobject_cast<SessionDialog *>(QApplication::activeModalWidget());
                QVERIFY(dialog);
                dialog->findChild<QLineEdit *>()->setText("Разобраны классы и сохранение данных");
                acceptDialog(dialog);
            });
            QVERIFY(QMetaObject::invokeMethod(&window, "addSession"));
            QCOMPARE(controller.plan().spentMinutes(1), 45);
            QCOMPARE(controller.plan().task(1)->status, planner::Status::InProgress);
            view->selectRow(0);
            QVERIFY(QMetaObject::invokeMethod(&window, "completeTask"));
            QCOMPARE(controller.plan().task(1)->status, planner::Status::Done);
            window.selectTab(2);
            auto *progress = window.findChild<QProgressBar *>();
            QVERIFY(progress);
            QCOMPARE(progress->value(), 100);
            QVERIFY(snapshot(&window, "lab5_progress"));
            window.close();
        }
        PlannerController restarted(path);
        QVERIFY(restarted.initialize(error));
        QCOMPARE(restarted.plan().tasks().size(), std::size_t(1));
        QCOMPARE(restarted.plan().sessions().size(), std::size_t(1));
        QCOMPARE(restarted.plan().spentMinutes(1), 45);
        QCOMPARE(restarted.plan().task(1)->status, planner::Status::Done);
    }
    void plannerBackupAndRestore(){
        QTemporaryDir directory;
        QString error;
        PlannerController controller(directory.filePath("study-plan.json"));
        QVERIFY(controller.initialize(error));
        QVERIFY(controller.addTask(sample(), error));
        QVERIFY(controller.addSession({0, 1, todayISO(), 45, "Запись занятия"}, error));
        const QString backup = directory.filePath("backup.json");
        QVERIFY(controller.backup(backup, error));
        QVERIFY(controller.removeTask(1, error));
        QVERIFY(controller.plan().sessions().empty());
        QVERIFY(controller.importData(backup, error));
        QCOMPARE(controller.plan().spentMinutes(1), 45);
        QCOMPARE(controller.plan().tasks().size(), std::size_t(1));
        QCOMPARE(controller.plan().sessions().size(), std::size_t(1));
    }
};
QTEST_MAIN(QtSmoke)
#include "qt_smoke.moc"
