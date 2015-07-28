/**
 * \file
 *
 * \brief SAM RTC Calendar Quick Start
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
 * \page asfdoc_sam0_rtc_calendar_basic_use_case Quick Start Guide for RTC (CAL) - Basic
 * In this use case, the RTC is set up in calendar mode. The time is set and
 * also a alarm is set to show a general use of the RTC in calendar mode. Also
 * the clock is swapped from 24h to 12h mode after initialization. The board LED
 * will be toggled once the current time matches the set time.
 *
 * \section asfdoc_sam0_rtc_calendar_basic_use_case_prereq Prerequisites
 * The Generic Clock Generator for the RTC should be configured and enabled; if
 * you are using the System Clock driver, this may be done via \c conf_clocks.h.
 *
 * \subsection asfdoc_sam0_rtc_calendar_basic_use_case_setup_clocks Clocks and Oscillators
 * The \c conf_clock.h file needs to be changed with the following values to
 * configure the clocks and oscillators for the module.
 *
 * The following oscillator settings are needed:
 * \snippet conf_clocks.h oscillator_settings
 * The following generic clock settings are needed:
 * \snippet conf_clocks.h gclk_settings
 *
 * \section asfdoc_sam0_rtc_calendar_basic_use_case_setup Setup
 *
 * \subsection asfdoc_sam0_rtc_calendar_basic_use_case_init_code Initialization Code
 * Create a rtc_module struct and add to the main application source file,
 * outside of any functions:
 * \snippet qs_rtc_calendar_basic.c rtc_module_instance
 *
 * Copy-paste the following setup code to your application:
 * \snippet qs_rtc_calendar_basic.c initiate
 *
 * \subsection asfdoc_sam0_rtc_calendar_basic_use_case_main_code Add to Main
 * Add the following to \c main().
 * \snippet qs_rtc_calendar_basic.c add_main
 *
 * \subsection asfdoc_sam0_rtc_calendar_basic_use_case_cal_basic_use_workflow Workflow
 * -# Make configuration structure.
 *    \snippet qs_rtc_calendar_basic.c set_conf
 * -# Fill the configuration structure with the default driver configuration.
 *    \snippet qs_rtc_calendar_basic.c get_default
 *    \note This should always be performed before using the configuration
 *          struct to ensure that all values are initialized to known default
 *          settings.
 *
 * -# Make time structure for alarm and set with default and desired values.
 *    \snippet qs_rtc_calendar_basic.c time_struct
 * -# Change configurations as desired.
 *    \snippet qs_rtc_calendar_basic.c set_config
 * -# Initialize module.
 *    \snippet qs_rtc_calendar_basic.c init_rtc
 * -# Enable module
 *    \snippet qs_rtc_calendar_basic.c enable
 *
 * \section asfdoc_sam0_rtc_calendar_basic_use_case_count_basic_implement Implementation
 * Add the following to \c main().
 * \snippet qs_rtc_calendar_basic.c main_imp
 *
 * \subsection asfdoc_sam0_rtc_calendar_basic_use_case_count_basic_workflow Workflow
 * -# Start an infinite loop, to continuously poll for a RTC alarm match.
 *    \snippet qs_rtc_calendar_basic.c main_loop
 * -# Check to see if a RTC alarm match has occurred.
 *    \snippet qs_rtc_calendar_basic.c check_alarm_match
 * -# Once an alarm match occurs, perform the desired user action.
 *    \snippet qs_rtc_calendar_basic.c alarm_match_action
 * -# Clear the alarm match, so that future alarms may occur.
 *    \snippet qs_rtc_calendar_basic.c clear_alarm_match
 */
