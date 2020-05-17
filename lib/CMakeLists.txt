cmake_minimum_required(VERSION 3.10)

set(TARGET sx127xtools)
set(SRC data_tx.pb.cc sx1276.cxx crc-ccitt.cxx DataTransfer.cxx ImageGrabber.cxx)

find_package(PkgConfig)

pkg_check_modules(GST REQUIRED gstreamer-1.0>=1.4
                               gstreamer-app-1.0>=1.4)

                               # gstreamer-sdp-1.0>=1.4
                               # gstreamer-video-1.0>=1.4

# Create a static library
add_library(${TARGET} ${SRC})
target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${GST_INCLUDE_DIRS})
target_link_libraries(${TARGET} pthread gpiod protobuf ${GST_LIBRARIES})