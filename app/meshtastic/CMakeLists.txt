
# SPDX-License-Identifier: Apache-2.0
#set_property(GLOBAL PROPERTY GLOBAL_DEPENDS_DEBUG_MODE 1)
set(CMAKE_VERBOSE_MAKEFILE on)

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(lvgl)


FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})
