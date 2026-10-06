cmake_minimum_required(VERSION 3.16)
project(handbook_lab3 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
add_executable(handbook_lab3
    main.cpp
    mainwindow.cpp
    stampdelegate.cpp
    stampdialog.cpp
    stampmodel.cpp
    stamptableview.cpp
    mainwindow.h
    stamp.h
    stampdelegate.h
    stampdialog.h
    stampmodel.h
    stamptableview.h
)
target_link_libraries(handbook_lab3 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
if(MSVC)
    target_compile_options(handbook_lab3 PRIVATE /utf-8)
endif()
