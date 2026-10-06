cmake_minimum_required(VERSION 3.16)
project(handbook LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
add_executable(handbook
    main.cpp
    mainwindow.cpp
    stampdelegate.cpp
    stampdialog.cpp
    stampmodel.cpp
    stampstorage.cpp
    stamptableview.cpp
    mainwindow.h
    stamp.h
    stampdelegate.h
    stampdialog.h
    stampmodel.h
    stampstorage.h
    stamptableview.h
)
target_link_libraries(handbook PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
if(MSVC)
    target_compile_options(handbook PRIVATE /utf-8)
endif()
