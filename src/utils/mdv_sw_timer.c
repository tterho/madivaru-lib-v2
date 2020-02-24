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

#include "mdv_sw_timer.h"
#include "assert.h"
#include <string.h>

/**
 * \defgroup mdv-sw-timer-internals Internals
 * \ingroup  mdv-sw-timer
 * @{
 */

 #ifndef MDV_DISABLE_SW_TIMER_STARVATION_AVERENESS

/**
 * \brief Initialize the timer starvation detection
 *
 * \param[in] starvation_avereness Starvation avereness data
 * \param[in] initial_tick_count Initial tick count
 *
 * \return No return value
 */
static void _local_init_starvation_detection(
        mdv_sw_timer_starvation_avereness_t *const starvation_avereness,
        uint32_t const initial_tick_count)
{
        starvation_avereness->last_tick_count = initial_tick_count;
}

/**
 * \brief Check if starvation detection is enabled
 *
 * \param[in] starvation_avereness Starvation avereness data
 *
 * \retval true Starvation detection is enabled
 * \retval false Starvation detection is disabled
 */
static bool _local_is_starvation_detection_enabled(
        mdv_sw_timer_starvation_avereness_t *const starvation_avereness)
{
        // Starvation detection is enabled, if the invocation limit has been set
        // to a non-zero value.
        return !!(starvation_avereness->invocation_limit);
}

/**
 * \brief Manage invocation count for starvation detection
 *
 * \param[in] starvation_avereness Starvation avereness data
 * \param[in] tick_count Current tick count
 *
 * \return No return value
 */
static void _local_manage_invocation_count(
        mdv_sw_timer_starvation_avereness_t *const starvation_avereness,
        uint32_t const tick_count)
{
        // If the timer tick count increases between the invocations, the
        // timer base is alive and there is no starvation. Otherwise, the
        // starvation avereness system counts the time how long the timer base
        // is not responding.
        if ((starvation_avereness->last_tick_count == tick_count)) {
                ++starvation_avereness->invocation_count;
        }else{
                starvation_avereness->invocation_count = 0;
        }

        // Keep the last tick count on track.
        starvation_avereness->last_tick_count = tick_count;
}

/**
 * \brief Check if timer is starving
 *
 * \param[in] starvation_avereness Starvation avereness data
 * \param[in] tick_count Tick count from the timer base
 *
 * \retval true Timer is starving
 * \retval false Timer is not starving
 */
static bool _local_is_timer_starving(
        mdv_sw_timer_starvation_avereness_t *const starvation_avereness,
        uint32_t const tick_count)
{
        // If the starvation detection is disabled, the timer can't be starving.
        if (!_local_is_starvation_detection_enabled(starvation_avereness)) {
                return false;
        }

        _local_manage_invocation_count(starvation_avereness, tick_count);

        // Check if the invocation count exceeds the invocation limit, and
        // return the result as a boolean value
        return (starvation_avereness->invocation_count >
                starvation_avereness->invocation_limit);
}

#endif // MDV_DISABLE_SW_TIMER_STARVATION_AVERENESS

/**
 * \brief Manage and get tick count in count wrap-around situations
 *
 * \param[in] timer_mask Timer mask for the timer width
 * \param[in] tick_count_startup_sample Tick count startup value
 * \param[in] tick_count Current tick count
 *
 * \return Managed tick count
 */
static uint32_t _local_get_managed_tick_count(uint32_t const timer_mask,
        uint32_t const tick_count_startup_sample, uint32_t const tick_count)
{
        return (tick_count_startup_sample <= tick_count) ?
                (tick_count - tick_count_startup_sample) :
                ((timer_mask - tick_count_startup_sample) + tick_count + 1);
}

/**
 * \brief Get time for the tick count
 *
 * \param[in] tick_duration_us Duration of one tick in microseconds
 * \param[in] tick_count Tick count
 * \param[in] order_of_magnitude
 *
 * \return Time in the given order of magnitude
 */
