/**
 *
 * \file
 *
 * \brief This module contains M2M host interface APIs implementation.
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

#include "common/include/nm_common.h"
#include "driver/source/nmbus.h"
#include "bsp/include/nm_bsp.h"
#include "driver/include/m2m_wifi.h"
#include "driver/source/m2m_hif.h"


#ifdef CORTUS_APP
#define APP_MAX_NUM_REQ						30
#define APP_M2M_IP_PACKET_LIMIT				7
#define WIFI_PERIPH_BASE					0x30000000
/*!
@struct
@brief
*/
typedef struct
{
	uint8 *pu8ReqBuffer;
	uint8 u8IsUsed;
} tstrAppReqEntry;

/*!
@struct
@brief
*/
typedef struct
{
	tstrAppReqEntry astrReq[APP_MAX_NUM_REQ];
	uint8 u8ReadIdx;
	uint8 u8WriteIdx;
} tstrAppReqList;

static tstrAppReqList gstrAppReqList;
extern tstrOsSemaphore gstrAppSem;


volatile tpfHifCallBack pfWifiCb = NULL;		/*!< pointer to Wi-Fi call back function */
volatile tpfHifCallBack pfIpCb  = NULL;		/*!< pointer to Socket call back function */
volatile tpfHifCallBack pfOtaCb = NULL;		/*!< pointer to OTA call back function */
volatile tpfHifCallBack pfHifCb = NULL;


sint8 hif_init(void * arg)
{

	m2m_memset((uint8*) &gstrAppReqList, 0, sizeof(gstrAppReqList));
	return M2M_SUCCESS;
}
sint8  hif_deinit(void *arg)
{
	return M2M_SUCCESS;

}
 void hif_set_sleep_mode(uint8 u8Pstype)
{

}
uint8 hif_get_sleep_mode(void)
{
	return 0;
}

/**
*	@fn		hif_send
*	@brief	Send packet using host interface
*	@param[IN]	u8Gid
*				Group ID
*	@param[IN]	u8Opcode
*				Operation ID
*	@param[IN]	pu8Buff
*				Packet buffer. Allocated by the caller
*	@param[IN]	Packet buffer size (including the HIF header)
*	@return	M2M_SUCCESS in case of success and -ve error code in case of failure
*	@version	1.0
*/ 
sint8 hif_send(uint8 u8Gid,uint8 u8Opcode,uint8 *pu8CtrlBuf,uint16 u16CtrlBufSize,
			   uint8 *pu8DataBuf,uint16 u16DataSize, uint16 u16DataOffset)
{
	sint8	s8Ret = M2M_ERR_MEM_ALLOC;
		uint8	*pu8M2MRequest,*pu8Buffer = NULL;
		uint8 u8Data = 0;
		u8Data = u8Opcode & (NBIT7);
		u8Opcode = u8Opcode & (~NBIT7);

		if (u8Data)
		{
			if (app_get_num_free_packet_buffers() > APP_M2M_IP_PACKET_LIMIT)
			{
				pu8Buffer = app_m2m_alloc_spacket(); /* This size is fixed */
			}
			else
			{
				//M2M_ERR("Reach limits for IP TX PACKETS\n");
			}
		}
		else
		{
			pu8Buffer = app_m2m_alloc_cpacket(u16CtrlBufSize + M2M_HIF_HDR_OFFSET + u16DataOffset + u16DataSize);
		}
		if(pu8Buffer != NULL)
		{
			tstrHifHdr *pstrHifHeader = (tstrHifHdr*) pu8Buffer;

			pstrHifHeader->u8Gid = u8Gid;
			pstrHifHeader->u8Opcode = u8Opcode;
			pstrHifHeader->u16Length = 0;

			pu8M2MRequest = pu8Buffer + M2M_HIF_HDR_OFFSET;
			if ((pu8DataBuf != NULL) && (u16DataSize != 0))
			{
				pstrHifHeader->u16Length += u16DataOffset + u16DataSize;
				m2m_memcpy(pu8M2MRequest + u16DataOffset, pu8DataBuf, u16DataSize);
			}
			else
			{
				pstrHifHeader->u16Length += u16CtrlBufSize;
			}

			if ((pu8CtrlBuf != NULL) && (u16CtrlBufSize != 0))
			{
				m2m_memcpy(pu8M2MRequest, pu8CtrlBuf, u16CtrlBufSize);
			}

			if (app_post_req != NULL)
			{
				app_post_req(u8Gid, u8Opcode, pu8Buffer);
			}
			else
			{
				M2M_ERR("NULL CB\n");
			}
			s8Ret = M2M_SUCCESS;
		}
		else
		{
			//M2M_ERR("Out Of Memory\n");
		}
		return s8Ret;

}
/**
*	@fn		hif_isr
*	@brief	Host interface interrupt serviece routine
*	@param[OUT]	pstrHif
*				Pointer to structure holding packet header information
*	@param[OUT]	pu32BufAddr
*				Pointer to u32 variable used to return receive buffer address
*	@author	M. Abdelmawla
*	@date	15 July 2012
*	@return	1 in case of interrupt received else 0 will be returned
*	@version	1.0
*/ 
static sint8 hif_isr(void)
{
	uint8 u8ReqIdx = gstrAppReqList.u8ReadIdx;
	uint8 *pu8ReqBuffer = gstrAppReqList.astrReq[u8ReqIdx].pu8ReqBuffer;
	tstrHifHdr *pstrReqHdr = (tstrHifHdr*) pu8ReqBuffer;

	if (gstrAppReqList.astrReq[u8ReqIdx].u8IsUsed)
	{
		/* Reset the used flag. */
		gstrAppReqList.astrReq[u8ReqIdx].u8IsUsed = 0;

		/* Process the received request. */
		if(pstrReqHdr->u8Gid == M2M_REQ_GRP_WIFI)
		{
			if(pfWifiCb)
				pfWifiCb(pstrReqHdr->u8Opcode, pstrReqHdr->u16Length, (uint32)pu8ReqBuffer + M2M_HIF_HDR_OFFSET);
		}
		else if(pstrReqHdr->u8Gid == M2M_REQ_GRP_IP)
		{
			if(pfIpCb)
				pfIpCb(pstrReqHdr->u8Opcode, pstrReqHdr->u16Length, (uint32)pu8ReqBuffer + M2M_HIF_HDR_OFFSET);
		}

		/* Adjust the next read Idx. */
		gstrAppReqList.u8ReadIdx ++;
		if(gstrAppReqList.u8ReadIdx == APP_MAX_NUM_REQ)
			gstrAppReqList.u8ReadIdx = 0;

		/* Release the request buffer. */
		app_m2m_free_packet(pu8ReqBuffer);
	}
	return M2M_SUCCESS;
}
/**
*	@fn		hif_handle_isr(void)
*	@brief	
			Handle interrupt received from NMC1500 firmware.
*   @return     
			The function SHALL return 0 for success and a negative value otherwise.
*/

