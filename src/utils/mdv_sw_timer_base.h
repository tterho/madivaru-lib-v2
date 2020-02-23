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

#ifndef MDV_SW_TIMER_BASE_H
#define MDV_SW_TIMER_BASE_H

#include "mdv_timer_driver.h"

/**
 * \file       mdv_sw_timer_base.h
 * \defgroup   mdv-sw-timer-base Software timer base system
 * \ingroup    madivaru-lib-v2
 * \copyright  Copyright &copy; 2020, Tuomas Terho. All rights reserved.
 *
 * The software timer base enables one hardware timer being used for multiple
 * software timers. It provides an interface and configuration for software
 * timers using the base.
 *
 * The software timer base can be used by two ways depending on the behavior of
 * the hardware timer: interrupt based, or directly polling the hardware timer
 * counter. If the hardware timer driver is provided as a parameter to the
 * initialization function, the software timer base uses the direct polling.
 * If no hardware time driver is provided, the timer interrupt must call the
 * tick function from the timer interrupt handler sequentially to advance the
 * internal counter of the timer base.
 *
 * The other interface functions are used by software timers which rely on this
 * timer base.
 *
 * @{
 */

/**
 * \brief Software timer base results and error codes
 */
typedef enum _mdv_sw_timer_base_result_t{
        /// Result ok
        MDV_SW_TIMER_BASE_OK = 0,
        /// Invalid pointer parameter
        MDV_SW_TIMER_BASE_ERROR_INVALID_POINTER = -1,
        /// Invalid parameter value
        MDV_SW_TIMER_BASE_ERROR_INVALID_PARAMETER = -2
} mdv_sw_timer_base_result_t;

/**
 * \brief Software timer base instance data
 */
typedef struct _mdv_sw_timer_base_t{
        /// Timer driver
        mdv_timer_driver_t *timer_driver;
        /// Timer tick counter
        uint32_t tick_counter;
        /// One time tick duration (in microseconds)
        uint32_t tick_duration_us;
        /// Timer mask
        uint32_t timer_mask;
} mdv_sw_timer_base_t;

#ifdef __cplusplus
extern "C"{
#endif // ifdef __cplusplus

/**
 * \brief Initialize the software timer base
 *
 * \param[in] sw_timer_base Software timer base to be initialized
 * \param[in] tick_duration_us Configures duration of one timer tick in
 *      microseconds
 * \param[in] timer_width_bits Timer width in bits from 1 to 32
 * \param[in] timer_driver A pointer to a hardware timer driver (optional, used
 *      for polling)
 *
 * \retval MDV_SW_TIMER_BASE_OK Initialization successful
 * \retval MDV_SW_TIMER_BASE_ERROR_INVALID_POINTER The sw_timer_base pointer
 *      points to null
 * \retval MVD_SW_TIMER_BASE_ERROR_INVALID_PARAMETER The tick_duration_us and
 *      the timer_width_bits parameters can't be null
*/
mdv_sw_timer_base_result_t mdv_sw_timer_base_init(
        mdv_sw_timer_base_t *const sw_timer_base,
        uint32_t const tick_duration_us, uint8_t const timer_width_bits,
        mdv_timer_driver_t *const timer_driver);

/**
 * \brief Uninitialize the software timer base
 *
 * \param[in] sw_timer_base Software timer base to be uninitialized
 *
 * \retval MDV_SW_TIMER_BASE_OK Initialization successful
 * \retval MDV_SW_TIMER_BASE_ERROR_INVALID_POINTER The sw_timer_base pointer
 *      points to null
*/
mdv_sw_timer_base_result_t mdv_sw_timer_base_uninit(
        mdv_sw_timer_base_t *const sw_timer_base);

/**
 * \brief Advance the timer system by the given timer tick count
 *
 * \param[in] sw_timer_base Timer system in use
 * \param[in] tick_count Count to advance the timer by
 *
 * \retval MDV_SW_TIMER_BASE_OK Initialization successful
 * \retval MDV_SW_TIMER_BASE_ERROR_INVALID_POINTER The sw_timer_base pointer
 *      points to null
 */
mdv_sw_timer_base_result_t mdv_sw_timer_base_tick(
        mdv_sw_timer_base_t *const sw_timer_base, uint32_t tick_count);

/**
 * \brief Get the current tick count
 *
 * \param[in] sw_timer_base Timer system in use
 *
 * \return Current tick count
 */
uint32_t mdv_sw_timer_base_get_tick_count(
        mdv_sw_timer_base_t *const sw_timer_base);

/**
 * \brief Get the tick duration
 *
 * \param[in] sw_timer_base Timer system in use
 *
 * \return Tick duration in microseconds
 */
uint32_t mdv_sw_timer_base_get_tick_duration_us(
        mdv_sw_timer_base_t *const sw_timer_base);

/**
 * \brief Get the timer mask
 *
 * \param[in] sw_timer_base Timer system in use
 *
 * \return Timer mask
 */
uint32_t mdv_sw_timer_base_get_timer_mask(
        mdv_sw_timer_base_t *const sw_timer_base);

#ifdef __cplusplus
}
#endif // ifdef __cplusplus

/** @} mdv-sw-timer-base */

#endif // ifndef MDV_SW_TIMER_BASE_H

/* EOF */