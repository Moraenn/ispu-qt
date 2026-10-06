QT += widgets
CONFIG += c++17
TEMPLATE = app
TARGET = handbook

SOURCES += main.cpp \
    mainwindow.cpp \
    stampdelegate.cpp \
    stampdialog.cpp \
    stampmodel.cpp \
    stampstorage.cpp \
    stamptableview.cpp

HEADERS += mainwindow.h \
    stamp.h \
    stampdelegate.h \
    stampdialog.h \
    stampmodel.h \
    stampstorage.h \
    stamptableview.h

msvc:QMAKE_CXXFLAGS += /utf-8
