# madivaru-lib-v2
# Copyright (c) 2020, Tuomas Terho. All rights reserved.

cmake_minimum_required(VERSION 3.10)
project(madivaru-lib-v2 C CXX)
include(GoogleTest)
add_subdirectory(googletest)
link_directories(${googletest_BINARY_DIR})
enable_testing()

# EOF