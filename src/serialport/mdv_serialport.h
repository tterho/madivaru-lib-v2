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

#ifndef MDV_SERIALPORT_H
#define MDV_SERIALPORT_H

#include "mdv_serialport_driver.h"
#include "mdv_sw_timer.h"

/**
 * \file mdv_serialport.h
 * \defgroup mdv_serialport Serial port API
 * \ingroup    madivaru-lib-v2
 * \copyright  Copyright &copy; 2020, Tuomas Terho. All rights reserved.
 *
 * Serial port API and implementation with support for asynchronous transfers.
 *
 * @{
 */

/**
 * \brief Serial port status
 */
typedef enum _mdv_serialport_status_t {
        /// Serial port is ok
        MDV_SERIALPORT_STATUS_OK = 0,
        /// Asynchronous transfer is in progress
        MDV_SERIALPORT_ERROR_ASYNC_TRANSFER_IN_PROGRESS = -1,
        /// The receiver buffer is empty (synchronous transmission only)
        MDV_SERIALPORT_ERROR_RX_BUFFER_EMPTY = -2,
        /// The transmitter buffer is full (synchronous transmission only)
        MDV_SERIALPORT_ERROR_TX_BUFFER_FULL = -3,
        /// Asynchronous transfer has been cancelled by the user
        MDV_SERIALPORT_ERROR_ASYNC_TRANSFER_CANCELLED = -4,
        /// Transfer failed (hardware or transmission line failure)
        MDV_SERIALPORT_ERROR_TRANSFER_FAILED = -5,
        /// Transfer has been timed out
        MDV_SERIALPORT_ERROR_TIMEOUT = -6
} mdv_serialport_status_t;

/**
 * \brief Transfer completion callback
 * \param[in] user_data A pointer to user specified data to be passed to the
 *      callback handler
 * \param[in] status Status of the transfer
 * \return No return value
 *
 * This callback is invoked by the driver on the completion of an asynchronous
 * transfer.
 */
typedef void (*mdv_serialport_transfer_completion_callback_t)(
        void *const user_data, mdv_serialport_status_t const status);

/// Transfer ongoing flag ON value
#define MDV_SERIALPORT_TRANSFER_ON 1
/// Transfer ongoing flag OFF value
#define MDV_SERIALPORT_TRANSFER_OFF 0

/**
 * \brief Asynchronous transfer process data
 */
typedef struct _mdv_serialport_async_transfer_t {
        /// Flags
        struct {
                /// Transfer ongoing flag
                uint8_t transfer_on : 1;
        } flags;
        /// Data length in bytes
        uint32_t data_length;
        /// Pointer to the length of data transferred
        uint32_t *transferred_length;
        /// Data being transferred
        uint8_t *data;
        /// Length of data to be transferred
        uint32_t data_left;
        /// Previous length of data to be transferred
        uint32_t data_left_prev;
        /// Transfer timeout in time milliseconds
        uint32_t timeout_ms;
        /// Timer for the timeout
        mdv_sw_timer_t timer;
        /// Transfer completed callback.
        mdv_serialport_transfer_completion_callback_t completion_callback;
        /// A pointer to user specified data for the callback.
        void *user_data;
} mdv_serialport_async_transfer_t;

/**
 * \brief Serial port
 */
typedef struct _mdv_serialport_t {
        /// Serial port driver
        mdv_serialport_driver_t *driver;
        /// Serial port configuration data
        mdv_serialport_configuration_t configuration;
        /// Asynchronous transfer data for receiver
        mdv_serialport_async_transfer_t async_rx;
        /// Asynchronous transfer data for transmitter
        mdv_serialport_async_transfer_t async_tx;
        /// Serial port open/closed indicator
        bool is_open;
} mdv_serialport_t;

/**
 * \brief Default port configuration
 *
 * The default port configuration is the following:
 *      * Baudrate: 9600 bps
 *      * Data bits: 8
 *      * Parity: none
 *      * Stop bits: 1
 *      * Flow control: None
 */
extern const mdv_serialport_configuration_t serialport_default_configuration;

