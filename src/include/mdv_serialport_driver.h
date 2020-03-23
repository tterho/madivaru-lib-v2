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

#ifndef MDV_SERIALPORT_DRIVER_H
#define MDV_SERIALPORT_DRIVER_H

#include "mdv_common.h"

/**
 * \file       mdv_serialport_driver.h
 * \defgroup   mdv-serialport-driver Serial port driver API
 * \ingroup    madivaru-lib-v2
 * \copyright  Copyright &copy; 2020, Tuomas Terho. All rights reserved.
 * \{
 */

/**
 * \brief Data bits per character
 */
typedef enum _mdv_serialport_databits_t {
        /// 5 data bits per character
        MDV_SERIALPORT_DATABITS_5 = 5,
        /// 6 data bits per character
        MDV_SERIALPORT_DATABITS_6,
        /// 7 data bits per character
        MDV_SERIALPORT_DATABITS_7,
        /// 8 data bits per character
        MDV_SERIALPORT_DATABITS_8,
        /// 9 data bits per character
        MDV_SERIALPORT_DATABITS_9
} mdv_serialport_databits_t;

/**
 * \brief Parity on/off and polarity control values
 */
typedef enum _mdv_serialport_parity_t {
        /// No parity
        MDV_SERIALPORT_PARITY_NONE,
        /// Even parity
        MDV_SERIALPORT_PARITY_EVEN,
        /// Odd parity
        MDV_SERIALPORT_PARITY_ODD,
        /// Parity value is permanent '1'
        MDV_SERIALPORT_PARITY_STICK_1,
        /// Parity value is permanent '0'
        MDV_SERIALPORT_PARITY_STICK_0
} mdv_serialport_parity_t;

/**
 * \brief Stop bit control values
 */
typedef enum _mdv_serialport_stopbits_t {
        /// One stop bit
        MDV_SERIALPORT_STOPBITS_ONE = 1,
        /// Two stop bits
        MDV_SERIALPORT_STOPBITS_TWO,
        /// One and half stop bits
        MDV_SERIALPORT_STOPBITS_ONE_AND_HALF
} mdv_serialport_stopbits_t;

/**
 * \brief Flow control
 */
typedef enum _mdv_serialport_flowcontrol_t {
        /// No flow control
        MDV_SERIALPORT_FLOWCONTROL_NONE,
        /// xOn/xOff flow control
        MDV_SERIALPORT_FLOWCONTROL_XONXOFF,
        /// Hardware flow control based on RTS/CTS or DSR/DTR signals
        MDV_SERIALPORT_FLOWCONTROL_HARDWARE
} mdv_serialport_flowcontrol_t;

/**
 * \brief Serial port configuration structure
 */
typedef struct _mdv_serialport_configuration_t {
        /// Baud rate in baud per second
        uint32_t baudrate;
        /// Data bits (character size)
        mdv_serialport_databits_t databits;
        /// Parity
        mdv_serialport_parity_t parity;
        /// Stop bits
        mdv_serialport_stopbits_t stopbits;
        /// Flow control
        mdv_serialport_flowcontrol_t flowcontrol;
} mdv_serialport_configuration_t;

/// Serial port configuration OK value
#define MDV_SERIALPORT_CONFIGURATION_OK 0

/**
 * \brief Configuration status information
 */
typedef union _mdv_serialport_configuration_status_t {
        /// Configuration overall status (0 = OK)
        uint8_t value;
        /// Error flags (1 = error, 0 = OK)
        struct {
                /// Invalid baudrate
                uint8_t invalid_baudrate : 1;
                /// Invalid data bits
                uint8_t invalid_databits : 1;
                /// Invalid parity
                uint8_t invalid_parity : 1;
                /// Invalid stop bits
                uint8_t invaid_stopbits : 1;
                /// Invalid flow control
                uint8_t invalid_flow_control : 1;
                /// Reserved, set always to zero
                uint8_t __reserved : 3;
        } errors;
} mdv_serialport_configuration_status_t;

/**
 * \brief Serial port driver status
 */
typedef enum _mdv_serialport_driver_status_t {
        /// Serial port driver is OK
        MDV_SERIALPORT_DRIVER_STATUS_OK = 0,
        /// Hardware error during transmission
        MDV_SERIALPORT_DRIVER_HARDWARE_FAILURE = -1,
        /// The receiver buffer is empty
        MDV_SERIALPORT_DRIVER_ERROR_RX_BUFFER_EMPTY = -2,
        /// The transmitter buffer is full
        MDV_SERIALPORT_DRIVER_ERROR_TX_BUFFER_FULL = -3
} mdv_serialport_driver_status_t;

/**
 * \brief Transfer data to/from serial port
 * \param[in,out] data_left Data left for transfer
 * \param[in] data Data to transfer
 * \return Serial port driver status
 */
typedef mdv_serialport_driver_status_t (*mdv_serialport_transfer_function_t)(
        uint32_t *const data_left, uint8_t *const data);

/**
 * \brief Serial port driver API
 */
typedef struct _mdv_serialport_driver_t {
        /**
   * \brief Initialize the driver
   * \param[in] configuration Serial port configuration
   * \return Configuration status
   */
        mdv_serialport_configuration_status_t (*init)(
                mdv_serialport_configuration_t *const configuration);
        /**
   * \brief Uninitialize the driver
   * \return No return value
   */
        void (*uninit)(void);
        /**
   * \brief Write data to the serial port
   */
        mdv_serialport_transfer_function_t write;
        /**
   * \brief Read data from the serial port
   */
        mdv_serialport_transfer_function_t read;
} const mdv_serialport_driver_t;

/** \} mdv-serialport-driver */

#endif // ifndef MDV_SERIALPORT_DRIVER_H

/* EOF */
