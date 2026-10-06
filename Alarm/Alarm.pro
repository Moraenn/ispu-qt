QT += widgets
CONFIG += c++17
TEMPLATE = app
TARGET = Alarm

SOURCES += alarmwindow.cpp \
    main.cpp

HEADERS += alarmwindow.h

msvc:QMAKE_CXXFLAGS += /utf-8
