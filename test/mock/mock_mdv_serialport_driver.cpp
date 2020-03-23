#include "mock_mdv_serialport_driver.h"

std::unique_ptr<MockMdvSerialportDriver>
        MockMdvSerialportDriver::m_mockMdvSerialportDriver;

void MockMdvSerialportDriver::init()
{
        m_mockMdvSerialportDriver.reset(
                new testing::NiceMock<MockMdvSerialportDriver>());
}

void MockMdvSerialportDriver::destroy()
{
        m_mockMdvSerialportDriver.reset();
}

MockMdvSerialportDriver &MockMdvSerialportDriver::instance()
{
        if (!hasInstance()) {
                printf("MockMdvSerialportDriver::init() not called!\r\n");
                abort();
        }

        return *m_mockMdvSerialportDriver;
}

bool MockMdvSerialportDriver::hasInstance()
{
        return (bool)m_mockMdvSerialportDriver;
}

mdv_serialport_driver_t *MockMdvSerialportDriver::GetMdvSerialportDriver()
{
        return &MockMdvSerialportDriver::m_mdvSerialportDriver;
}

extern "C" {

static mdv_serialport_configuration_status_t
mock_init(mdv_serialport_configuration_t *const configuration)
{
        return MockMdvSerialportDriver::instance().mock_init(configuration);
}

static void mock_uninit(void)
{
        MockMdvSerialportDriver::instance().mock_uninit();
}

static mdv_serialport_driver_status_t mock_write(uint32_t *const data_left,
                                                 uint8_t *const data)
{
        return MockMdvSerialportDriver::instance().mock_write(data_left, data);
}

static mdv_serialport_driver_status_t mock_read(uint32_t *const data_left,
                                                uint8_t *const data)
{
        return MockMdvSerialportDriver::instance().mock_read(data_left, data);
}

} // extern "C"

/*
 * Mocked timer driver interface
 */
mdv_serialport_driver_t MockMdvSerialportDriver::m_mdvSerialportDriver = {
        ::mock_init, ::mock_uninit, ::mock_write, ::mock_read
};
