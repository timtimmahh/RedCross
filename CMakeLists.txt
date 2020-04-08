cmake_minimum_required(VERSION 3.16.0)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

#set(CONFIG_COMPILER_CXX_RTTI y)
list(APPEND EXTRA_COMPONENT_DIRS)
project(redcross)

include(includes.cmake)

SUBDIRLIST(include_list ${CMAKE_CURRENT_LIST_DIR}/main)

FOREACH (dir ${include_list})
    include_directories(${dir})
ENDFOREACH ()
#
#if(NOT CONFIG_COMPILER_CXX_RTTI)
#    set()
#endif()
#
#message("RTTI Enabled? ${CONFIG_COMPILER_CXX_RTTI}")

#file(GLOB_RECURSE DUMMY_SRCS RELATIVE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/main/*.cpp ${CMAKE_CURRENT_LIST_DIR}/main/*.hpp ${CMAKE_CURRENT_LIST_DIR}/main/*.h)

#add_custom_target(
#        idf_build ALL
#        COMMAND idf.py build
#        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#)

#add_custom_target(
#        idf_flash ALL
#        COMMAND ESPBAUD=921600 ESPPORT=/dev/ttyUSB0 ninja -C build flash
#        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#)
##
#add_custom_target(
#        idf_monitor ALL
#        COMMAND ESPBAUD=115200 ESPPORT=/dev/ttyUSB0 ninja -C build monitor
#        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#)
#
#add_executable(Z_DUMMY_TARGET ${DUMMY_SRCS})