sint8 hif_handle_isr(void)
{

	return hif_isr();

}


/*
*	@fn		hif_receive
*	@brief	Host interface interrupt serviece routine
*	@param[IN]	u32Addr
*				Receive start address
*	@param[OUT]	pu8Buf
*				Pointer to receive buffer. Allocated by the caller
*	@param[IN]	u16Sz
*				Receive buffer size
*	@return	M2M_SUCCESS in case of success and -ve error code in case of failure
*	@author	M. Abdelmawla
*	@date	15 July 2012
*	@return	1 in case of interrupt received else 0 will be returned
*	@version	1.0
*/ 
sint8 hif_receive(uint32 u32Addr, uint8 *pu8Buf, uint16 u16Sz, uint8 isDone)
{
	if (pu8Buf != NULL)
	{
		m2m_memcpy(pu8Buf, (uint8*) u32Addr, u16Sz);
	}
	return M2M_SUCCESS;

}
/**
*	@fn		hif_Resp_handler(uint8 *pu8Buffer, uint16 u16BufferSize)
*	@brief  
			 	Response handler for HIF layer.
				
*	@param [in]	pu8Buffer
				Pointer to the buffer.
				
*	@param [in]	u16BufferSize
				Buffer size.
				
*   @return    
				The function SHALL return 0 for success and a negative value otherwise.
*/
sint8  hif_Resp_handler(uint8 *pu8Buffer, uint16 u16BufferSize)
{
	uint8 u8WriteIdx = gstrAppReqList.u8WriteIdx;

	M2M_DBG("m2m_callback %x %x %x\n",pu8Buffer,u16BufferSize,u8WriteIdx);
	if (gstrAppReqList.astrReq[u8WriteIdx].u8IsUsed)
		M2M_ERR("APP Req Overflow\n");

	gstrAppReqList.astrReq[u8WriteIdx].u8IsUsed = 1;
	gstrAppReqList.astrReq[u8WriteIdx].pu8ReqBuffer = pu8Buffer;

	gstrAppReqList.u8WriteIdx++;
	if (gstrAppReqList.u8WriteIdx >= APP_MAX_NUM_REQ)
		gstrAppReqList.u8WriteIdx = 0;

	app_os_sem_up(&gstrAppSem);

	return 0;
}

void hif_set_rx_done(void)
{

}

sint8 hif_read_reg(uint32 u32Addr, uint32* pu32RetVal)
{
	*pu32RetVal = *((volatile unsigned *)(u32Addr + WIFI_PERIPH_BASE));
	return 0;
}

void hif_write_reg(uint32 u32Addr, uint32 u32Val)
{
	 *((volatile unsigned *)(u32Addr + WIFI_PERIPH_BASE)) = u32Val;
}
/**
*	@fn		hif_register_cb
*	@brief	To set Call back function for every compantent Component
*	@param [in]	fn
*				function which you want to set
*   @author		Awad A. Bekhet
*   @date		23 June 2014
*	@version	1.0
*/
sint8 hif_register_cb(uint8 u8Grp,tpfHifCallBack fn)
{
	sint8 ret = M2M_SUCCESS;
	switch(u8Grp)
	{
		case M2M_REQ_GRP_IP:
			pfIpCb = fn;
			break;
		case M2M_REQ_GRP_WIFI:
			pfWifiCb = fn;
			break;
		case M2M_REQ_GRP_OTA:
			pfOtaCb = fn;
			break;
		case M2M_REQ_GRP_HIF:
			pfHifCb = fn;
			break;
		default:
			M2M_ERR("GRp ? %d\n",u8Grp);
			ret = M2M_ERR_FAIL;
			break;
	}
	return ret;
}
#endif

