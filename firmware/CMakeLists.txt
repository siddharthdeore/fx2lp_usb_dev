cmake_minimum_required(VERSION 3.14)

# the name of the target operating system
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR 8051)

set(CMAKE_C_FLAGS " -mmcs51 -model-large --std-c99")
# set(CMAKE_C_FLAGS " -mmcs51 --code-size 0x1c00 --xram-size 0x0200 --xram-loc 0x1c00 --std-c99")
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
# here is the target environment is located

find_program(SDCC_EXECUTABLE sdcc)
if(NOT SDCC_EXECUTABLE)
    message(FATAL_ERROR "sdcc not found")
endif()

# which compilers to use for C and C++
set(CMAKE_C_COMPILER ${SDCC_EXECUTABLE})


project(FX2FW LANGUAGES C)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

include_directories("fx2lib/include")
link_directories("fx2lib/lib")

add_executable(fifo_ep6 examples/fifo_ep6.c)
target_link_libraries(fifo_ep6 fx2.lib)

add_executable(led examples/led.c)
target_link_libraries(led fx2.lib)

add_executable(timer0 examples/timer0.c)
target_link_libraries(timer0 fx2.lib)





file(GLOB FIRWARE_HEX_FILES ${CMAKE_CURRENT_SOURCE_DIR}/build/*.ihx)
file(COPY ${FIRWARE_HEX_FILES} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../build)