#pragma once

#include <gmock/gmock.h>
#include "mdv_timer_driver.h"

/*
 * Mock for mdv_timer_driver_t interface functions
 */
class MockMdvTimerDriver {
        public:

        virtual ~MockMdvTimerDriver() {
        }

        static void init();
        static void destroy();
        static bool hasInstance();
        static MockMdvTimerDriver &instance();

        static mdv_timer_driver_t *GetMdvTimerDriver();

        MOCK_METHOD2(mdv_timer_driver_init,
                mdv_result_t(mdv_timer_event_handler_t const, void *const));
        MOCK_METHOD0(mdv_timer_driver_uninit, mdv_result_t(void));
        MOCK_METHOD0(mdv_timer_driver_start, mdv_result_t(void));
        MOCK_METHOD0(mdv_timer_driver_stop, mdv_result_t(void));
        MOCK_METHOD0(mdv_timer_driver_reset, mdv_result_t(void));
        MOCK_METHOD0(mdv_timer_driver_get_count, uint32_t(void));
        MOCK_METHOD0(mdv_timer_driver_is_running, bool(void));

        private:

        static std::unique_ptr<MockMdvTimerDriver> m_mockMdvTimerDriver;
        static mdv_timer_driver_t m_mdvTimerDriver;
};

/* EOF */
