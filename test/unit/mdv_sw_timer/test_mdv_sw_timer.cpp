#include <gtest/gtest.h>
#include "mdv_sw_timer.c"
#include "mock_mdv_sw_timer_base.h"

// Test value for timer tick duration
#define TEST_TICK_DURATION_US 100u
// Test mask (23-bit) for the timer counter
#define TEST_TIMER_MASK 0x007fffffu
// Test value for invocation limit
#define TEST_INVOCATION_LIMIT 10000u
// Test value for the initial tick count
#define TEST_TIMER_INITIAL_TICK_COUNT 1234u
// Test value for the timer tick count
#define TEST_TIMER_TICK_COUNT (TEST_TIMER_INITIAL_TICK_COUNT * 1000)
// Test time in ticks
#define TEST_TIME_IN_TICKS (TEST_TIMER_TICK_COUNT - TEST_TIMER_INITIAL_TICK_COUNT)
// Test value for ticking the timer
#define TEST_TIMER_TICK 10u

using namespace testing;

namespace{

class test_mdv_sw_timer : public Test
{
        protected:

        void SetUp() override {
                MockMdvSwTimerBase::init();
                memset(&m_sw_timer, 0, sizeof(mdv_sw_timer_t));
                m_time = 0;

                static_assert(TEST_TIMER_MASK > TEST_TIMER_TICK_COUNT,
                        "Timer mask must be always greater than the counter " \
                        "value");
        }

        void TearDown() override {
                MockMdvSwTimerBase::destroy();
        }

        void Init() {
                mdv_sw_timer_init(&m_sw_timer, &m_sw_timer_base);
        }

        void StartTimer() {
                Init();
                m_sw_timer.tick_duration_us = TEST_TICK_DURATION_US;
                m_sw_timer.timer_mask = TEST_TIMER_MASK;
                m_sw_timer.tick_count_startup_sample =
                        TEST_TIMER_INITIAL_TICK_COUNT;
                m_sw_timer.starvation_avereness.last_tick_count =
                        TEST_TIMER_INITIAL_TICK_COUNT;
                m_sw_timer.starvation_avereness.invocation_count = 0;
                m_sw_timer.starvation_avereness.invocation_limit =
                        TEST_INVOCATION_LIMIT;
        }

