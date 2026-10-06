add_executable(qt_smoke
    qt_smoke.cpp
    ../../Calculator/calculator.cpp ../../Calculator/calculator.h
    ../../Alarm/alarmwindow.cpp ../../Alarm/alarmwindow.h
    ../../handbook/stampmodel.cpp ../../handbook/stampmodel.h
    ../../handbook/stampstorage.cpp ../../handbook/stampstorage.h
    ../../handbook/stampdelegate.cpp ../../handbook/stampdelegate.h
    ../../handbook/mainwindow.cpp ../../handbook/mainwindow.h
    ../../handbook/stamptableview.cpp ../../handbook/stamptableview.h
    ../../handbook/stampdialog.cpp ../../handbook/stampdialog.h
    ../../StudyPlanner/plannerstorage.cpp ../../StudyPlanner/plannerstorage.h
    ../../StudyPlanner/plannercontroller.cpp ../../StudyPlanner/plannercontroller.h
    ../../StudyPlanner/plannermodels.cpp ../../StudyPlanner/plannermodels.h
    ../../StudyPlanner/taskfilter.cpp ../../StudyPlanner/taskfilter.h
    ../../StudyPlanner/plannerwindow.cpp ../../StudyPlanner/plannerwindow.h
    ../../StudyPlanner/plannerdialogs.cpp ../../StudyPlanner/plannerdialogs.h
    ../../StudyPlanner/taskdelegate.cpp ../../StudyPlanner/taskdelegate.h
    ../../StudyPlanner/summarywidget.cpp ../../StudyPlanner/summarywidget.h
)
target_link_libraries(qt_smoke PRIVATE studyplan_core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Test)
add_test(NAME qt_smoke COMMAND qt_smoke)
set_tests_properties(qt_smoke PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
set_tests_properties(qt_smoke PROPERTIES TIMEOUT 90)
