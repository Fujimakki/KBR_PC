file(GLOB srcs
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/series.h
)

qt_add_library(widgets STATIC ${srcs})

target_link_libraries(widgets
    PRIVATE
        Qt::Core
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
)

target_include_directories(widgets INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
