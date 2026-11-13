cmake_minimum_required(VERSION 3.19)
project(CustomActivity LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network)

qt_standard_project_setup()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/app.rc")

qt_add_executable(CustomActivity
    WIN32 MACOSX_BUNDLE
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    assets/assets.qrc
    src/platform/windows/TrayManager.h src/platform/windows/TrayManager.cpp
    src/rpc/Activity.h src/rpc/Activity.cpp
    src/rpc/RPCProtocol.h src/rpc/RPCProtocol.cpp
    src/rpc/IPCClient.h src/rpc/IPCClient.cpp
    src/rpc/DiscordRPC.h src/rpc/DiscordRPC.cpp
    ${app_icon_resource_windows}
)

target_link_libraries(CustomActivity
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::Network
)

install(TARGETS CustomActivity
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET CustomActivity
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
