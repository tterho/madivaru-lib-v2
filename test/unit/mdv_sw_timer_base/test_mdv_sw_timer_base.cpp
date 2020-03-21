#include <gtest/gtest.h>
#include "mdv_sw_timer_base.c"
#include "mock_mdv_timer_driver.h"

// Test value for timer tick duration
#define TEST_TICK_DURATION_US 100u
// Test value for timer width in bits
#define TEST_TIMER_WIDTH_BITS 23u
// Test mask (23-bit) for the timer counter
#define TEST_TIMER_MASK 0x007fffffu
// Test value for invocation limit
#define TEST_TIMER_SYSTEM_ILIM 10000
// Test value for ticking the timer
#define TEST_TIMER_TICK 10

using namespace testing;

namespace{

class test_mdv_sw_timer_base : public Test
{
        protected:

        void SetUp() override {
                MockMdvTimerDriver::init();
                m_timer_driver = MockMdvTimerDriver::GetMdvTimerDriver();
                memset(&m_sw_timer_base, 0, sizeof(mdv_sw_timer_base_t));
        }

        void TearDown() override {
                MockMdvTimerDriver::destroy();
        }

        void Init() {
                mdv_sw_timer_base_init(&m_sw_timer_base, TEST_TICK_DURATION_US,
                                       TEST_TIMER_WIDTH_BITS, m_timer_driver);
        }

