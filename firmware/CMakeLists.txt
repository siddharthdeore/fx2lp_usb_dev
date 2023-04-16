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





# TODO add desc.a51
# cmake_minimum_required(VERSION 3.5)
# project(firmware)

# set(FX2LIBDIR ../../../3rd/fx2lib)
# set(LIBS -L${FX2LIBDIR}/lib fx2.lib)
# set(INCS -I sdcc -I${FX2LIBDIR}/include)

# file(GLOB_RECURSE CC_SRCS "*.c")
# file(GLOB_RECURSE AS_SRCS "*.a51")
# set(CC_OBJS)
# set(AS_OBJS)

# foreach(CC_SRC ${CC_SRCS})
#     get_filename_component(CC_OBJ ${CC_SRC} NAME_WE)
#     set(CC_OBJ ${CC_OBJ}.rel)
#     add_custom_command(OUTPUT ${CC_OBJ}
#                        COMMAND sdcc ${CCFLAGS} -c --disable-warning 85 ${INCS} ${CC_SRC}
#                        DEPENDS ${CC_SRC})
#     list(APPEND CC_OBJS ${CC_OBJ})
# endforeach()

# foreach(AS_SRC ${AS_SRCS})
#     get_filename_component(AS_OBJ ${AS_SRC} NAME_WE)
#     set(AS_OBJ ${AS_OBJ}.rel)
#     add_custom_command(OUTPUT ${AS_OBJ}
#                        COMMAND asx8051 -logs ${AS_SRC}
#                        DEPENDS ${AS_SRC})
#     list(APPEND AS_OBJS ${AS_OBJ})
# endforeach()

# add_custom_target(${PROJECT_NAME}.rel DEPENDS ${CC_OBJS} ${AS_OBJS})

# add_executable(${PROJECT_NAME}.hex ${CC_OBJS} ${AS_OBJS})
# set_target_properties(${PROJECT_NAME}.hex PROPERTIES OUTPUT_NAME ${PROJECT_NAME}.hex)

# target_link_libraries(${PROJECT_NAME}.hex ${LIBS})

# add_custom_command(TARGET ${PROJECT_NAME}.hex POST_BUILD
#                    COMMAND sdcc ${CCFLAGS} -o ${PROJECT_NAME}.hex ${CC_OBJS} ${AS_OBJS} ${LIBS}
#                    COMMENT "Generating ${PROJECT_NAME}.hex")
                   
# add_custom_command(OUTPUT firmware.c
#                    COMMAND cp ${FX2LIBDIR}/fw/fw.c firmware.c
#                    DEPENDS ${FX2LIBDIR}/fw/fw.c)