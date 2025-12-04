cmake_minimum_required(VERSION 3.19)
project(tuning_app LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Gui Charts Network WebSockets Quick QuickWidgets Qml)

qt_standard_project_setup()

qt_add_executable(tuning_app
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    sdk/rom_time.hpp
    design/rom_structures.h
    communication/ros_bridge_client.hpp
    communication/ros_bridge_client.cpp
    design/rom_design.hpp
    design/readmeviewer.h
    design/readmeviewer.cpp
    design/covarianceDisplay.hpp
    design/covarianceDisplay.cpp
    sdk/rom_map_widget.hpp
    sdk/rom_time.cpp
    sdk/rom_map_widget.cpp
    sdk/carto_rom_map_widget.hpp
    sdk/carto_rom_map_widget.cpp





    
)

target_link_libraries(tuning_app PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::WebSockets
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::QuickWidgets
    Qt${QT_VERSION_MAJOR}::Qml
    /usr/include/eigen3
)

include(GNUInstallDirs)
include_directories(/usr/include/eigen3)

qt_add_resources(tuning_app "resources"
    PREFIX "/"
    FILES
        Speed.qml
        RightSpeed.qml
        LeftSpeed.qml
        ActualSpeed.qml
        ActualRightSpeed.qml
        ActualLeftSpeed.qml
)



install(TARGETS tuning_app
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET tuning_app
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})


