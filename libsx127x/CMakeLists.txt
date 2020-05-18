cmake_minimum_required(VERSION 3.10)

set(TARGET sx127x)
set(SRC sx1276.cxx crc-ccitt.cxx DataTransfer.cxx)

# Create a static library
add_library(${TARGET} ${SRC})
target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${TARGET} pthread gpiod)