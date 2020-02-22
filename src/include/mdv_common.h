/**
 * \defgroup  madivaru-lib-v2 Madivaru library V2
 * \copyright Copyright &copy; 2020, Tuomas Terho. All rights reserved.
 *
 * The Madivaru library is a collection of general purpose APIs, driver
 * interfaces and utilities written in C and designed for use in embedded
 * systems.
 *
 * # Licensing
 *
 * BSD 3-Clause License
 *
 * Copyright &copy; 2020, Tuomas Terho
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

#ifndef MDV_COMMON_H
#define MDV_COMMON_H

#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

/**
 * \file      mdv_common.h
 * \defgroup  mdv-common Common types and macros
 * \ingroup   madivaru-lib-v2
 * @{
 */

/**
 * Common result type
 */
typedef int32_t mdv_result_t;

/**
 * \brief A macro to test a result
 *
 * \param[in] RESULT Result to test
 *
 * \retval true Result is successful
 * \retval false Result is an error code
 */
#define MDV_SUCCESSFUL(RESULT) ((RESULT)>=0)

/// A generic successful result
#define MDV_RESULT_OK 0

/** @} */

#endif // ifndef MDV_COMMON_H

/* EOF */
