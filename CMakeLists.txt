cmake_minimum_required(VERSION 3.16)

project(StatusCheckerApp VERSION 1.0)

find_package(Qt5 COMPONENTS Widgets Core Gui Network
    REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

qt5_add_resources(StatusCheckerApp_RESOURCES app.qrc)

add_executable(StatusCheckerApp 
    main.cpp 
    mainwindow.h 
    mainwindow.cpp
    qrc_app.cpp
    ${StatusCheckerApp_RESOURCES}
)

target_link_libraries(StatusCheckerApp PRIVATE 
    Qt5::Widgets 
    Qt5::Network
    Qt5::Core
    Qt5::Gui
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
