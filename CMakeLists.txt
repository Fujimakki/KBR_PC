cmake_minimum_required(VERSION 3.19)
project(KBR_PC LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets SerialPort)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(ZLIB REQUIRED)

qt_standard_project_setup()

file(GLOB_RECURSE srcs
    src/*.cpp
    src/*.hpp
    src/*.h
)

qt_add_executable(KBR_PC
    WIN32 MACOSX_BUNDLE
    ${srcs}
)

target_include_directories(KBR_PC PUBLIC src/)

file(GLOB_RECURSE guis
    gui/*.ui
)

qt_add_ui(KBR_PC SOURCES ${guis})

target_link_libraries(KBR_PC
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::SerialPort
        ZLIB::ZLIB
)
target_link_libraries(KBR_PC PRIVATE Qt6::Core)

include(GNUInstallDirs)

install(TARGETS KBR_PC
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET KBR_PC
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
