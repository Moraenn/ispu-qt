QT += widgets
CONFIG += c++17
TEMPLATE = app
TARGET = Calculator

SOURCES += calculator.cpp \
    main.cpp

HEADERS += calculator.h

msvc:QMAKE_CXXFLAGS += /utf-8
