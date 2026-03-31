file(GLOB srcs
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

qt_add_library(serial STATIC ${srcs})

target_link_libraries(serial
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        ZLIB::ZLIB
)
target_link_libraries(serial
    PUBLIC
        Qt6::SerialPort
)

target_include_directories(serial INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
