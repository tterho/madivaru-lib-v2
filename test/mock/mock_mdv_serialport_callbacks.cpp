#include "mock_mdv_serialport_callbacks.h"

std::unique_ptr<MockMdvSerialportCallbacks>
        MockMdvSerialportCallbacks::m_mockMdvSerialportCallbacks;

void MockMdvSerialportCallbacks::init()
{
        m_mockMdvSerialportCallbacks.reset(
                new testing::NiceMock<MockMdvSerialportCallbacks>());
}

void MockMdvSerialportCallbacks::destroy()
{
        m_mockMdvSerialportCallbacks.reset();
}

MockMdvSerialportCallbacks &MockMdvSerialportCallbacks::instance()
{
        if (!hasInstance()) {
                printf("MockMdvSerialportCallbacks::init() not called!\r\n");
                abort();
        }

        return *m_mockMdvSerialportCallbacks;
}

bool MockMdvSerialportCallbacks::hasInstance()
{
        return (bool)m_mockMdvSerialportCallbacks;
}

void serialport_rx_completed(void *const user_data,
                             mdv_serialport_status_t const status)
{
        MockMdvSerialportCallbacks::instance().serialport_rx_completed(
                user_data, status);
}

void serialport_tx_completed(void *const user_data,
                             mdv_serialport_status_t const status)
{
        MockMdvSerialportCallbacks::instance().serialport_tx_completed(
                user_data, status);
}

/* EOF */
