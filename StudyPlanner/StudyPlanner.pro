QT += widgets
CONFIG += c++17
TEMPLATE = app
TARGET = StudyPlanner

SOURCES += main.cpp \
    plannercontroller.cpp \
    plannerdialogs.cpp \
    plannermodels.cpp \
    plannerstorage.cpp \
    plannerwindow.cpp \
    summarywidget.cpp \
    taskdelegate.cpp \
    taskfilter.cpp \
    core/studyplan.cpp

HEADERS += plannercontroller.h \
    plannerdialogs.h \
    plannermodels.h \
    plannerstorage.h \
    plannerwindow.h \
    qtutils.h \
    summarywidget.h \
    taskdelegate.h \
    taskfilter.h \
    core/studyplan.h

msvc:QMAKE_CXXFLAGS += /utf-8
