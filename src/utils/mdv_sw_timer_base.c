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

#include "mdv_sw_timer_base.h"
#include "assert.h"

/**
 * \defgroup mdv-sw-timer-base-internals Internals
 * \ingroup  mdv-sw-timer-base
 * @{
 */

/**
 * \brief Create a mask for the given amount of bits
 *
 * \param[in] bits Bit count in the mask
 *
 * \return The mask
 */
uint32_t create_mask(uint8_t bits)
{
        uint32_t mask = 0;

        for (; bits; bits--) {
                mask <<= 1u;
                mask |= 1u;
        }

        return mask;
}

/** @} mdv-sw-timer-base-internals */

void mdv_sw_timer_base_init(mdv_sw_timer_base_t *const sw_timer_base,
                            uint32_t const tick_duration_us,
                            uint8_t const timer_width_bits,
                            mdv_timer_driver_t *const timer_driver)
{
        assert(sw_timer_base);
        assert(tick_duration_us > 0);
        assert((timer_width_bits > 0) && (timer_width_bits <= 32));

        sw_timer_base->timer_mask = create_mask(timer_width_bits);
        sw_timer_base->tick_counter = 0;
        sw_timer_base->tick_duration_us = tick_duration_us;
        sw_timer_base->timer_driver = timer_driver;

        // Initialize the timer driver if needed
        if (sw_timer_base->timer_driver) {
                // Set event handler and user data to zero because they are not
                // needed by the polling mode
                sw_timer_base->timer_driver->init(0, 0);
        }
}

void mdv_sw_timer_base_uninit(mdv_sw_timer_base_t *const sw_timer_base)
{
        assert(sw_timer_base);

        // Uninitialize the timer driver if needed
        if (sw_timer_base->timer_driver) {
                sw_timer_base->timer_driver->uninit();
        }
}

void mdv_sw_timer_base_tick(mdv_sw_timer_base_t *const sw_timer_base,
                            uint32_t const tick_count)
{
        assert(sw_timer_base);
        assert(tick_count);

        // Advance the tick counter
        sw_timer_base->tick_counter += tick_count;
        // Emulate the width of the timer by limiting the tick counter by the
        // timer mask
        sw_timer_base->tick_counter &= sw_timer_base->timer_mask;
}

uint32_t mdv_sw_timer_base_get_tick_count(mdv_sw_timer_base_t *sw_timer_base)
{
        assert(sw_timer_base);

        // If the timer driver has been set, use the direct hardware polling
        // mode (return the hardware counter value). Otherwise, return the local
        // tick counter.
        if (sw_timer_base->timer_driver) {
                return sw_timer_base->timer_driver->get_count();
        } else {
                return sw_timer_base->tick_counter;
        }
}

uint32_t mdv_sw_timer_base_get_tick_duration_us(
        mdv_sw_timer_base_t *const sw_timer_base)
{
        assert(sw_timer_base);

        return sw_timer_base->tick_duration_us;
}

uint32_t mdv_sw_timer_base_get_timer_mask(
        mdv_sw_timer_base_t *const sw_timer_base)
{
        assert(sw_timer_base);

        return sw_timer_base->timer_mask;
}

/* EOF */