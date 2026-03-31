cmake_minimum_required(VERSION 3.19)
project(KBR_PC LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets SerialPort Gui)
find_package(ZLIB REQUIRED)

qt_standard_project_setup()

add_subdirectory(src/app)
add_subdirectory(src/serial)
add_subdirectory(src/widgets)

#add_compile_definitions(FPS_LOCK)
