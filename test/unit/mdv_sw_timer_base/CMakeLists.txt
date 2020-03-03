# madivaru-lib-v2
# Copyright (c) 2020, Tuomas Terho. All rights reserved.

add_executable(
        test_mdv_sw_timer_base
        test_mdv_sw_timer_base.cpp
        ../../mock/mock_mdv_timer_driver.cpp
)

target_include_directories(
        test_mdv_sw_timer_base
        PUBLIC
                ${PROJECT_SOURCE_DIR}/src/include
                ${PROJECT_SOURCE_DIR}/src/utils
                ${PROJECT_SOURCE_DIR}/test/mock
)

target_link_libraries(
        test_mdv_sw_timer_base
        gtest
        gmock
        gtest_main
)

gtest_discover_tests(
        test_mdv_sw_timer_base
        EXTRA_ARGS --gtest_output=xml:../../../test_results/
)

# EOF