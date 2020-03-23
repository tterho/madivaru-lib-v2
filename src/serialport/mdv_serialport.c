/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2020, Tuomas Terho
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "mdv_serialport.h"

#include <assert.h>
#include <string.h>

/**
 * \file serialport.c
 * \defgroup mdv-serialport-implementation Implementation
 * \ingroup mdv-serialport
 * @{
 */

/// Default baudrate
#define MDV_SERIALPORT_DEFAULT_BAUDRATE 9600

const mdv_serialport_configuration_t serialport_default_configuration = {
        MDV_SERIALPORT_DEFAULT_BAUDRATE, MDV_SERIALPORT_DATABITS_8,
        MDV_SERIALPORT_PARITY_NONE, MDV_SERIALPORT_STOPBITS_ONE,
        MDV_SERIALPORT_FLOWCONTROL_NONE
};

/**
 * \brief Initialize a transfer
 * \param[in] async_transfer Asynchronous transfer data
 * \param[in] length Data length
 * \param[in] data Data
 * \param[out] transferred_length Transferred data length
 * \param[in] timeout_ms Timeout value in milliseconds
 * \return No return value
 *
 * Marks the transfer incompleted and resets all the required values
 */
static void init_transfer(mdv_serialport_async_transfer_t *const async_transfer,
                          uint32_t const length, uint8_t *const data,
                          uint32_t *const transferred_length,
                          uint32_t const timeout_ms)
{
        async_transfer->flags.transfer_on = MDV_SERIALPORT_TRANSFER_ON;
        async_transfer->data_length = length;
        async_transfer->data = data;
        async_transfer->transferred_length = transferred_length;
        async_transfer->timeout_ms = timeout_ms;
        async_transfer->data_left = length;
        async_transfer->data_left_prev = 0;

        // Reset the transferred data length output
        if (async_transfer->transferred_length) {
                *async_transfer->transferred_length = 0;
        }

        // Start the timeout timer if the timeout has been specified
        if (async_transfer->timeout_ms) {
                mdv_sw_timer_start(&(async_transfer->timer));
        }
}

/**
 * \brief Complete a transfer
 * \param[in] async_transfer Asynchronous transfer data
 * \param[in] status Status of the transfer
 * \return No return value
 *
 * Marks the transfer completed and invokes a callback if the callback has been
 * set.
 */
static void
complete_transfer(mdv_serialport_async_transfer_t *const async_transfer,
                  mdv_serialport_status_t const status)
{
        async_transfer->flags.transfer_on = MDV_SERIALPORT_TRANSFER_OFF;

        // Set the transferred data length output
        if (async_transfer->transferred_length) {
                *async_transfer->transferred_length =
                        async_transfer->data_length - async_transfer->data_left;
        }

        // Invoke the callback
        if (async_transfer->completion_callback) {
                async_transfer->completion_callback(async_transfer->user_data,
                                                    status);
        }
}

/**
 * \brief Cancel an ongoing asynchronous transfer
 * \param[in] async_transfer Asynchronous transfer data
 * \return No return value
 */
static void
cancel_transfer(mdv_serialport_async_transfer_t *const async_transfer)
{
        if (async_transfer->flags.transfer_on) {
                complete_transfer(
                        async_transfer,
                        MDV_SERIALPORT_ERROR_ASYNC_TRANSFER_CANCELLED);
        }
}

/**
 * \brief Manage asynchronous transfer timeouts
 * \param[in] async_transfer Asynchronous transfer data
 * \return Transfer status
 */
