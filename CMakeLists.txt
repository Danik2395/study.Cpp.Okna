cmake_minimum_required(VERSION 3.16)
project(Okna VERSION 1.0 LANGUAGES CXX)

add_compile_definitions(UNICODE _UNICODE)

include(FetchContent)

FetchContent_Declare(
    ssstl
    GIT_REPOSITORY https://github.com/Danik2395/ssstl.git
    GIT_TAG        main
)

FetchContent_MakeAvailable(ssstl)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    main/main.cpp
    main/Theme.h

    Windows/Templates/BaseWindow.h
    Windows/Templates/D2DWindow.h
    Windows/Templates/MainWindow.h
    Windows/Templates/ScrollWindowBase.h
    Windows/Helpers/WndProps.h
    Windows/Helpers/ThreadLauncher.h
    Windows/Helpers/DpiScale.h
    Windows/Helpers/IControl.h
    Windows/Child/MainButton.h
    Windows/Child/MainEdit.h
    Windows/Child/MainCheckBox.h
    Windows/Child/MainRadioButton.h
    Windows/Child/VariablesListView.h
    Windows/Child/PlotWindow.h
    Windows/Child/TreeDrawer.h

    Windows/Actual/HubWindow.h
    Windows/Actual/HubWindow.cpp
    Windows/Actual/Lab1Window.h
    Windows/Actual/Lab1Window.cpp
    Windows/Actual/Lab2Window.h
    Windows/Actual/Lab2Window.cpp
    Windows/Actual/Lab3Window.h
    Windows/Actual/Lab3Window.cpp
    Windows/Actual/Lab4Window.h
    Windows/Actual/Lab4Window.cpp
    Windows/Actual/Lab5Window.h
    Windows/Actual/Lab5Window.cpp
    Windows/Actual/Lab6Window.h
    Windows/Actual/Lab6Window.cpp
    Windows/Actual/Lab8Window.h
    Windows/Actual/Lab8Window.cpp
    Windows/Actual/LabTestWindow.h
    Windows/Actual/LabTestWindow.cpp

    Utils/Utils.h
    Utils/Functions.h

    Algorithms_and_containers/shuntingYard.h
    Algorithms_and_containers/IntegralSolver.h
    Algorithms_and_containers/NoLinearSolver.h
)

add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Windows/Actual"
    "${CMAKE_CURRENT_SOURCE_DIR}/Windows/Child"
    "${CMAKE_CURRENT_SOURCE_DIR}/Windows/Helpers"
    "${CMAKE_CURRENT_SOURCE_DIR}/Windows/Templates"
    "${CMAKE_CURRENT_SOURCE_DIR}/Algorithms_and_containers"
    "${CMAKE_CURRENT_SOURCE_DIR}/main"
    "${CMAKE_CURRENT_SOURCE_DIR}/Utils"
)

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DPI_AWARE "PerMonitor")

target_link_libraries(${PROJECT_NAME} PRIVATE
    kernel32
    user32
    gdi32
    d2d1
    dwrite
    dxgi
    comctl32
    ssstl
)
