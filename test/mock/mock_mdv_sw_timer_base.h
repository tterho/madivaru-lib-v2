#pragma once

#include <gmock/gmock.h>
#include "mdv_sw_timer_base.h"

/*
 * Mock for mdv_sw_timer_base_t interface functions
 */
class MockMdvSwTimerBase {
        public:

        virtual ~MockMdvSwTimerBase() {
        }

        static void init();
        static void destroy();
        static bool hasInstance();
        static MockMdvSwTimerBase &instance();

        MOCK_METHOD4(mdv_sw_timer_base_init,
                     void(mdv_sw_timer_base_t *const, uint32_t const,
                     uint8_t const, mdv_timer_driver_t *const));
        MOCK_METHOD1(mdv_sw_timer_base_uninit,
                     void(mdv_sw_timer_base_t *const));
        MOCK_METHOD2(mdv_sw_timer_base_tick,
                     void(mdv_sw_timer_base_t *const, uint32_t const));
        MOCK_METHOD1(mdv_sw_timer_base_get_tick_count,
                     uint32_t(mdv_sw_timer_base_t *const));
        MOCK_METHOD1(mdv_sw_timer_base_get_tick_duration_us,
                     uint32_t(mdv_sw_timer_base_t *const));
        MOCK_METHOD1(mdv_sw_timer_base_get_timer_mask,
                     uint32_t(mdv_sw_timer_base_t *const));

        private:

        static std::unique_ptr<MockMdvSwTimerBase> m_mockMdvSwTimerBase;
};

/* EOF */
