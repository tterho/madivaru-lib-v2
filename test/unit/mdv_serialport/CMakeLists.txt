# madivaru-lib-v2
# Copyright (c) 2020, Tuomas Terho. All rights reserved.

add_executable(
        test_mdv_serialport
        test_mdv_serialport.cpp
        ../../mock/mock_mdv_serialport_driver.cpp
        ../../mock/mock_mdv_serialport_callbacks.cpp
        ../../mock/mock_mdv_sw_timer.cpp
)

target_include_directories(
        test_mdv_serialport
        PUBLIC
                ${PROJECT_SOURCE_DIR}/src/include
                ${PROJECT_SOURCE_DIR}/src/utils
                ${PROJECT_SOURCE_DIR}/src/serialport
                ${PROJECT_SOURCE_DIR}/test/mock
)

target_link_libraries(
        test_mdv_serialport
        gtest
        gmock
        gtest_main
)

gtest_discover_tests(
        test_mdv_serialport
        EXTRA_ARGS --gtest_output=xml:../../../test_results/
)

# EOF