cmake_minimum_required(VERSION 3.14)
project(FX2)

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)

# copy firmare file to binary
file(GLOB FIRWARE_HEX_FILES ${CMAKE_CURRENT_SOURCE_DIR}/firmwares/*.ihx)
file(COPY ${FIRWARE_HEX_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

include_directories("include")

add_executable(loader examples/loader.cpp)
target_link_libraries(loader usb-1.0 usb)

add_executable(ep6_fifo examples/ep6_fifo.cpp)
target_link_libraries(ep6_fifo usb-1.0 usb)