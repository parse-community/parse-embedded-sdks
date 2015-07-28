/**
 * \file
 *
 * \brief SAM TCC - Timer Counter for Control Applications Driver Quick Start
 *
 * Copyright (C) 2014 Atmel Corporation. All rights reserved.
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
 * \page asfdoc_sam0_tcc_timer_use_case Quick Start Guide for TCC - Timer
 *
 * The supported board list:
 *    - SAM D21/R21 Xplained Pro
 *    - SAM D11 Xplained Pro
 *
 * In this use case, the TCC will be used as a timer, to generate overflow and
 * compare match callbacks. In the callbacks the on-board LED is toggled.
 *
 * The TCC module will be set up as follows:
 * - GCLK generator 1 (GCLK 32K) clock source
 * - Use double buffering write when set top, compare or pattern through API
 * - No dithering on the counter or compare
 * - Prescaler is divided by 64
 * - GCLK reload action
 * - Count upward
 * - Don't run in standby
 * - No waveform outputs
 * - No capture enabled
 * - Don't perform one-shot operations
 * - No event input enabled
 * - No event action
 * - No event generation enabled
 * - Counter starts on 0
 * - Counter top set to 2000 (about 4s) and generate overflow callback
 * - Channel 0 is set to compare and match value 900 and generate callback
 * - Channel 1 is set to compare and match value 930 and generate callback
 * - Channel 2 is set to compare and match value 1100 and generate callback
 * - Channel 3 is set to compare and match value 1250 and generate callback
 *
 * \section asfdoc_sam0_tcc_timer_use_case_setup Quick Start
 *
 * \subsection asfdoc_sam0_tcc_timer_use_case_prereq Prerequisites
 * For this use case, XOSC32K should be enabled and available through GCLK
 * generator 1 clock source selection. Within Atmel Software Framework (ASF)
 * it can be done through modifying <i>conf_clocks.h</i>.
 * See \ref asfdoc_sam0_system_clock_group "System Clock Management Driver" for
 * more details about clock configuration.
 *
 * \subsection asfdoc_sam0_tcc_timer_use_case_setup_code Code
 *
 * Add to the main application source file, outside of any functions:
 * \snippet qs_tcc_timer.c module_inst
 *
 * Copy-paste the following callback function code to your user application:
 * \snippet qs_tcc_timer.c callback_funcs
 *
 * Copy-paste the following setup code to your user application:
 * \snippet qs_tcc_timer.c setup
 *
 * Add to user application initialization (typically the start of \c main()):
 * \snippet qs_tcc_timer.c setup_init
 *
 * \subsection asfdoc_sam0_tcc_timer_use_case_setup_flow Workflow
 * -# Create a module software instance structure for the TCC module to store
 *    the TCC driver state while it is in use.
 *    \snippet qs_tcc_timer.c module_inst
 *    \note This should never go out of scope as long as the module is in use.
 *          In most cases, this should be global.
 *
 * -# Configure the TCC module.
 *  -# Create a TCC module configuration struct, which can be filled out to
 *     adjust the configuration of a physical TCC peripheral.
 *     \snippet qs_tcc_timer.c setup_config
 *  -# Initialize the TCC configuration struct with the module's default values.
 *     \snippet qs_tcc_timer.c setup_config_defaults
 *     \note This should always be performed before using the configuration
 *           struct to ensure that all values are initialized to known default
 *           settings.
 *
 *  -# Alter the TCC settings to configure the GCLK source, prescaler, period
 *     and compare channel values.
 *     \snippet qs_tcc_timer.c setup_change_config
 *  -# Configure the TCC module with the desired settings.
 *     \snippet qs_tcc_timer.c setup_set_config
 *  -# Enable the TCC module to start the timer.
 *     \snippet qs_tcc_timer.c setup_enable
 * -# Configure the TCC callbacks.
 *  -# Register the Overflow and Compare Channel Match callback functions with
 *     the driver.
 *     \snippet qs_tcc_timer.c setup_register_callback
 *  -# Enable the Overflow and Compare Channel Match callbacks so that it will
 *     be called by the driver when appropriate.
 *     \snippet qs_tcc_timer.c setup_enable_callback
 *
 *
 * \section asfdoc_sam0_tcc_timer_use_case_main Use Case
 *
 * \subsection asfdoc_sam0_tcc_timer_use_case_main_code Code
 * Copy-paste the following code to your user application:
 * \snippet qs_tcc_timer.c main
 *
 * \subsection asfdoc_sam0_tcc_timer_use_case_main_flow Workflow
 * -# Enter an infinite loop while the timer is running.
 *  \snippet qs_tcc_timer.c main_loop
 */


#include <asf.h>
#include <conf_clocks.h>