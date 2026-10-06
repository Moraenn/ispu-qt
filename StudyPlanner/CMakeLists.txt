cmake_minimum_required(VERSION 3.16)
project(StudyPlanner LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
add_executable(StudyPlanner
    main.cpp
    plannercontroller.cpp
    plannerdialogs.cpp
    plannermodels.cpp
    plannerstorage.cpp
    plannerwindow.cpp
    summarywidget.cpp
    taskdelegate.cpp
    taskfilter.cpp
    plannercontroller.h
    plannerdialogs.h
    plannermodels.h
    plannerstorage.h
    plannerwindow.h
    qtutils.h
    summarywidget.h
    taskdelegate.h
    taskfilter.h
)
target_link_libraries(StudyPlanner PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
if(MSVC)
    target_compile_options(StudyPlanner PRIVATE /utf-8)
endif()
if(NOT TARGET studyplan_core)
    add_library(studyplan_core core/studyplan.cpp)
    target_include_directories(studyplan_core PUBLIC core)
    if(MSVC)
        target_compile_options(studyplan_core PUBLIC /utf-8)
    endif()
endif()
target_link_libraries(StudyPlanner PRIVATE studyplan_core)
