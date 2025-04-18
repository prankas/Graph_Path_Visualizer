cmake_minimum_required(VERSION 3.5)
project(GraphPathVisualizer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_executable(${PROJECT_NAME}
    main.cpp

    MainWindow.h
    MainWindow.cpp

    Graph.h
    Graph.cpp

    GraphWidget.h
    GraphWidget.cpp
)

target_link_libraries(${PROJECT_NAME}
    Qt5::Widgets
)
