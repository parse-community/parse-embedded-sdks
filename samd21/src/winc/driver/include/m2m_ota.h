/**
 *
 * \file
 *
 * \brief NMC1500 IoT OTA Interface.
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

#ifndef __M2M_OTA_H__
#define __M2M_OTA_H__

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#include "common/include/nm_common.h"
#include "driver/include/m2m_types.h"

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
MACROS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
DATA TYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/




/*!
@typedef typedef void (*tpfOtaUpdateCb) (uint8 u8OtaUpdateStatusType ,uint8 u8OtaUpdateStatus);

@brief A callback to get OTA status update.
 
@param[in] u8OtaUpdateStatusType possible values are listed in tenuOtaUpdateStatusType. Possible types are:
- [DL_STATUS](@ref DL_STATUS)
- [SW_STATUS](@ref SW_STATUS)
- [RB_STATUS](@ref RB_STATUS)

@param[in] u8OtaUpdateStatus possible values are listed in tenuOtaUpdateStatus. 

@sa
	tenuOtaUpdateStatusType
	tenuOtaUpdateStatus
 */
typedef void (*tpfOtaUpdateCb) (uint8 u8OtaUpdateStatusType ,uint8 u8OtaUpdateStatus);

/*!
@typedef typedef void (*tpfOtaNotifCb) (tstrOtaUpdateInfo *);

@brief A callback to get notification about an potential OTA update.
 
@param[in] pstrOtaUpdateInfo A structure to provide notification payload.

*/
typedef void (*tpfOtaNotifCb) (tstrOtaUpdateInfo * pstrOtaUpdateInfo);

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#ifdef __cplusplus
     extern "C" {
#endif


/*!
@fn	\
	NMI_API sint8  m2m_ota_init(tpfOtaUpdateCb * pfOtaUpdateCb,tpfOtaNotifCb * pfOtaNotifCb);

@brief
	Initialize the OTA layer.

@param [in]	pfOtaUpdateCb
				OTA Update callback function
				
@param [in]	pfOtaNotifCb
				OTA notify callback function 

@return		
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8  m2m_ota_init(tpfOtaUpdateCb  pfOtaUpdateCb,tpfOtaNotifCb  pfOtaNotifCb);
/*!
@fn	\
	NMI_API sint8  m2m_ota_notif_set_url(uint8 * u8Url);

@brief
	Set the OTA url

@param [in]	u8Url
			 The url server address 

@return		
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8  m2m_ota_notif_set_url(uint8 * u8Url);

/*!
@fn	\
	NMI_API sint8  m2m_ota_notif_check_for_update(void);

@brief
	check for ota update

@return		
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8  m2m_ota_notif_check_for_update(void);

/*!
@fn	\
	NMI_API sint8 m2m_ota_notif_sched(uint32 u32Period);

@brief
	Schedule OTA update

@param [in]	u32Period
	Period in days 

@return		
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8 m2m_ota_notif_sched(uint32 u32Period);

/*!
@fn	\
	NMI_API sint8 m2m_ota_start_update(uint8 * u8DownloadUrl);

@brief
	Request OTA start update using the downloaded url 

@param [in]	u8DownloadUrl
		The download firmware url, you get it from device info
				
@return		
	The function SHALL return 0 for success and a negative value otherwise.

*/
NMI_API sint8 m2m_ota_start_update(uint8 * u8DownloadUrl);


/*!
@fn	\
	NMI_API sint8 m2m_ota_rollback(void);

@brief
	Request OTA Rollback image 

@return		
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8 m2m_ota_rollback(void);


/*!
@fn	\
	NMI_API sint8 m2m_ota_switch_firmware(void);

@brief
	Switch to the upgraded Firmware 

@return		
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8 m2m_ota_switch_firmware(void);

NMI_API sint8 m2m_ota_test(void);

#ifdef __cplusplus
}
#endif
#endif /* __M2M_OTA_H__ */