static uint32_t _local_get_time_for_tick_count(uint32_t const tick_duration_us,
        uint32_t const tick_count,
        mdv_sw_timer_order_of_magnitude_t const order_of_magnitude)
{
        // Calculate the time lapse based on the time base and the requested
        // order of magnitude.
        switch (order_of_magnitude) {
        case MDV_SW_TIMER_US:
                // Calculate time in microseconds
                return (tick_count * tick_duration_us);

        case MDV_SW_TIMER_MS:
                // Calculate time in milliseconds
                return (tick_count * tick_duration_us) / 1000;

        case MDV_SW_TIMER_S:
                // Calculate time in seconds
                return (tick_count * tick_duration_us) / 1000000;

        case MDV_SW_TIMER_TIMERTICK:
        default:
                // The time value is in correct order of magnitude, or the order
                // of magnitude is unknown
                return tick_count;
        }
}

/** @} mdv-sw-timer-internals */

void mdv_sw_timer_init(mdv_sw_timer_t *const sw_timer,
        mdv_sw_timer_base_t *const sw_timer_base)
{
        assert(sw_timer);
        assert(sw_timer_base);

        // Reset all timer data (this resets also the starvation averness if
        // that feature is enabled)
        memset(sw_timer, 0, sizeof(mdv_sw_timer_t));
        // Link the timer with the timer base
        sw_timer->sw_timer_base = sw_timer_base;
        // Get the tick duration from the timer base
        sw_timer->tick_duration_us =
                mdv_sw_timer_base_get_tick_duration_us(sw_timer_base);
        // Get the timer mask from the timer base
        sw_timer->timer_mask =
                mdv_sw_timer_base_get_timer_mask(sw_timer_base);
}

#ifndef MDV_DISABLE_SW_TIMER_STARVATION_AVERENESS
void mdv_sw_timer_set_invocation_limit(
        mdv_sw_timer_t *const sw_timer, uint32_t const invocation_limit)
{
        assert(sw_timer);

        sw_timer->starvation_avereness.invocation_limit = invocation_limit;
}
#endif // MDV_DISABLE_SW_TIMER_STARVATION_AVERENESS

void mdv_sw_timer_start(mdv_sw_timer_t *const sw_timer)
{
        assert(sw_timer);

        sw_timer->tick_count_startup_sample =
                mdv_sw_timer_base_get_tick_count(sw_timer->sw_timer_base);

#ifndef MDV_DISABLE_SW_TIMER_STARVATION_AVERENESS
        init_starvation_detector(&(sw_timer->starvation_avereness),
                sw_timer->tick_count_startup_sample);
#endif // MDV_DISABLE_SW_TIMER_STARVATION_AVERENESS
}

uint32_t mdv_sw_timer_get_time(
        mdv_sw_timer_t *const sw_timer,
        mdv_sw_timer_order_of_magnitude_t order_of_magnitude
#ifndef MDV_DISABLE_SW_TIMER_STARVATION_AVERENESS
        , bool *const is_timer_starving
#endif // MDV_DISABLE_SW_TIMER_STARVATION_AVERENESS
        )
{
        assert(sw_timer);
        assert(is_timer_starving);

        uint32_t tick_count;

        // Get the current tick count
        tick_count = mdv_sw_timer_base_get_tick_count(sw_timer->sw_timer_base);

        // Manage tick count wrap-around situation
        tick_count = _local_get_managed_tick_count(sw_timer->timer_mask,
                sw_timer->tick_count_startup_sample, tick_count);

#ifndef MDV_DISABLE_SW_TIMER_STARVATION_AVERENESS
        *is_timer_starving =
                _local_is_timer_starving(&(sw_timer->starvation_avereness),
                                         tick_count);
        if (*is_timer_starving) {
                return 0;
        }
#endif // MDV_DISABLE_SW_TIMER_STARVATION_AVERENESS

        return _local_get_time_for_tick_count(sw_timer->tick_duration_us,
                                              tick_count, order_of_magnitude);
}

/* EOF */
