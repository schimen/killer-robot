cmake_path(GET CMAKE_SOURCE_DIR PARENT_PATH PARENT_DIR)
list(APPEND ZEPHYR_EXTRA_MODULES ${PARENT_DIR}/custom_drivers)

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(transmitter)

FILE(GLOB app_sources src/*.c*)
target_sources(app PRIVATE ${app_sources})

