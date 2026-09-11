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
    main.cpp
    BaseWindow.h
    D2DWindow.h
    MainWindow.h
    WndProps.h
    ThreadLauncher.h
    DpiScale.h
    IControl.h
    MainButton.h
    MainEdit.h
    MainCheckBox.h
    MainRadioButton.h
    VariablesListView.h
    ScrollWindowBase.h
    Theme.h
    Utils.h
    shuntingYard.h
    TreeDrawer.h
    Functions.h
    IntegralSolver.h
    NoLinearSolver.h
    PlotWindow.h
    HubWindow.h
    HubWindow.cpp
    Lab1Window.h
    Lab1Window.cpp
    Lab2Window.h
    Lab2Window.cpp
    Lab3Window.h
    Lab3Window.cpp
    Lab4Window.h
    Lab4Window.cpp
    Lab5Window.h
    Lab5Window.cpp
    Lab6Window.h
    Lab6Window.cpp
    Lab8Window.h
    Lab8Window.cpp
    LabTestWindow.h
    LabTestWindow.cpp
)

add_executable(${PROJECT_NAME} WIN32 ${SOURCES})

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
