cmake_minimum_required(VERSION 3.16)
project(Alarm LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
add_executable(Alarm
    alarmwindow.cpp
    main.cpp
    alarmwindow.h
)
target_link_libraries(Alarm PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
if(MSVC)
    target_compile_options(Alarm PRIVATE /utf-8)
endif()