static mdv_serialport_status_t manage_asynchronous_timeouts(
        mdv_serialport_async_transfer_t *const async_transfer)
{
        uint32_t time;

        // If timeout not specified, continue infinitely
        if (!(async_transfer->timeout_ms)) {
                return MDV_SERIALPORT_STATUS_OK;
        }

        // If some data got transferred, reset the timeout timer and continue
        // reception
        if (async_transfer->data_left != async_transfer->data_left_prev) {
                async_transfer->data_left_prev = async_transfer->data_left;
                mdv_sw_timer_start(&(async_transfer->timer));
                return MDV_SERIALPORT_STATUS_OK;
        }

        // Check the timeout
        mdv_sw_timer_get_time(&(async_transfer->timer), MDV_SW_TIMER_MS, &time);

        if (time > async_transfer->timeout_ms) {
                complete_transfer(async_transfer, MDV_SERIALPORT_ERROR_TIMEOUT);
                return MDV_SERIALPORT_ERROR_TIMEOUT;
        }

        // Nothing happened, but transfer is still in progress
        return MDV_SERIALPORT_STATUS_OK;
}

/**
 * \brief Run an asynchronous transfer
 * \param[in] transfer_function Driver read/write function
 * \param[in] async_transfer Asynchronous transfer data
 * \return Transfer status
 */
static mdv_serialport_status_t run_asynchronous_transfer(
        mdv_serialport_transfer_function_t const transfer_function,
        mdv_serialport_async_transfer_t *const async_transfer)
{
        mdv_serialport_driver_status_t driver_status;

        // If there is no transfer ongoing, do nothing
        if (!(async_transfer->flags.transfer_on)) {
                return MDV_SERIALPORT_STATUS_OK;
        }

        // Run the driver read or write function to transfer data
        driver_status = transfer_function(&(async_transfer->data_left),
                                          async_transfer->data);

        if (driver_status == MDV_SERIALPORT_DRIVER_HARDWARE_FAILURE) {
                // Something went wrong in the hardware
                complete_transfer(async_transfer,
                                  MDV_SERIALPORT_ERROR_TRANSFER_FAILED);
                return MDV_SERIALPORT_ERROR_TRANSFER_FAILED;
        }

        // If all data has been transferred, complete the transfer
        if (!(async_transfer->data_left)) {
                complete_transfer(async_transfer, MDV_SERIALPORT_STATUS_OK);
                return MDV_SERIALPORT_STATUS_OK;
        }

        return manage_asynchronous_timeouts(async_transfer);
}

/**
 * \brief Run synchronous transfer
 * \param[in] transfer_function Driver read/write function
 * \param[in] async_transfer Asynchronous transfer data
 * \return Status of the operation
 */
static mdv_serialport_status_t run_synchronous_transfer(
        mdv_serialport_transfer_function_t const transfer_function,
        mdv_serialport_async_transfer_t *async_transfer)
{
        mdv_serialport_status_t status;

        // Loop until the transfer has been completed
        while (async_transfer->flags.transfer_on) {
                // Use the asynchronous transfer function to retrieve data
                // and to manage timeouts
                status = run_asynchronous_transfer(transfer_function,
                                                   async_transfer);

                if (status != MDV_SERIALPORT_STATUS_OK) {
                        return status;
                }
        }

        return MDV_SERIALPORT_STATUS_OK;
}

/**
 * \brief Transfers data to/from serial port
 * \param[in] transfer_function Driver read/write function
 * \param[in] async_transfer Asynchronous transfer data structure
 * \param[in] length Data length
 * \param[in] data Data
 * \param[out] transferred_length Transferred data length
 * \param[in] timeout_ms Timeout time in milliseconds
 * \return Current transfer status
 */
static mdv_serialport_status_t
begin_transfer(mdv_serialport_transfer_function_t const transfer_function,
               mdv_serialport_async_transfer_t *async_transfer, uint32_t length,
               uint8_t *data, uint32_t *transferred_length, uint32_t timeout_ms)
{
        // Do not allow starting new transfer if a transfer is already ongoing.
        if (async_transfer->flags.transfer_on) {
                return MDV_SERIALPORT_ERROR_ASYNC_TRANSFER_IN_PROGRESS;
        }

        init_transfer(async_transfer, length, data, transferred_length,
                      timeout_ms);

        // If no callback or timeout defined, use synchronous transfer.
        // Otherwise, start asynchronous process.
        if (!(async_transfer->completion_callback) ||
            !(async_transfer->timeout_ms)) {
                return run_synchronous_transfer(transfer_function,
                                                async_transfer);
        } else {
                return run_asynchronous_transfer(transfer_function,
                                                 async_transfer);
        }
}

