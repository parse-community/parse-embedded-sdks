/**
 *
 * \file
 *
 * \brief This module contains SAMD21 BSP APIs implementation.
 *
 * Copyright (c) 2014 Atmel Corporation. All rights reserved.
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

#include "bsp/include/nm_bsp.h"
#include "common/include/nm_common.h"
#include "asf.h"

static tpfNmBspIsr gpfIsr;

static void chip_isr(void)
{
	if (gpfIsr) {
		gpfIsr();
	}
}

/*
 *	@fn		init_chip_pins
 *	@brief	Initialize reset, chip enable and wake pin
 *	@author	M.S.M
 *	@date	11 July 2012
 *	@version	1.0
 */
static void init_chip_pins(void)
{
	struct port_config pin_conf;

	port_get_config_defaults(&pin_conf);

	/* Configure control pins as output. */
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(CONF_WIFI_M2M_RESET_PIN, &pin_conf);
	port_pin_set_config(CONF_WIFI_M2M_CHIP_ENABLE_PIN, &pin_conf);
	port_pin_set_config(CONF_WIFI_M2M_WAKE_PIN, &pin_conf);
}

/*
 *	@fn		nm_bsp_init
 *	@brief	Initialize BSP
 *	@return	0 in case of success and -1 in case of failure
 *	@author	M.S.M
 *	@date	11 July 2012
 *	@version	1.0
 */
sint8 nm_bsp_init(void)
{
	gpfIsr = NULL;

	init_chip_pins();

	/* Initialize the delay driver. */
	delay_init();

	nm_bsp_reset();

	system_interrupt_enable_global();

	return M2M_SUCCESS;
}

/**
 *	@fn		nm_bsp_deinit
 *	@brief	De-iInitialize BSP
 *	@return	0 in case of success and -1 in case of failure
 *	@author	M. Abdelmawla
 *	@date	11 July 2012
 *	@version	1.0
 */
sint8 nm_bsp_deinit(void)
{
	return M2M_SUCCESS;
}

/**
 *	@fn		nm_bsp_reset
 *	@brief	Reset NMC1500 SoC by setting CHIP_EN and RESET_N signals low,
 *           CHIP_EN high then RESET_N high
 *	@author	M. Abdelmawla
 *	@date	11 July 2012
 *	@version	1.0
 */
void nm_bsp_reset(void)
{
	port_pin_set_output_level(CONF_WIFI_M2M_CHIP_ENABLE_PIN, false);
	port_pin_set_output_level(CONF_WIFI_M2M_RESET_PIN, false);
	nm_bsp_sleep(100);
	port_pin_set_output_level(CONF_WIFI_M2M_CHIP_ENABLE_PIN, true);
	nm_bsp_sleep(10);
	port_pin_set_output_level(CONF_WIFI_M2M_RESET_PIN, true);
	nm_bsp_sleep(100);
}

/*
 *	@fn		nm_bsp_sleep
 *	@brief	Sleep in units of mSec
 *	@param[IN]	u32TimeMsec
 *				Time in milliseconds
 *	@author	M.S.M
 *	@date	28 OCT 2013
 *	@version	1.0
 */
void nm_bsp_sleep(uint32 u32TimeMsec)
{
	while (u32TimeMsec--) {
		delay_ms(1);
	}
}

/*
 *	@fn		nm_bsp_register_isr
 *	@brief	Register interrupt service routine
 *	@param[IN]	pfIsr
 *				Pointer to ISR handler
 *	@author	M.S.M
 *	@date	28 OCT 2013
 *	@sa		tpfNmBspIsr
 *	@version	1.0
 */
void nm_bsp_register_isr(tpfNmBspIsr pfIsr)
{
	struct extint_chan_conf config_extint_chan;

	gpfIsr = pfIsr;

	extint_chan_get_config_defaults(&config_extint_chan);
	config_extint_chan.gpio_pin = CONF_WIFI_M2M_INT_PIN;
	config_extint_chan.gpio_pin_mux = CONF_WIFI_M2M_INT_MUX;
	config_extint_chan.gpio_pin_pull = EXTINT_PULL_UP;
	config_extint_chan.detection_criteria = EXTINT_DETECT_FALLING;

	extint_chan_set_config(CONF_WIFI_M2M_INT_EIC, &config_extint_chan);
	extint_register_callback(chip_isr, CONF_WIFI_M2M_INT_EIC,
			EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(CONF_WIFI_M2M_INT_EIC,
			EXTINT_CALLBACK_TYPE_DETECT);
}

/*
 *	@fn		nm_bsp_interrupt_ctrl
 *	@brief	Enable/Disable interrupts
 *	@param[IN]	u8Enable
 *				'0' disable interrupts. '1' enable interrupts
 *	@author	M.S.M
 *	@date	28 OCT 2013
 *	@version	1.0
 */
void nm_bsp_interrupt_ctrl(uint8 u8Enable)
{
	if (u8Enable) {
		extint_chan_enable_callback(CONF_WIFI_M2M_INT_EIC,
				EXTINT_CALLBACK_TYPE_DETECT);
	} else {
		extint_chan_disable_callback(CONF_WIFI_M2M_INT_EIC,
				EXTINT_CALLBACK_TYPE_DETECT);
	}
}
