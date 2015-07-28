/**
 *
 * \file
 *
 * \brief This module contains NMC1500 BSP APIs declarations.
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

#ifndef _NM_BSP_H_
#define _NM_BSP_H_

#define NMI_API

typedef unsigned char	uint8;
typedef unsigned short	uint16;
typedef unsigned long	uint32;
typedef signed char		sint8;
typedef signed short	sint16;
typedef signed long		sint32;

#ifdef __MCF964548__
#define CONST code
#else
#define CONST const
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif
typedef void (*tpfNmBspIsr)(void);
#ifndef NULL
#define NULL ((void*)0)
#endif

#define BSP_MIN(x,y) ((x)>(y)?(y):(x))

#ifndef CORTUS_APP


#ifdef __cplusplus
extern "C"{
#endif
/**
*	@fn		nm_bsp_init
*	@brief	Initialize BSP
*	@return	0 in case of success and -1 in case of failure
*/ 
sint8 nm_bsp_init(void);

/**
*	@fn		nm_bsp_deinit
*	@brief	De-iInitialize BSP
*	@return	0 in case of success and -1 in case of failure
*/ 
sint8 nm_bsp_deinit(void);

/**
*	@fn		nm_bsp_deinit
*	@brief	Reset NMC1500 SoC by setting CHIP_EN and RESET_N signals low, 
*           CHIP_EN high then RESET_N high
*/ 
void nm_bsp_reset(void);

/**
*	@fn		nm_bsp_sleep
*	@brief	Sleep in units of mSec
*	@param [in]	u32TimeMsec
*				Time in milliseconds
*/ 
void nm_bsp_sleep(uint32 u32TimeMsec);
/**
*	@fn		nm_bsp_register_isr
*	@brief	Register interrupt service routine
*	@param [in]	pfIsr
*				Pointer to ISR handler
*	@sa		tpfNmBspIsr
*/
void nm_bsp_register_isr(tpfNmBspIsr pfIsr);

/**
*	@fn		nm_bsp_interrupt_ctrl
*	@brief	Enable/Disable interrupts
*	@param [in]	u8Enable
*				'0' disable interrupts. '1' enable interrupts
*/
void nm_bsp_interrupt_ctrl(uint8 u8Enable);


#ifdef __cplusplus
}
#endif

#endif

#ifdef WIN32
#include "nm_bsp_win32.h"
#endif

#ifdef __K20D50M__
#include "nm_bsp_k20d50m.h"
#endif

#ifdef __MSP430FR5739__
#include "bsp_msp430fr5739.h"
#endif

#ifdef _FREESCALE_MCF51CN128_
#include "bsp/include/nm_bsp_mcf51cn128.h"
#endif

#ifdef __MCF964548__
#include "bsp/include/nm_bsp_mc96f4548.h"
#endif

#ifdef __APP_APS3_CORTUS__
#include "nm_bsp_aps3_cortus.h"
#endif

#if (defined __SAMD21J18A__) || (defined __SAMD21G18A__)
#include "bsp/include/nm_bsp_samd21.h"
#endif


#ifdef __SAM4S16C__
#include "bsp/include/nm_bsp_sam4s.h"
#endif

#ifdef __SAM4SD32C__
#include "bsp/include/nm_bsp_sam4s.h"
#endif

#ifdef CORTUS_APP
#include "crt_iface.h"
#endif

#ifdef NRF51 
#include "nm_bsp_nrf51822.h"
#endif

#ifdef _ARDUINO_UNO_
#include <bsp/include/nm_bsp_arduino_uno.h>
#endif


#ifdef _NM_BSP_BIG_END
#define NM_BSP_B_L_32(x) \
((((x) & 0x000000FF) << 24) + \
(((x) & 0x0000FF00) << 8)  + \
(((x) & 0x00FF0000) >> 8)   + \
(((x) & 0xFF000000) >> 24))
#define NM_BSP_B_L_16(x) \
((((x) & 0x00FF) << 8) + \
(((x)  & 0xFF00) >> 8))
#else
#define NM_BSP_B_L_32(x)  (x)
#define NM_BSP_B_L_16(x)  (x)
#endif


#endif	/*_NM_BSP_H_*/