void mdv_serialport_init(
        mdv_serialport_t *const serialport,
        mdv_serialport_driver_t *const driver,
        mdv_serialport_transfer_completion_callback_t const rx_callback,
        mdv_serialport_transfer_completion_callback_t const tx_callback,
        void *const user_data)
{
        assert(serialport);
        assert(driver);

        // Initialize the rx transfer data structure
        memset(&(serialport->async_rx), 0,
               sizeof(mdv_serialport_async_transfer_t));
        serialport->async_rx.completion_callback = rx_callback;
        serialport->async_rx.user_data = user_data;

        // Initialize the tx transfer data structure
        memset(&(serialport->async_tx), 0,
               sizeof(mdv_serialport_async_transfer_t));
        serialport->async_tx.completion_callback = tx_callback;
        serialport->async_tx.user_data = user_data;

        serialport->configuration = serialport_default_configuration;
        serialport->driver = driver;
        serialport->is_open = false;
}

void mdv_serialport_get_current_configuration(
        mdv_serialport_t *const serialport,
        mdv_serialport_configuration_t *const configuration)
{
        assert(serialport);
        assert(configuration);

        *configuration = serialport->configuration;
}

mdv_serialport_configuration_status_t
mdv_serialport_open(mdv_serialport_t *const serialport,
                    mdv_serialport_configuration_t *const configuration)
{
        mdv_serialport_configuration_status_t status;

        assert(serialport);
        assert(configuration);

        status = serialport->driver->init(configuration);

        if (status.value == MDV_SERIALPORT_CONFIGURATION_OK) {
                serialport->configuration = *configuration;
                serialport->is_open = true;
        }

        return status;
}

void mdv_serialport_close(mdv_serialport_t *const serialport)
{
        assert(serialport);

        cancel_transfer(&(serialport->async_rx));
        cancel_transfer(&(serialport->async_tx));

        serialport->driver->uninit();
        serialport->is_open = false;
}

bool mdv_serialport_is_open(mdv_serialport_t *const serialport)
{
        assert(serialport);

        return serialport->is_open;
}

mdv_serialport_status_t mdv_serialport_read(mdv_serialport_t *serialport,
                                            uint32_t const length,
                                            uint8_t *const data,
                                            uint32_t *const bytes_read,
                                            uint32_t const timeout_ms)
{
        assert(serialport);
        assert(length);
        assert(data);

        return begin_transfer(serialport->driver->read, &serialport->async_rx,
                              length, data, bytes_read, timeout_ms);
}

mdv_serialport_status_t mdv_serialport_write(mdv_serialport_t *const serialport,
                                             uint32_t length, uint8_t *data,
                                             uint32_t *bytes_written,
                                             uint32_t timeout_ms)
{
        assert(serialport);
        assert(length);
        assert(data);

        return begin_transfer(serialport->driver->write,
                              &(serialport->async_tx), length, data,
                              bytes_written, timeout_ms);
}

mdv_serialport_status_t
mdv_serialport_getchar(mdv_serialport_t *const serialport, uint8_t *const data)
{
        // Read one byte from the serial port synchronously
        return mdv_serialport_read(serialport, 1, data, 0, 0);
}

mdv_serialport_status_t
mdv_serialport_putchar(mdv_serialport_t *const serialport, uint8_t const data)
{
        // Write one byte to the serial port synchronously
        return mdv_serialport_write(serialport, 1, (uint8_t *const) & data, 0,
                                    0);
}

void mdv_serialport_run(mdv_serialport_t *const serialport)
{
        assert(serialport);

        run_asynchronous_transfer(serialport->driver->read,
                                  &(serialport->async_rx));
        run_asynchronous_transfer(serialport->driver->write,
                                  &(serialport->async_tx));
}

/** @} mdv-serialport-implementation */

/* EOF */