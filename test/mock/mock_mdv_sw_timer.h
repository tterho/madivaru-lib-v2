#pragma once

#include "mdv_sw_timer.h"
#include <gmock/gmock.h>

/*
 * Mock for mdv_sw_timer_t interface functions
 */
class MockMdvSwTimer {
    public:
        virtual ~MockMdvSwTimer()
        {
        }

        static void init();
        static void destroy();
        static bool hasInstance();
        static MockMdvSwTimer &instance();

        MOCK_METHOD2(mdv_sw_timer_init,
                     void(mdv_sw_timer_t *const, mdv_sw_timer_base_t *const));
        MOCK_METHOD2(mdv_sw_timer_set_invocation_limit,
                     void(mdv_sw_timer_t *const, uint32_t const));
        MOCK_METHOD1(mdv_sw_timer_start, void(mdv_sw_timer_t *const));
        MOCK_METHOD3(mdv_sw_timer_get_time,
                     uint32_t(mdv_sw_timer_t *const,
                              mdv_sw_timer_order_of_magnitude_t const,
                              uint32_t *const));

    private:
        static std::unique_ptr<MockMdvSwTimer> m_mockMdvSwTimer;
};

/* EOF */
