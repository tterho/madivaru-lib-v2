#pragma once

#include "mdv_serialport.h"
#include <gmock/gmock.h>

/*
 * Mock for serial port RX and TX callback functions
 */
class MockMdvSerialportCallbacks {
    public:
        virtual ~MockMdvSerialportCallbacks()
        {
        }

        static void init();
        static void destroy();
        static bool hasInstance();
        static MockMdvSerialportCallbacks &instance();

        MOCK_METHOD2(serialport_rx_completed,
                     void(void *const, mdv_serialport_status_t const));
        MOCK_METHOD2(serialport_tx_completed,
                     void(void *const, mdv_serialport_status_t const));

    private:
        static std::unique_ptr<MockMdvSerialportCallbacks>
                m_mockMdvSerialportCallbacks;
};

extern "C" {

// Mock for RX completed callbacks to be used with the serial port API
// (mdv_serialport module)
void serialport_rx_completed(void *const user_data,
                             mdv_serialport_status_t const status);

// Mock for TX completed callbacks to be used with the serial port API
// (mdv_serialport module)
void serialport_tx_completed(void *const user_data,
                             mdv_serialport_status_t const status);

} // extern "C"

/* EOF */
