#include "mock_mdv_sw_timer.h"

std::unique_ptr<MockMdvSwTimer> MockMdvSwTimer::m_mockMdvSwTimer;

void MockMdvSwTimer::init()
{
        m_mockMdvSwTimer.reset(new testing::NiceMock<MockMdvSwTimer>());
}

void MockMdvSwTimer::destroy()
{
        m_mockMdvSwTimer.reset();
}

MockMdvSwTimer &MockMdvSwTimer::instance()
{
        if (!hasInstance()) {
                printf("MockMdvSwTimer::init() not called!\r\n");
                abort();
        }

        return *m_mockMdvSwTimer;
}

bool MockMdvSwTimer::hasInstance()
{
        return (bool)m_mockMdvSwTimer;
}

extern "C" {

void mdv_sw_timer_init(mdv_sw_timer_t *const sw_timer,
                       mdv_sw_timer_base_t *const sw_timer_base)
{
        MockMdvSwTimer::instance().mdv_sw_timer_init(sw_timer, sw_timer_base);
}

void mdv_sw_timer_set_invocation_limit(mdv_sw_timer_t *const sw_timer,
                                       uint32_t const invocation_limit)
{
        MockMdvSwTimer::instance().mdv_sw_timer_set_invocation_limit(
                sw_timer, invocation_limit);
}

void mdv_sw_timer_start(mdv_sw_timer_t *const sw_timer)
{
        MockMdvSwTimer::instance().mdv_sw_timer_start(sw_timer);
}

void mdv_sw_timer_get_time(
        mdv_sw_timer_t *const sw_timer,
        mdv_sw_timer_order_of_magnitude_t const order_of_magnitude,
        uint32_t *const time)
{
        MockMdvSwTimer::instance().mdv_sw_timer_get_time(
                sw_timer, order_of_magnitude, time);
}

} // extern "C"
