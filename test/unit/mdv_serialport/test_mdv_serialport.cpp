#include "mdv_serialport.c"
#include "mock_mdv_serialport_driver.h"
#include "mock_mdv_serialport_callbacks.h"
#include <gtest/gtest.h>

static mdv_serialport_configuration_t test_configuration = {
        112000, MDV_SERIALPORT_DATABITS_7, MDV_SERIALPORT_PARITY_ODD,
        MDV_SERIALPORT_STOPBITS_TWO, MDV_SERIALPORT_FLOWCONTROL_HARDWARE
};

using namespace testing;

namespace
{
class test_mdv_serialport : public Test {
    protected:
        void SetUp() override
        {
                MockMdvSerialportDriver::init();
                MockMdvSerialportCallbacks::init();
                m_serialport_driver =
                        MockMdvSerialportDriver::GetMdvSerialportDriver();
                memset(&m_serialport, 0, sizeof(mdv_serialport_t));
                memset(&m_configuration, 0,
                       sizeof(mdv_serialport_configuration_t));
                memset(&m_status, 0,
                       sizeof(mdv_serialport_configuration_status_t));
        }

        void TearDown() override
        {
                MockMdvSerialportDriver::destroy();
                MockMdvSerialportCallbacks::destroy();
        }

        void SerialportInit()
        {
                mdv_serialport_init(&m_serialport, m_serialport_driver,
                                    serialport_rx_completed,
                                    serialport_tx_completed, &m_user_data);
        }

        void SerialportOpen()
        {
                SerialportInit();
                mdv_serialport_open(&m_serialport, &m_configuration);
        }

