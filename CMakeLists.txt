

cmake_minimum_required(VERSION 3.14)
project(FX2APPS LANGUAGES CXX)

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_CXX_STANDARD 14)

include_directories("include")

add_executable(loader examples/loader.cpp)
target_link_libraries(loader usb-1.0 usb)


add_executable(FIFO_EP6 examples/fifo_ep6.cpp)
target_link_libraries(FIFO_EP6 usb-1.0 usb)

