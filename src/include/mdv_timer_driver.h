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

#ifndef MDV_TIMER_DRIVER_H
#define MDV_TIMER_DRIVER_H

#include "mdv_common.h"

/**
 * \file       mdv_timer_driver.h
 * \defgroup   mdv-timer-driver Timer driver API
 * \ingroup    madivaru-lib-v2
 * \copyright  Copyright &copy; 2020, Tuomas Terho. All rights reserved.
 * \{
 */

/**
 * \brief Timer event handler callback function type
 *
 * \param[in] user_data Pointer to user data passed to the event handler
 * \param[in] counter The current timer counter value
 *
 * \return No return value
 */
typedef void (*mdv_timer_event_handler_t)(void *const user_data,
        uint32_t const counter);

/**
 * \brief Timer driver interface definition
 */
typedef struct _mdv_timer_driver_t{
        /**
         * \brief Initialize timer hardware
         *
         * \param[in] event_handler Timer event handler callback
         * \param[in] user_data User data to be passed to the event handler
         *
         * \return Implementation specific result of the operation
         */
        mdv_result_t (*init)(mdv_timer_event_handler_t const event_handler,
                void *const user_data);
        /**
         * \brief Uninitialize timer hardware
         *
         * \return Implementation specific result of the operation
         */
        mdv_result_t (*uninit)(void);
        /**
         * \brief Start the timer
         *
         * \return Implementation specific result of the operation
         */
        mdv_result_t (*start)(void);
        /**
         * \brief Stop the timer
         *
         * \param[in] instance Instance to the timer hardware
         *
         * \return Implementation specific result of the operation
         */
        mdv_result_t (*stop)(void);
        /**
         * \brief Reset the timer
         *
         * \return Implementation specific result of the operation
         */
        mdv_result_t (*reset)(void);
        /**
         * \brief Gets the current timer counter value
         *
         * \return The current timer counter value in ticks
         */
        uint32_t (*get_count)(void);
        /**
         * \brief Gets the current status of the timer
         *
         * \retval true Timer is running
         * \retval false Timer is stopped
         */
        bool (*is_running)(void);
} const mdv_timer_driver_t;

/** \} mdv-timer-driver */

#endif // ifndef MDV_TIMER_DRIVER_H

/* EOF */