#ifdef __cplusplus
extern "C" {
#endif // ifdef __cplusplus

/**
 * \brief Initialize a serial port
 * \param[in] serialport Serial port
 * \param[in] driver Serial port driver
 * \param[in] rx_completed RX completed callback for asynchronous read
 *     operations. Set to null for synchronous-only communication.
 * \param[in] tx_completed TX completed callback for asynchronous write
 *     operations. Set to null for synchronous-only communication.
 * \param[in] user_data A pointer to user specified data
 * \return No return value
 */
void mdv_serialport_init(
        mdv_serialport_t *const serialport,
        mdv_serialport_driver_t *const driver,
        mdv_serialport_transfer_completion_callback_t const rx_completed,
        mdv_serialport_transfer_completion_callback_t const tx_completed,
        void *const user_data);

/**
 * \brief Return the current configuration of the given serial port
 * \param[in] serialport Serial port
 * \param[out] configuration Pointer to a configuration structure where the
 *     information will be copied
 * \return No return value
 */
void mdv_serialport_get_current_configuration(
        mdv_serialport_t *const serialport,
        mdv_serialport_configuration_t *const configuration);

/**
 * \brief Open a serial port
 * \param[in] serialport Serial port
 * \param[in] configuration Configuration to use
 * \return Configuration status
 *
 * Opens a serial port using the given configuration.
 */
mdv_serialport_configuration_status_t
mdv_serialport_open(mdv_serialport_t *const serialport,
                    mdv_serialport_configuration_t *const configuration);

/**
 * \brief Closes a serial port
 * \param[in] serialport Serial port
 * \return No return value
 *
 * Closes a serial port and resets the given port handle.
 */
void mdv_serialport_close(mdv_serialport_t *const serialport);

/**
 * \brief Check if the serial port is open or not
 * \param[in] serialport Serial port
 * \retval true Serial port is open
 * \retval false Serial port is closed
 */
bool mdv_serialport_is_open(mdv_serialport_t *const serialport);

/**
 * \brief Read data from a serial port
 * \param[in] serialport Serial port
 * \param[in] length Data length in bytes
 * \param[out] data Pointer to an output buffer
 * \param[out] bytes_read Pointer to a variable for received data length. This
 *     parameter is optional and can be null.
 * \param[in] timeout_ms Maximum time in milliseconds between two bytes or an
 *     initial wait timeout.
 * \return Serial port status
 *
 * Reads a chunk of data from the serial port. If not enough data received
 * within the given timeout time, returns an error.
 */
mdv_serialport_status_t mdv_serialport_read(mdv_serialport_t *const serialport,
                                            uint32_t const length,
                                            uint8_t *const data,
                                            uint32_t *const bytes_read,
                                            uint32_t const timeout_ms);

/**
 * \brief Write data to a serial port
 * \param[in] serialport Serial port
 * \param[in] length Data length in bytes
 * \param[in] data Pointer to a data buffer
 * \param[out] bytes_written Pointer to a variable for transmitted data
 *     length. This parameter is optional and can be null.
 * \param[in] timeout_ms Maximum time in milliseconds between two bytes or an
 *     initial wait timeout.
 * \return Serial port status
 *
 * Writes a chunk of data to a TX buffer.
 */
mdv_serialport_status_t mdv_serialport_write(mdv_serialport_t *const serialport,
                                             uint32_t const length,
                                             uint8_t *const data,
                                             uint32_t *const bytes_written,
                                             uint32_t const timeout_ms);

/**
 * \brief Get a single character from a serial port
 * \param[in] serialport Serial port
 * \param[out] data Pointer to a character variable
 * \return Serial port status
 *
 * Gets a single character from the serial port. If the RX buffer is empty,
 * or a receiver error encountered returns an error without waiting.
 */
mdv_serialport_status_t
mdv_serialport_getchar(mdv_serialport_t *const serialport, uint8_t *const data);

/**
 * \brief Put a single character to a serial port
 * \param[in] serialport Serial port
 * \param[in] data Character to put
 * \return Serial port status
 *
 * Puts a single character to the TX buffer. If the buffer is full, returns
 * an error without waiting.
 */
mdv_serialport_status_t
mdv_serialport_putchar(mdv_serialport_t *const serialport, uint8_t const data);

/**
 * \brief Run asynchronous transfers
 * \param[in] serialport Serial port
 * \return No return value
 *
 * This function must be called periodically in order to run asynchronous
 * transfers. The callback functions are called in this context.
 */
void mdv_serialport_run(mdv_serialport_t *const serialport);

#ifdef __cplusplus
}
#endif // ifdef __cplusplus

/** @} serialport */

#endif // ifndef MDV_SERIALPORT_H

/* EOF */
