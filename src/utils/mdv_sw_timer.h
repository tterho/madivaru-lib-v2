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

#ifndef MDV_SW_TIMER_H
#define MDV_SW_TIMER_H

#include "mdv_sw_timer_base.h"

/**
 * \file       mdv_sw_timer.h
 * \defgroup   mdv-sw-timer Software timer
 * \ingroup    madivaru-lib-v2
 * \copyright  Copyright &copy; 2020, Tuomas Terho. All rights reserved.
 *
 * This timer is equipped with a starvation avereness functionality. That
 * feature is used to detect situations where the underlying hardware or
 * software timer stops running, but the software is still waiting the timer.
 * The starvation detection recognizes the situation and raises an error.
 *
 * If the starvation avareness is not needed, the functionality can be disabled
 * by adding the define MDV_DISABLE_SW_TIMER_STARVATION_AVERENESS to the project
 * options. Disabling the feature saves some memory for each timer instance, and
 * has a small impact to the overall timer perfomance.
 *
 * @{
 */

/**
 * \brief Order of magnitude of time
 */
typedef enum _mdv_sw_timer_order_of_magnitude_t{
        /// The order of magnitude equals to timer ticks
        MDV_SW_TIMER_TIMERTICK = 0,
        /// The order of magnitude is microseconds
        MDV_SW_TIMER_US,
        /// The order of magnitude is milliseconds
        MDV_SW_TIMER_MS,
        /// The order of magnitude is seconds
        MDV_SW_TIMER_S
} mdv_sw_timer_order_of_magnitude_t;

#ifndef MDV_DISABLE_SW_TIMER_STARVATION_AVERENESS
/**
 * \brief Timer starvation avereness data
 */
typedef struct _mdv_sw_timer_starvation_avereness_t{
        /// Limit for \ref mdv_sw_timer_get_time invocations to detect timer
        /// starvation when the timer system is not running
        uint32_t invocation_limit;
        /// Invocation count for \ref mdv_sw_timer_get_time invocations
        uint32_t invocation_count;
        /// Last tick count value for detecting starvation
        uint32_t last_tick_count;
} mdv_sw_timer_starvation_avereness_t;
#endif // ifndef MDV_DISABLE_SW_TIMER_STARVATION_AVERENESS

/**
 * \brief Timer data
 */
typedef struct _mdv_sw_timer_t{
        /// \brief Timer base on which this timer runs
        mdv_sw_timer_base_t *sw_timer_base;
        /// \brief Sample from the tick counter when the timer is started
        uint32_t tick_count_startup_sample;
        /// Time tick duration (in nanoseconds), inherited from the timer base
        uint32_t tick_duration_us;
        /// Timer counter mask, inherited from the timer base
        uint32_t timer_mask;
#ifndef MDV_DISABLE_SW_TIMER_STARVATION_AVERENESS
        /// Timer starvation avereness support
        mdv_sw_timer_starvation_avereness_t starvation_avereness;
#endif // ifndef MDV_DISABLE_SW_TIMER_STARVATION_AVERENESS
} mdv_sw_timer_t;

#ifdef __cplusplus
extern "C"{
#endif // ifdef __cplusplus

/**
 * \brief Initialize a timer
 *
 * \param[in] timer Timer to initialize
 * \param[in] sw_timer_base Timer base which this timer will use
 *
 * \return No return value
 */
void mdv_sw_timer_init(mdv_sw_timer_t *const sw_timer,
        mdv_sw_timer_base_t *const sw_timer_base);

#ifndef MDV_DISABLE_SW_TIMER_STARVATION_AVERENESS
/**
 * \brief Set invocation limit for \ref timer_get_time function
 *
 * \param[in] timer Timer to configure
 * \param[in] invocation_limit Invocation limit (set to 0 to disable the
 *      starvation check)
 *
 * \return No return value
 */
void mdv_sw_timer_set_invocation_limit(
        mdv_sw_timer_t *const sw_timer, uint32_t const invocation_limit);
#endif // ifndef MDV_DISABLE_SW_TIMER_STARVATION_AVERENESS

/**
 * \brief Start a timer
 *
 * \param[in] timer Timer to start
 *
 * \return No return value
 */
void mdv_sw_timer_start(mdv_sw_timer_t *const sw_timer);

/**
 * \brief Get the time elapsed from the timer start
 *
 * \param[in] timer Timer in use
 * \param[in] order_of_magnitude The order of magnitude of time to use
 * \param[out] time Elapsed time in the given order of magnitude
 */
void mdv_sw_timer_get_time(
        mdv_sw_timer_t *const sw_timer,
        mdv_sw_timer_order_of_magnitude_t order_of_magnitude,
        uint32_t *const time);

#ifdef __cplusplus
}
#endif // ifdef __cplusplus

/** @} mdv-sw-timer */

#endif // ifndef MDV_SW_TIMER_H

/* EOF */
