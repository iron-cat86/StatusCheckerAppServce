cmake_minimum_required(VERSION 3.16)

project(StatusCheckerApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

add_executable(StatusCheckerApp 
    main.cpp 
    mainwindow.h 
    mainwindow.cpp
)

target_link_libraries(StatusCheckerApp PRIVATE 
    Qt5::Widgets 
    Qt5::Network
)

# Определяем файлы для сервиса
add_executable(StatusService
    service_main.cpp
    simpleserver.h
    simpleserver.cpp
)

# Привязываем библиотеки Qt к нашему сервису
target_link_libraries(StatusService PRIVATE
    Qt5::Core
    Qt5::Network
)
