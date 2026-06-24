cmake_minimum_required(VERSION 3.16)

project(AudioNoderC)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(PIPEWIRE REQUIRED libpipewire-0.3)

add_executable(AudioNoderC
    main.cpp
    service.c
    service.h
    MixerFilter.c
    MixerFilter.h
    VirtualMic.c
    VirtualMic.h
    MainWindow.cpp
    MainWindow.h
    FinderLinker.c
    FinderLinker.h
    ConfigLinker.c
    ConfigLinker.h
    resources.qrc
)

target_include_directories(AudioNoderC PRIVATE ${PIPEWIRE_INCLUDE_DIRS})

target_link_libraries(AudioNoderC 
    Qt6::Widgets
    Threads::Threads
    ${PIPEWIRE_LIBRARIES}
)