        mdv_serialport_t m_serialport;
        mdv_serialport_driver_t *m_serialport_driver;
        mdv_serialport_configuration_t m_configuration;
        mdv_serialport_configuration_status_t m_status;
        uint32_t m_user_data;
        /*
        static void serialport_rx_completed(void *const user_data,
                                 mdv_serialport_status_t const status);
        static void serialport_tx_completed(void *const user_data,
                                 mdv_serialport_status_t const status);
*/
};
/*
void test_mdv_serialport::serialport_rx_completed(void *const user_data,
                                       mdv_serialport_status_t const status)
{
}

void test_mdv_serialport::serialport_tx_completed(void *const user_data,
                                       mdv_serialport_status_t const status)
{
}
*/

TEST_F(test_mdv_serialport, init__invalid_parameters_cause_assertion_failure)
{
        EXPECT_DEATH(mdv_serialport_init(0, m_serialport_driver,
                                         serialport_rx_completed,
                                         serialport_tx_completed, 0),
                     "")
                << "If null, serialport must cause an assertion failure.";
        EXPECT_DEATH(mdv_serialport_init(&m_serialport, 0,
                                         serialport_rx_completed,
                                         serialport_tx_completed, 0),
                     "")
                << "If null, serialport must cause an assertion failure.";
}

TEST_F(test_mdv_serialport, init__serialport_initialized)
{
        // Initialize the serial port with 0xff to ensure that all
        // fields are filled properly
        memset(&m_serialport, 0xff, sizeof(mdv_serialport_t));

        mdv_serialport_init(&m_serialport, m_serialport_driver,
                            serialport_rx_completed, serialport_tx_completed,
                            &m_user_data);

        EXPECT_EQ(serialport_rx_completed,
                  m_serialport.async_rx.completion_callback)
                << "Async RX must be initialized with the mock function" \
                   "serialport_rx_completed (in mdv_serialport_callbacks).";
        EXPECT_EQ(&m_user_data, m_serialport.async_rx.user_data)
                << "Async RX must be initialized with user data.";

        EXPECT_EQ(serialport_tx_completed,
                  m_serialport.async_tx.completion_callback)
                << "Async TX must be initialized with the mock function " \
                   "serialport_tx_completed (in mdv_serialport_callbacks).";
        EXPECT_EQ(&m_user_data, m_serialport.async_tx.user_data)
                << "Async TX must be initialized with user data.";

        EXPECT_EQ(0, memcmp(&serialport_default_configuration,
                            &(m_serialport.configuration),
                            sizeof(mdv_serialport_configuration_t)))
                << "Serial port configuration must be initialized with "
                   "default values.";

        EXPECT_EQ(m_serialport_driver, m_serialport.driver)
                << "Serial port driver be set.";
        EXPECT_FALSE(m_serialport.is_open)
                << "Serial port must be initially closed.";
}

TEST_F(test_mdv_serialport,
       get_current_configuration__invalid_parameters_cause_assertion_failure)
{
        EXPECT_DEATH(mdv_serialport_get_current_configuration(0,
                                                              &m_configuration),
                     "")
                << "If null, serialport must cause an assertion failure.";
        EXPECT_DEATH(mdv_serialport_get_current_configuration(&m_serialport, 0),
                     "")
                << "If null, configuration must cause an assertion failure.";
}

TEST_F(test_mdv_serialport, get_current_configuration__successful)
{
        SerialportInit();

        mdv_serialport_get_current_configuration(&m_serialport,
                                                 &m_configuration);

        EXPECT_EQ(0, memcmp(&m_configuration, &serialport_default_configuration,
                            sizeof(mdv_serialport_configuration_t)))
                << "The current configuration must be set to the output.";
}

TEST_F(test_mdv_serialport, open__invalid_parameters_cause_assertion_failure)
{
        EXPECT_DEATH(mdv_serialport_open(0, &test_configuration), "")
                << "If null, serialport must cause an assertion failure.";
        EXPECT_DEATH(mdv_serialport_open(&m_serialport, 0), "")
                << "If null, configuration must cause an assertion failure.";
}

TEST_F(test_mdv_serialport, open__successful)
{
        mdv_serialport_configuration_status_t status;

        status.value = 0xff;

        SerialportInit();

        EXPECT_CALL(MockMdvSerialportDriver::instance(), mock_init(_))
                .WillOnce(Return(m_status));

        status = mdv_serialport_open(&m_serialport, &test_configuration);

        EXPECT_EQ(0, memcmp(&(m_serialport.configuration), &test_configuration,
                            sizeof(mdv_serialport_configuration_t)))
                << "The new configuration must be set to the output.";
        EXPECT_TRUE(m_serialport.is_open)
                << "The serial port must be set open.";
        EXPECT_EQ(MDV_SERIALPORT_CONFIGURATION_OK, status.value)
                << "The serial port configuration status must be returned.";
}

TEST_F(test_mdv_serialport, open__invalid_configuration)
{
        mdv_serialport_configuration_status_t status;

        status.value = 0xff;

        SerialportInit();

        // Set at least one of the configuration status bits to indicate
        // invalid configuration.
        m_status.errors.invalid_baudrate = 1;

        EXPECT_CALL(MockMdvSerialportDriver::instance(), mock_init(_))
                .WillOnce(Return(m_status));

        status = mdv_serialport_open(&m_serialport, &test_configuration);

        EXPECT_EQ(0, memcmp(&(m_serialport.configuration),
                            &serialport_default_configuration,
                            sizeof(mdv_serialport_configuration_t)))
                << "The invalid configuration must NOT be set to the output.";
        EXPECT_FALSE(m_serialport.is_open)
                << "The serial port must NOT be set open.";
        EXPECT_EQ(status.value, m_status.value)
                << "The returned status value must indicate invalid "
                   "configuration.";
}

TEST_F(test_mdv_serialport, close__invalid_parameters_cause_assertion_failure)
{
        EXPECT_DEATH(mdv_serialport_close(0), "")
                << "If null, serialport must cause an assertion failure.";
}

TEST_F(test_mdv_serialport, close__successful)
{
        SerialportOpen();

        // Emulate ongoing transfers
        m_serialport.async_rx.flags.transfer_on = 1;
        m_serialport.async_tx.flags.transfer_on = 1;

        // Expect that the transfers are cancelled and the driver uninitialized
        EXPECT_CALL(MockMdvSerialportCallbacks::instance(),
                    serialport_rx_completed(_, _));
        EXPECT_CALL(MockMdvSerialportCallbacks::instance(),
                    serialport_tx_completed(_, _));
        EXPECT_CALL(MockMdvSerialportDriver::instance(), mock_uninit());

        mdv_serialport_close(&m_serialport);

        EXPECT_EQ(0, m_serialport.async_rx.flags.transfer_on)
                << "RX transfer must be set off.";
        EXPECT_EQ(0, m_serialport.async_tx.flags.transfer_on)
                << "TX transfer must be set off.";

        EXPECT_FALSE(m_serialport.is_open)
                << "The serial port must be set closed.";
}

TEST_F(test_mdv_serialport, is_open__invalid_parameters_cause_assertion_failure)
{
        EXPECT_DEATH(mdv_serialport_is_open(0), "")
                << "If null, serialport must cause an assertion failure.";
}

TEST_F(test_mdv_serialport, is_open__successful)
{
        bool is_open;

        m_serialport.is_open = false;

        is_open = mdv_serialport_is_open(&m_serialport);

        EXPECT_FALSE(is_open);

        m_serialport.is_open = true;

        is_open = mdv_serialport_is_open(&m_serialport);

        EXPECT_TRUE(is_open);
}



#if 0
void mdv_serialport_close(mdv_serialport_t *const serialport);

bool mdv_serialport_is_open(mdv_serialport_t *const serialport);

mdv_serialport_configuration_status_t serialport_change_configuration(
        mdv_serialport_t *const serialport,
        mdv_serialport_configuration_t *const configuration);

mdv_serialport_status_t mdv_serialport_read(mdv_serialport_t *const serialport,
                                            uint32_t const length,
                                            uint8_t *const data,
                                            uint32_t *const bytes_read,
                                            uint32_t const timeout_ms);

mdv_serialport_status_t mdv_serialport_write(mdv_serialport_t *const serialport,
                                             uint32_t const length,
                                             uint8_t *const data,
                                             uint32_t *const bytes_written,
                                             uint32_t const timeout_ms);

mdv_serialport_status_t mdv_serialport_getchar(mdv_serialport_t *const serialport,
                                               uint8_t *data);

mdv_serialport_status_t mdv_serialport_putchar(mdv_serialport_t *const serialport,
                                               uint8_t data);

void mdv_serialport_run(mdv_serialport_t *const serialport);

TEST_F(test_mdv_serialport,
       init__serialport_driver_init_not_called_if_serialport_driver_not_provided)
{
        EXPECT_CALL(MockMdvSerialportDriver::instance(), mdv_serialport_driver_init(0, 0))
                .Times(0);

        // This will lead into segmentation fault, if the null value of the
        // serialport_driver pointer is not handled correctly
        mdv_serialport_init(&m_serialport, TEST_TICK_DURATION_US,
                               TEST_TIMER_WIDTH_BITS, 0);
}

TEST_F(test_mdv_serialport,
       uninit__invalid_parameters_cause_assertion_failure)
{
        EXPECT_DEATH(mdv_serialport_uninit(0), "")
                << "If null, serialport must cause an assertion failure.";
}

TEST_F(test_mdv_serialport,
       uninit__serialport_driver_init_not_called_if_serialport_not_initialized)
{
        EXPECT_CALL(MockMdvSerialportDriver::instance(), mdv_serialport_driver_uninit())
                .Times(0);

        // This will cause a segmentation fault if the null value of the timer
        // driver pointer is not handled properly
        mdv_serialport_uninit(&m_serialport);
}

TEST_F(test_mdv_serialport, uninit__serialport_uninitialized)
{
        SerialportInit();

        EXPECT_CALL(MockMdvSerialportDriver::instance(),
                    mdv_serialport_driver_uninit())
                .WillOnce(Return(MDV_RESULT_OK));

        mdv_serialport_uninit(&m_serialport);
}

TEST_F(test_mdv_serialport,
       tick__invalid_parameters_cause_assertion_failure)
{
        SerialportInit();

        EXPECT_DEATH(mdv_serialport_tick(0, TEST_TIMER_TICK), "")
                << "If null, serialport must cause an assertion failure.";
        EXPECT_DEATH(mdv_serialport_tick(&m_serialport, 0), "")
                << "If zero, tick must cause an assertion failure.";
}

TEST_F(test_mdv_serialport,
       tick__tick_counter_advanced_and_wrapped_around_successfully)
{
        SerialportInit();

        mdv_serialport_tick(&m_serialport, TEST_TIMER_TICK);

        EXPECT_EQ(TEST_TIMER_TICK, m_serialport.tick_counter)
                << "Tick counter must be advanced by the test value.";

        // Set the tick counter to the mask value to demonstrate the wrap
        // around.
        m_serialport.tick_counter = TEST_TIMER_MASK;

        mdv_serialport_tick(&m_serialport, 1);

        EXPECT_EQ(0, m_serialport.tick_counter)
                << "Tick counter must wrap around when adding 1 to the " \
                   "maximum value.";
}

TEST_F(test_mdv_serialport,
       get_tick_count__invalid_parameters_cause_assertion_failure)
{
        SerialportInit();

        EXPECT_DEATH(mdv_serialport_get_tick_count(0), "")
                << "If null, serialport must cause an assertion failure.";
}

TEST_F(test_mdv_serialport,
       get_tick_count__tick_count_returned_successfully)
{
        uint32_t tick_count;

        SerialportInit();

        EXPECT_CALL(MockMdvSerialportDriver::instance(), mdv_serialport_driver_get_count())
                .WillOnce(Return(TEST_TIMER_TICK));

        tick_count = mdv_serialport_get_tick_count(&m_serialport);

        EXPECT_EQ(TEST_TIMER_TICK, tick_count)
                << "The tick count must be read and returned from the timer " \
                   "driver, when the driver is present.";

        // Set the local tick count to a known value
        m_serialport.tick_counter = TEST_TIMER_TICK;

        // Remove the timer driver association
        m_serialport.serialport_driver = 0;

        // Get the tick count
        tick_count = mdv_serialport_get_tick_count(&m_serialport);

        EXPECT_EQ(TEST_TIMER_TICK, tick_count)
                << "The tick count must be read and returned from the local " \
                    "counter variable, when the timer driver is not present.";
}

TEST_F(test_mdv_serialport,
       get_tick_duration_us__invalid_parameters_cause_assertion_failure)
{
        SerialportInit();

        EXPECT_DEATH(mdv_serialport_get_tick_duration_us(0), "")
                << "If null, serialport must cause an assertion failure.";
}

TEST_F(test_mdv_serialport,
       get_tick_duration_us__value_returned_successfully)
{
        uint32_t tick_duration_us;

        SerialportInit();

        tick_duration_us = mdv_serialport_get_tick_duration_us(&m_serialport);

        EXPECT_EQ(TEST_TICK_DURATION_US, tick_duration_us)
                << "The tick duration must be returned.";
}

TEST_F(test_mdv_serialport,
       get_timer_mask__invalid_parameters_cause_assertion_failure)
{
        SerialportInit();

        EXPECT_DEATH(mdv_serialport_get_timer_mask(0), "")
                << "If null, serialport must cause an assertion failure.";
}

TEST_F(test_mdv_serialport,
       get_timer_mask__value_returned_successfully)
{
        uint32_t timer_mask;

        SerialportInit();

        timer_mask = mdv_serialport_get_timer_mask(&m_serialport);

        EXPECT_EQ(TEST_TIMER_MASK, timer_mask)
                << "The timer mask must be returned.";
}
#endif
} // namespace