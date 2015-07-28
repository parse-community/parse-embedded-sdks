/**
 * \file
 *
 * \brief AT30TSE75X Temperature Sensor driver Quick Start
 *
 * Copyright (C) 2012-2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
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
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

/**
 * \page asfdoc_sam0_at30tse75x_qs Quick Start Guide for the SAM AT30TSE75X Temperature Sensor Driver
 * In this use case we will interface the temperature sensor, set a resolution and read
 * the temperature.
 *
 * \section asfdoc_sam0_at30tse75x_qs_prereq Prerequisites
 * The temperature sensor must be connected to the device.
 *
 * \section asfdoc_sam0_at30tse75x_qs_setup Setup
 *
 * \subsection asfdoc_sam0_at30tse75x_qs_init_code Initialization Code
 * Add a global variable to store the temperature:
 * \snippet qs_at30tse75x.c temp_res
 * Copy-paste the following setup code to your application main function:
 * \snippet qs_at30tse75x.c init
 *
 * \subsection asfdoc_sam0_at30tse75x_qs_workflow Workflow
 * -# Initialize system
 *  \snippet qs_at30tse75x.c system_init
 * -# Initialize I2C interface
 *  \snippet qs_at30tse75x.c temp_init
 *
 * \section asfdoc_sam0_at30tse75x_qs_implement Implementation
 *
 * \subsection asfdoc_sam0_at30tse75x_qs_imp_code Code
 * Add after initialization in main.
 * \snippet qs_at30tse75x.c impl
 *
 * \subsection asfdoc_sam0_at30tse75x_qs_imp_workflow Workflow
 * -# Read THIGH register from temperature sensor:
 *  \snippet qs_at30tse75x.c read_thigh
 * -# Read TLOW register from temperature sensor:
 *  \snippet qs_at30tse75x.c read_tlow
 * -# Set temperature sensor resolution to 12-bit:
 *  \snippet qs_at30tse75x.c write_conf
 * -# Continuously read the temperature:
 *  \snippet qs_at30tse75x.c read_temp
 *
 */
