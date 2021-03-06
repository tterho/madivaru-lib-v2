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

#ifndef MDV_DIGITAL_INPUT_H
#define MDV_DIGITAL_INPUT_H

#include "mdv_common.h"

/**
 * \file       mdv_digital_input.h
 * \defgroup   mdv-digital-input Digital input API
 * \ingroup    madivaru-lib-v2
 * \copyright  Copyright &copy; 2020, Tuomas Terho. All rights reserved.
 * @{
 */

/**
 * \brief Digital input API
 */
typedef struct _mdv_digital_input_t {
        /**
         * \brief Initializes the digital input
         *
         * \return Implementation specific result of the operation
         */
        mdv_result_t (*init)(void);
        /**
         * \brief Uninitializes the digital input
         *
         * \return Implementation specific result of the operation
         */
        mdv_result_t (*uninit)(void);
        /**
         * \brief Gets the input value
         *
         * \return Implementation specific input value
         */
         uint32_t (*get)(void);
} const mdv_digital_input_t;

/** @} mdv-digital-input */

#endif // ifndef MDV_DIGITAL_INPUT_H

/* EOF */