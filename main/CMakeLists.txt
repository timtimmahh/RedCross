# Warning! This code was automatically generated for projects
# without default 'CMakeLists.txt' file.

file(GLOB_RECURSE src_list RELATIVE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

include(../includes.cmake)

SUBDIRLIST(include_list ${CMAKE_CURRENT_LIST_DIR})

idf_component_register(SRCS ${src_list} INCLUDE_DIRS ${include_list})
