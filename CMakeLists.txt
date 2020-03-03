# madivaru-lib-v2
# Copyright (c) 2020, Tuomas Terho. All rights reserved.

cmake_minimum_required(VERSION 3.10)
project(madivaru-lib-v2 C CXX)
include(GoogleTest)

add_subdirectory(test/googletest)
add_subdirectory(test/unit/mdv_sw_timer_base)

link_directories(${googletest_BINARY_DIR})

enable_testing()

# EOF