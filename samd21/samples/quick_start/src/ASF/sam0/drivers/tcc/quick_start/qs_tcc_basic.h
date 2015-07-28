/**
 * \file
 *
 * \brief SAM TCC Driver Quick Start
 *
 * Copyright (C) 2013-2014 Atmel Corporation. All rights reserved.
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
 * \page asfdoc_sam0_tcc_basic_use_case Quick Start Guide for TCC - Basic
 *
 * The supported board list:
 *    - SAM D21/R21 Xplained Pro
 *
 * In this use case, the TCC will be used to generate a PWM signal. Here
 * the pulse width is set to one quarter of the period.
 * When connect PWM output to LED it makes the LED light. To see the waveform,
 * you may need an oscilloscope.
 *
 * The PWM output is set up as follows:
 * <table>
 *  <tr><th> Board        </td><th> Pin  </td><th> Connect to </td></tr>
 *  <tr><td> SAMD21 Xpro  </td><td> PB30 </td><td> LED0       </td></tr>
 *  <tr><td> SAMR21 Xpro  </td><td> PA19 </td><td> LED0       </td></tr>
 * </table>
 *
 * The TCC module will be set up as follows:
 * - GCLK generator 0 (GCLK main) clock source
 * - Use double buffering write when set top, compare or pattern through API
 * - No dithering on the counter or compare
 * - No prescaler
 * - Single Slope PWM wave generation
 * - GCLK reload action
 * - Don't run in standby
 * - No fault or waveform extensions
 * - No inversion of waveform output
 * - No capture enabled
 * - Count upward
 * - Don't perform one-shot operations
 * - No event input enabled
 * - No event action
 * - No event generation enabled
 * - Counter starts on 0
 * - Counter top set to 0xFFFF
 * - Capture compare channel 0 set to 0xFFFF/4
 *
 * \section asfdoc_sam0_tcc_basic_use_case_setup Quick Start
 *
 * \subsection asfdoc_sam0_tcc_basic_use_case_prereq Prerequisites
 * There are no prerequisites for this use case.
 *
 * \subsection asfdoc_sam0_tcc_basic_use_case_setup_code Code
 *
 * Add to the main application source file, before any functions:
 * \snippet conf_quick_start.h definition_pwm
 *
 * Add to the main application source file, outside of any functions:
 * \snippet qs_tcc_basic.c module_inst
 *
 * Copy-paste the following setup code to your user application:
 * \snippet qs_tcc_basic.c setup
 *
 * Add to user application initialization (typically the start of \c main()):
 * \snippet qs_tcc_basic.c setup_init
 *
 * \subsection asfdoc_sam0_tcc_basic_use_case_setup_flow Workflow
 * -# Create a module software instance structure for the TCC module to store
 *    the TCC driver state while it is in use.
 *    \snippet qs_tcc_basic.c module_inst
 *    \note This should never go out of scope as long as the module is in use.
 *          In most cases, this should be global.
 *
 * -# Configure the TCC module.
 *  -# Create a TCC module configuration struct, which can be filled out to
 *     adjust the configuration of a physical TCC peripheral.
 *     \snippet qs_tcc_basic.c setup_config
 *  -# Initialize the TCC configuration struct with the module's default values.
 *     \snippet qs_tcc_basic.c setup_config_defaults
 *     \note This should always be performed before using the configuration
 *           struct to ensure that all values are initialized to known default
 *           settings.
 *
 *  -# Alter the TCC settings to configure the counter width, wave generation
 *     mode and the compare channel 0 value.
 *     \snippet qs_tcc_basic.c setup_change_config
 *  -# Alter the TCC settings to configure the PWM output on a physical device
 *     pin.
 *     \snippet qs_tcc_basic.c setup_change_config_pwm
 *  -# Configure the TCC module with the desired settings.
 *     \snippet qs_tcc_basic.c setup_set_config
 *  -# Enable the TCC module to start the timer and begin PWM signal generation.
 *     \snippet qs_tcc_basic.c setup_enable
 *
 *
 * \section asfdoc_sam0_tcc_basic_use_case_main Use Case
 *
 * \subsection asfdoc_sam0_tcc_basic_use_case_main_code Code
 * Copy-paste the following code to your user application:
 * \snippet qs_tcc_basic.c main
 *
 * \subsection asfdoc_sam0_tcc_basic_use_case_main_flow Workflow
 * -# Enter an infinite loop while the PWM wave is generated via the TCC module.
 *  \snippet qs_tcc_basic.c main_loop
 */