        mdv_sw_timer_base_t m_sw_timer_base;
        mdv_timer_driver_t *m_timer_driver;
};

TEST_F(test_mdv_sw_timer_base,
       init__invalid_function_parameters_cause_assertion_failure)
{
        EXPECT_DEATH(mdv_sw_timer_base_init(0, TEST_TICK_DURATION_US,
                                            TEST_TIMER_WIDTH_BITS,
                                            m_timer_driver), "")
                << "If null, sw_timer_base must cause an assertion failure.";
        EXPECT_DEATH(mdv_sw_timer_base_init(&m_sw_timer_base, 0,
                                            TEST_TIMER_WIDTH_BITS,
                                            m_timer_driver), "")
                << "If null, tick_duration_us must cause an assertion failure.";
        EXPECT_DEATH(mdv_sw_timer_base_init(&m_sw_timer_base,
                                            TEST_TICK_DURATION_US, 0,
                                            m_timer_driver), "")
                << "If null, timer_width parameter must cause an assertion " \
                   "failure.";
        EXPECT_DEATH(mdv_sw_timer_base_init(&m_sw_timer_base,
                                            TEST_TICK_DURATION_US, 33,
                                            m_timer_driver), "")
                << "If greater than 32, timer_width parameter must cause an " \
                   "assertion failure.";
}

TEST_F(test_mdv_sw_timer_base, init__sw_timer_base_initialized)
{
        // Initialize the software timer base with 0xff to ensure that all
        // fields are filled properly
        memset(&m_sw_timer_base, 0xff, sizeof(mdv_sw_timer_base_t));

        EXPECT_CALL(MockMdvTimerDriver::instance(), mdv_timer_driver_init(0, 0))
                .WillOnce(Return(MDV_RESULT_OK));

        mdv_sw_timer_base_init(&m_sw_timer_base, TEST_TICK_DURATION_US,
                               TEST_TIMER_WIDTH_BITS, m_timer_driver);

        EXPECT_EQ(TEST_TIMER_MASK, m_sw_timer_base.timer_mask)
                << "Valid test mask must be created for the timer counter.";
        EXPECT_EQ(0, m_sw_timer_base.tick_counter)
                << "Timer tick counter must be resetted to zero.";
        EXPECT_EQ(TEST_TICK_DURATION_US, m_sw_timer_base.tick_duration_us)
                << "Timer tick duration in us must be initialized with the " \
                   "given value.";
        EXPECT_EQ(m_timer_driver, m_sw_timer_base.timer_driver)
                << "The pointer to the timer driver must be set to the given " \
                   "value.";
}

TEST_F(test_mdv_sw_timer_base,
       init__timer_driver_init_not_called_if_timer_driver_not_provided)
{
        EXPECT_CALL(MockMdvTimerDriver::instance(), mdv_timer_driver_init(0, 0))
                .Times(0);

        // This will lead into segmentation fault, if the null value of the
        // timer_driver pointer is not handled correctly
        mdv_sw_timer_base_init(&m_sw_timer_base, TEST_TICK_DURATION_US,
                               TEST_TIMER_WIDTH_BITS, 0);
}

TEST_F(test_mdv_sw_timer_base,
       uninit__invalid_function_parameters_cause_assertion_failure)
{
        EXPECT_DEATH(mdv_sw_timer_base_uninit(0), "")
                << "If null, sw_timer_base must cause an assertion failure.";
}

TEST_F(test_mdv_sw_timer_base,
       uninit__timer_driver_init_not_called_if_sw_timer_base_not_initialized)
{
        EXPECT_CALL(MockMdvTimerDriver::instance(), mdv_timer_driver_uninit())
                .Times(0);

        // This will cause a segmentation fault if the null value of the timer
        // driver pointer is not handled properly
        mdv_sw_timer_base_uninit(&m_sw_timer_base);
}

TEST_F(test_mdv_sw_timer_base, uninit__sw_timer_base_uninitialized)
{
        Init();

        EXPECT_CALL(MockMdvTimerDriver::instance(),
                    mdv_timer_driver_uninit())
                .WillOnce(Return(MDV_RESULT_OK));

        mdv_sw_timer_base_uninit(&m_sw_timer_base);
}

TEST_F(test_mdv_sw_timer_base,
       tick__invalid_function_parameters_cause_assertion_failure)
{
        Init();

        EXPECT_DEATH(mdv_sw_timer_base_tick(0, TEST_TIMER_TICK), "")
                << "If null, sw_timer_base must cause an assertion failure.";
        EXPECT_DEATH(mdv_sw_timer_base_tick(&m_sw_timer_base, 0), "")
                << "If zero, tick must cause an assertion failure.";
}

TEST_F(test_mdv_sw_timer_base,
       tick__tick_counter_advanced_and_wrapped_around_successfully)
{
        Init();

        mdv_sw_timer_base_tick(&m_sw_timer_base, TEST_TIMER_TICK);

        EXPECT_EQ(TEST_TIMER_TICK, m_sw_timer_base.tick_counter)
                << "Tick counter must be advanced by the test value.";

        // Set the tick counter to the mask value to demonstrate the wrap
        // around.
        m_sw_timer_base.tick_counter = TEST_TIMER_MASK;

        mdv_sw_timer_base_tick(&m_sw_timer_base, 1);

        EXPECT_EQ(0, m_sw_timer_base.tick_counter)
                << "Tick counter must wrap around when adding 1 to the " \
                   "maximum value.";
}

TEST_F(test_mdv_sw_timer_base,
       get_tick_count__invalid_function_parameters_cause_assertion_failure)
{
        Init();

        EXPECT_DEATH(mdv_sw_timer_base_get_tick_count(0), "")
                << "If null, sw_timer_base must cause an assertion failure.";
}

TEST_F(test_mdv_sw_timer_base,
       get_tick_count__tick_count_returned_successfully)
{
        uint32_t tick_count;

        Init();

        EXPECT_CALL(MockMdvTimerDriver::instance(), mdv_timer_driver_get_count())
                .WillOnce(Return(TEST_TIMER_TICK));

        tick_count = mdv_sw_timer_base_get_tick_count(&m_sw_timer_base);

        EXPECT_EQ(TEST_TIMER_TICK, tick_count)
                << "The tick count must be read and returned from the timer " \
                   "driver, when the driver is present.";

        // Set the local tick count to a known value
        m_sw_timer_base.tick_counter = TEST_TIMER_TICK;

        // Remove the timer driver association
        m_sw_timer_base.timer_driver = 0;

        // Get the tick count
        tick_count = mdv_sw_timer_base_get_tick_count(&m_sw_timer_base);

        EXPECT_EQ(TEST_TIMER_TICK, tick_count)
                << "The tick count must be read and returned from the local " \
                    "counter variable, when the timer driver is not present.";
}

TEST_F(test_mdv_sw_timer_base,
       get_tick_duration_us__invalid_function_parameters_cause_assertion_failure)
{
        Init();

        EXPECT_DEATH(mdv_sw_timer_base_get_tick_duration_us(0), "")
                << "If null, sw_timer_base must cause an assertion failure.";
}

TEST_F(test_mdv_sw_timer_base,
       get_tick_duration_us__value_returned_successfully)
{
        uint32_t tick_duration_us;

        Init();

        tick_duration_us = mdv_sw_timer_base_get_tick_duration_us(&m_sw_timer_base);

        EXPECT_EQ(TEST_TICK_DURATION_US, tick_duration_us)
                << "The tick duration must be returned.";
}

TEST_F(test_mdv_sw_timer_base,
       get_timer_mask__invalid_function_parameters_cause_assertion_failure)
{
        Init();

        EXPECT_DEATH(mdv_sw_timer_base_get_timer_mask(0), "")
                << "If null, sw_timer_base must cause an assertion failure.";
}

TEST_F(test_mdv_sw_timer_base,
       get_timer_mask__value_returned_successfully)
{
        uint32_t timer_mask;

        Init();

        timer_mask = mdv_sw_timer_base_get_timer_mask(&m_sw_timer_base);

        EXPECT_EQ(TEST_TIMER_MASK, timer_mask)
                << "The timer mask must be returned.";
}

} // namespace