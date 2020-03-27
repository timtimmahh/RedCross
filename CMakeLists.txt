cmake_minimum_required(VERSION 3.16.0)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

list(APPEND EXTRA_COMPONENT_DIRS)
project(redcross)

include(includes.cmake)

SUBDIRLIST(include_list ${CMAKE_CURRENT_LIST_DIR}/main)

FOREACH (dir ${include_list})
    message(Include: ${dir})
    include_directories(${dir})
ENDFOREACH ()
