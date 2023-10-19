# ***********************************************************************
# FILENAME :        CMakeLists.txt
#
# DESCRIPTION :
#      CMakeLists file for Zephyr Project.
#
# NOTES :
#      This application is part of the OpenSwarm Project.
# 
# COPYRIGHT:
#      Copyright Siemens AG.  All rights reserved.
#
# AUTHOR :    Fabian Graf       START DATE :    09 Oct 2023
#
# CHANGES :
#
# VERSION DATE    WHO     DETAIL
# 0       09Oct23 FG      Initial Commit
#

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(aiot_memfault)

target_sources(app PRIVATE src/main.c)

target_sources(app PRIVATE src/ntw.c)
target_sources(app PRIVATE src/ntw.h)
target_sources(app PRIVATE src/periodictimer.c)
target_sources(app PRIVATE src/periodictimer.h)
target_sources(app PRIVATE src/board.c)
target_sources(app PRIVATE src/board.h)

zephyr_include_directories(config)

file(GLOB CLIB 
    "${CMAKE_CURRENT_LIST_DIR}/src/sm_clib-REL-1.0.2.9/sm_clib/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/src/sm_clib-REL-1.0.2.9/sm_clib/*.h"
    "${CMAKE_CURRENT_LIST_DIR}/src/sm_clib-REL-1.0.2.9/sm_clib/ports/zephyr/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/src/sm_clib-REL-1.0.2.9/sm_clib/ports/zephyr/*.h")

target_sources(app PRIVATE ${CLIB})