        mdv_sw_timer_t m_sw_timer;
        mdv_sw_timer_base_t m_sw_timer_base;
        uint32_t m_time;
};

TEST_F(test_mdv_sw_timer,
       init__invalid_function_parameters_cause_assertion_failure)
{
        EXPECT_DEATH(mdv_sw_timer_init(0, &m_sw_timer_base), "")
                << "If null, sw_timer must cause an assertion failure.";
        EXPECT_DEATH(mdv_sw_timer_init(&m_sw_timer, 0), "")
                << "If null, tick_duration_us must cause an assertion failure.";
}

TEST_F(test_mdv_sw_timer, init__sw_timer_initialized)
{
        // Initialize the software timer with 0xff to ensure that all
        // fields are filled properly
        memset(&m_sw_timer, 0xff, sizeof(mdv_sw_timer_t));

        EXPECT_CALL(MockMdvSwTimerBase::instance(),
                mdv_sw_timer_base_get_tick_duration_us(&m_sw_timer_base))
                .WillOnce(Return(TEST_TICK_DURATION_US));

        EXPECT_CALL(MockMdvSwTimerBase::instance(),
                mdv_sw_timer_base_get_timer_mask(&m_sw_timer_base))
                .WillOnce(Return(TEST_TIMER_MASK));

        mdv_sw_timer_init(&m_sw_timer, &m_sw_timer_base);

        EXPECT_EQ(&m_sw_timer_base, m_sw_timer.sw_timer_base)
                << "Timer base pointer must be set to the given value.";
        EXPECT_EQ(TEST_TICK_DURATION_US, m_sw_timer.tick_duration_us)
                << "Timer tick duration must be retrieved from the timer base.";
        EXPECT_EQ(TEST_TIMER_MASK, m_sw_timer.timer_mask)
                << "Timer mask must be retrieved from the timer base.";
}

TEST_F(test_mdv_sw_timer,
        set_invocation_limit__invalid_function_parameters_cause_assertion_failure)
{
        EXPECT_DEATH(mdv_sw_timer_set_invocation_limit(0, TEST_INVOCATION_LIMIT), "")
                << "If null, sw_timer must cause an assertion failure.";
}

TEST_F(test_mdv_sw_timer,
        set_invocation_limit__successful)
{
        Init();
        mdv_sw_timer_set_invocation_limit(&m_sw_timer, TEST_INVOCATION_LIMIT);

        EXPECT_EQ(TEST_INVOCATION_LIMIT,
                m_sw_timer.starvation_avereness.invocation_limit)
                << "Invocation limit must be set to the given value.";
}

TEST_F(test_mdv_sw_timer,
        start__invalid_function_parameters_cause_assertion_failure)
{
        EXPECT_DEATH(mdv_sw_timer_start(0), "")
                << "If null, sw_timer must cause an assertion failure.";
}

TEST_F(test_mdv_sw_timer, start__successful)
{
        Init();

        EXPECT_CALL(MockMdvSwTimerBase::instance(),
                mdv_sw_timer_base_get_tick_count(&m_sw_timer_base))
                .WillOnce(Return(TEST_TIMER_INITIAL_TICK_COUNT));

        mdv_sw_timer_start(&m_sw_timer);

        EXPECT_EQ(TEST_TIMER_INITIAL_TICK_COUNT,
                  m_sw_timer.tick_count_startup_sample)
                << "Tick count startup value must be got from the timer base.";
        EXPECT_EQ(TEST_TIMER_INITIAL_TICK_COUNT,
                  m_sw_timer.starvation_avereness.last_tick_count)
                << "The starvation avereness must be initialized with the " \
                   " current tick count.";
}

TEST_F(test_mdv_sw_timer,
        get_time__invalid_function_parameters_cause_assertion_failure)
{
        EXPECT_DEATH(mdv_sw_timer_get_time(0, MDV_SW_TIMER_MS, &m_time), "")
                << "If null, sw_timer must cause an assertion failure.";
        EXPECT_DEATH(mdv_sw_timer_get_time(&m_sw_timer, MDV_SW_TIMER_MS, 0), "")
                << "If null, time must cause an assertion failure.";
}

TEST_F(test_mdv_sw_timer, get_time__successful)
{
        uint32_t test_time_us;

        StartTimer();

        EXPECT_CALL(MockMdvSwTimerBase::instance(),
                mdv_sw_timer_base_get_tick_count(&m_sw_timer_base))
                .WillRepeatedly(Return(TEST_TIMER_TICK_COUNT));

        // Calculate test time in microseconds
        test_time_us = (TEST_TICK_DURATION_US * TEST_TIME_IN_TICKS);

        // Get the time in timer ticks
        mdv_sw_timer_get_time(&m_sw_timer, MDV_SW_TIMER_TIMERTICK, &m_time);

        EXPECT_EQ(TEST_TIME_IN_TICKS, m_time)
                << "Time must be returned correctly.";

        // Get the time in microseconds
        mdv_sw_timer_get_time(&m_sw_timer, MDV_SW_TIMER_US, &m_time);

        EXPECT_EQ(test_time_us, m_time)
                << "Time must be returned correctly.";

        // Get the time in milliseconds
        mdv_sw_timer_get_time(&m_sw_timer, MDV_SW_TIMER_MS, &m_time);

        EXPECT_EQ(test_time_us / US_IN_ONE_MS, m_time)
                << "Time must be returned correctly.";

        // Get the time in seconds
        mdv_sw_timer_get_time(&m_sw_timer, MDV_SW_TIMER_S, &m_time);

        EXPECT_EQ(test_time_us / US_IN_ONE_SECOND, m_time)
                << "Time must be returned correctly.";
}


TEST_F(test_mdv_sw_timer, get_time__manage_tick_counter_wrap_around)
{
        uint32_t test_time_us;

        StartTimer();
        m_sw_timer.tick_count_startup_sample = TEST_TIMER_MASK -
                                               TEST_TIMER_INITIAL_TICK_COUNT;

        EXPECT_CALL(MockMdvSwTimerBase::instance(),
                mdv_sw_timer_base_get_tick_count(&m_sw_timer_base))
                .WillOnce(Return(TEST_TIMER_INITIAL_TICK_COUNT));

        // Get the time in timer ticks
        mdv_sw_timer_get_time(&m_sw_timer, MDV_SW_TIMER_TIMERTICK, &m_time);

        EXPECT_EQ((TEST_TIMER_INITIAL_TICK_COUNT * 2) + 1, m_time)
                << "Time must be returned correctly.";
}

TEST_F(test_mdv_sw_timer, get_time__timer_is_starving_when_timer_not_counting)
{
        uint32_t test_time_us;

        StartTimer();

        // Setup almost-starving situation
        m_sw_timer.starvation_avereness.invocation_count =
                TEST_INVOCATION_LIMIT - 1;

        EXPECT_CALL(MockMdvSwTimerBase::instance(),
                mdv_sw_timer_base_get_tick_count(&m_sw_timer_base))
                .WillRepeatedly(Return(TEST_TIMER_INITIAL_TICK_COUNT));

        // Get the time in timer ticks
        EXPECT_NO_FATAL_FAILURE(mdv_sw_timer_get_time(&m_sw_timer,
                MDV_SW_TIMER_TIMERTICK, &m_time));

        // Timer not ticked, but this is the first time. Assume that the time is
        // zero, but the invocation counter is resetted.
        EXPECT_EQ(0, m_time)
                << "Time must be zero when timer is not ticked.";
        EXPECT_EQ(0, m_sw_timer.starvation_avereness.invocation_count)
                << "Invocation counter must be reset.";

        // Setup almost-starving situation again
        m_sw_timer.starvation_avereness.invocation_count =
                TEST_INVOCATION_LIMIT - 1;

        // Get the time in timer ticks, now increasing the invocation counter
        // to the limit
        EXPECT_DEATH(mdv_sw_timer_get_time(&m_sw_timer,
                MDV_SW_TIMER_TIMERTICK, &m_time), "")
                << "If starving, the timer must cause an assertion failure.";
}

} // namespace