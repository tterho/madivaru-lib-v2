#include "mock_mdv_sw_timer_base.h"

std::unique_ptr<MockMdvSwTimerBase> MockMdvSwTimerBase::m_mockMdvSwTimerBase;

void MockMdvSwTimerBase::init()
{
        m_mockMdvSwTimerBase.reset(new testing::NiceMock<MockMdvSwTimerBase>());
}

void MockMdvSwTimerBase::destroy()
{
        m_mockMdvSwTimerBase.reset();
}

MockMdvSwTimerBase &MockMdvSwTimerBase::instance()
{
        if (!hasInstance()) {
                printf("MockMdvSwTimerBase::init() not called!\r\n");
                abort();
        }

        return *m_mockMdvSwTimerBase;
}

bool MockMdvSwTimerBase::hasInstance()
{
        return (bool)m_mockMdvSwTimerBase;
}

extern "C" {

void mdv_sw_timer_base_init(mdv_sw_timer_base_t *const sw_timer_base,
                            uint32_t const tick_duration_us,
                            uint8_t const timer_width_bits,
                            mdv_timer_driver_t *const timer_driver)
{
        return MockMdvSwTimerBase::instance().
                mdv_sw_timer_base_init(sw_timer_base, tick_duration_us,
                                       timer_width_bits, timer_driver);
}

void mdv_sw_timer_base_uninit(mdv_sw_timer_base_t *const sw_timer_base)
{
        return MockMdvSwTimerBase::instance().
                mdv_sw_timer_base_uninit(sw_timer_base);
}

void mdv_sw_timer_base_tick(mdv_sw_timer_base_t *const sw_timer_base,
                            uint32_t const tick_count)
{
        return MockMdvSwTimerBase::instance().
                mdv_sw_timer_base_tick(sw_timer_base, tick_count);
}

uint32_t mdv_sw_timer_base_get_tick_count(
        mdv_sw_timer_base_t *const sw_timer_base)
{
        return MockMdvSwTimerBase::instance().
                mdv_sw_timer_base_get_tick_count(sw_timer_base);
}

uint32_t mdv_sw_timer_base_get_tick_duration_us(
        mdv_sw_timer_base_t *const sw_timer_base)
{
        return MockMdvSwTimerBase::instance().
                mdv_sw_timer_base_get_tick_duration_us(sw_timer_base);
}

uint32_t mdv_sw_timer_base_get_timer_mask(
        mdv_sw_timer_base_t *const sw_timer_base)
{
        return MockMdvSwTimerBase::instance().
                mdv_sw_timer_base_get_timer_mask(sw_timer_base);
}

} // extern "C"
