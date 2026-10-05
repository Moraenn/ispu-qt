QT += widgets

CONFIG += c++17

TEMPLATE = app
TARGET = handbook

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    stampdelegate.cpp \
    stampdialog.cpp \
    stampmodel.cpp \
    stamptableview.cpp

HEADERS += \
    mainwindow.h \
    stamp.h \
    stampdelegate.h \
    stampdialog.h \
    stampmodel.h \
    stamptableview.h
