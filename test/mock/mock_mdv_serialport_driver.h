#pragma once

#include "mdv_serialport_driver.h"
#include <gmock/gmock.h>

/*
 * Mock for mdv_serialport_driver_t interface functions
 */
class MockMdvSerialportDriver {
    public:
        virtual ~MockMdvSerialportDriver()
        {
        }

        static void init();
        static void destroy();
        static bool hasInstance();
        static MockMdvSerialportDriver &instance();

        static mdv_serialport_driver_t *GetMdvSerialportDriver();

        MOCK_METHOD1(mock_init, mdv_serialport_configuration_status_t(
                                        mdv_serialport_configuration_t *const));
        MOCK_METHOD0(mock_uninit, void(void));
        MOCK_METHOD2(mock_write,
                     mdv_serialport_driver_status_t(uint32_t *const,
                                                    uint8_t *const));
        MOCK_METHOD2(mock_read, mdv_serialport_driver_status_t(uint32_t *const,
                                                               uint8_t *const));

    private:
        static std::unique_ptr<MockMdvSerialportDriver>
                m_mockMdvSerialportDriver;
        static mdv_serialport_driver_t m_mdvSerialportDriver;
};

/* EOF */
