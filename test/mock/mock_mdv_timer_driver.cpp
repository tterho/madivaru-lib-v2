#include "mock_mdv_timer_driver.h"

std::unique_ptr<MockMdvTimerDriver> MockMdvTimerDriver::m_mockMdvTimerDriver;

void MockMdvTimerDriver::init()
{
        m_mockMdvTimerDriver.reset(new testing::NiceMock<MockMdvTimerDriver>());
}

void MockMdvTimerDriver::destroy()
{
        m_mockMdvTimerDriver.reset();
}

MockMdvTimerDriver &MockMdvTimerDriver::instance()
{
        if (!hasInstance()) {
                printf("MockMdvTimerDriver::init() not called!\r\n");
                abort();
        }

        return *m_mockMdvTimerDriver;
}

bool MockMdvTimerDriver::hasInstance()
{
        return (bool)m_mockMdvTimerDriver;
}

mdv_timer_driver_t *MockMdvTimerDriver::GetMdvTimerDriver()
{
        return &MockMdvTimerDriver::m_mdvTimerDriver;
}

extern "C" {

mdv_result_t mdv_timer_driver_init(
        mdv_timer_event_handler_t const event_handler, void *const user_data)
{
        return MockMdvTimerDriver::instance().mdv_timer_driver_init(
                event_handler, user_data);
}

mdv_result_t mdv_timer_driver_uninit(void)
{
        return MockMdvTimerDriver::instance().mdv_timer_driver_uninit();
}

mdv_result_t mdv_timer_driver_start(void)
{
        return MockMdvTimerDriver::instance().mdv_timer_driver_start();
}

mdv_result_t mdv_timer_driver_stop(void)
{
        return MockMdvTimerDriver::instance().mdv_timer_driver_stop();
}

mdv_result_t mdv_timer_driver_reset(void)
{
        return MockMdvTimerDriver::instance().mdv_timer_driver_reset();
}

uint32_t mdv_timer_driver_get_count(void)
{
        return MockMdvTimerDriver::instance().mdv_timer_driver_get_count();
}

bool mdv_timer_driver_is_running(void)
{
        return MockMdvTimerDriver::instance().mdv_timer_driver_is_running();
}

} // extern "C"

/*
 * Mocked timer driver interface
 */
mdv_timer_driver_t MockMdvTimerDriver::m_mdvTimerDriver = {
        ::mdv_timer_driver_init, ::mdv_timer_driver_uninit,
        ::mdv_timer_driver_start, ::mdv_timer_driver_stop,
        ::mdv_timer_driver_reset, ::mdv_timer_driver_get_count,
        ::mdv_timer_driver_is_running
};
