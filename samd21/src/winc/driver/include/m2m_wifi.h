/**
 *
 * \file
 *
 * \brief NMC1500 IoT Application Interface.
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

#ifndef __M2M_WIFI_H__
#define __M2M_WIFI_H__

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

#ifdef CONF_MGMT

/*!
@enum	\
	tenuWifiFrameType

@brief
	Basic Wlan Frame Type Codes (2-bit) 
*/
typedef enum {
	MANAGEMENT            = 0x00,
	/*!< Wi-Fi Management frame (Probe Req/Resp, Beacon, Association Req/Resp ...etc).
	*/
	CONTROL               = 0x04,
	/*!< Wi-Fi Control frame (eg. ACK frame).
	*/
	DATA_BASICTYPE        = 0x08,
	/*!< Wi-Fi Data frame.
	*/
	RESERVED              = 0x0C
}tenuWifiFrameType;


/*!
@enum	\
	tenuSubTypes

@brief
	Wi-Fi MAC Frame Sub-Types (6-bit) 
*/
typedef enum {
	/* Sub-Types related to ManagementSub-Types */
	ASSOC_REQ             = 0x00,
	ASSOC_RSP             = 0x10,
	REASSOC_REQ           = 0x20,
	REASSOC_RSP           = 0x30,
	PROBE_REQ             = 0x40,
	PROBE_RSP             = 0x50,
	BEACON                = 0x80,
	ATIM                  = 0x90,
	DISASOC               = 0xA0,
	AUTH                  = 0xB0,
	DEAUTH                = 0xC0,
	ACTION                = 0xD0,
	/* Sub-Types related to Control */
	PS_POLL               = 0xA4,
	RTS                   = 0xB4,
	CTS                   = 0xC4,
	ACK                   = 0xD4,
	CFEND                 = 0xE4,
	CFEND_ACK             = 0xF4,
	BLOCKACK_REQ          = 0x84,
	BLOCKACK              = 0x94,
	/* Sub-Types related to Data */
	DATA                  = 0x08,
	DATA_ACK              = 0x18,
	DATA_POLL             = 0x28,
	DATA_POLL_ACK         = 0x38,
	NULL_FRAME            = 0x48,
	CFACK                 = 0x58,
	CFPOLL                = 0x68,
	CFPOLL_ACK            = 0x78,
	QOS_DATA              = 0x88,
	QOS_DATA_ACK          = 0x98,
	QOS_DATA_POLL         = 0xA8,
	QOS_DATA_POLL_ACK     = 0xB8,
	QOS_NULL_FRAME        = 0xC8,
	QOS_CFPOLL            = 0xE8,
	QOS_CFPOLL_ACK        = 0xF8
}tenuSubTypes;


/*!
@enum	\
	tenuInfoElementId

@brief
	Wi-Fi Information Element IDs
*/
typedef enum {
	ISSID               = 0,   
	/*!< Service Set Identifier (SSID)
	*/
	ISUPRATES           = 1,   
	/*!< Supported Rates
	*/
	IFHPARMS            = 2,   
	/*!< FH parameter set
	*/
	IDSPARMS            = 3,   
	/*!< DS parameter set
	*/
	ICFPARMS            = 4,   
	/*!< CF parameter set
	*/
	ITIM                = 5,   
	/*!< Traffic Information Map
	*/
	IIBPARMS            = 6,   
	/*!< IBSS parameter set
	*/
	ICOUNTRY            = 7,   
	/*!< Country element.
	*/
	IEDCAPARAMS         = 12,  
	/*!< EDCA parameter set
	*/
	ITSPEC              = 13,  
	/*!< Traffic Specification
	*/
	ITCLAS              = 14,  
	/*!< Traffic Classification
	*/
	ISCHED              = 15,  
	/*!< Schedule.
	*/
	ICTEXT              = 16,  
	/*!< Challenge Text
	*/
	IPOWERCONSTRAINT    = 32,  
	/*!< Power Constraint.
	*/
	IPOWERCAPABILITY    = 33,  
	/*!< Power Capability
	*/
	ITPCREQUEST         = 34,  
	/*!< TPC Request                    
	*/
	ITPCREPORT          = 35,  
	/*!< TPC Report                     
	*/
	ISUPCHANNEL         = 36, 
	/* Supported channel list  
	*/
	ICHSWANNOUNC        = 37,  
	/*!< Channel Switch Announcement    
	*/
	IMEASUREMENTREQUEST = 38,  
	/*!< Measurement request            
	*/
	IMEASUREMENTREPORT  = 39,  
	/*!< Measurement report             
	*/
	IQUIET              = 40,  
	/*!< Quiet element Info             
	*/
	IIBSSDFS            = 41,  
	/*!< IBSS DFS                       
	*/
	IERPINFO            = 42,  
	/*!< ERP Information                
	*/
	ITSDELAY            = 43,  
	/*!< TS Delay                       
	*/
	ITCLASPROCESS       = 44,  
	/*!< TCLAS Processing               
	*/
	IHTCAP              = 45,  
	/*!< HT Capabilities                
	*/
	IQOSCAP             = 46,  
	/*!< QoS Capability                 
	*/
	IRSNELEMENT         = 48,  
	/*!< RSN Information Element        
	*/
	IEXSUPRATES         = 50,  
	/*!< Extended Supported Rates       
	*/
	IEXCHSWANNOUNC      = 60,  
	/*!< Extended Ch Switch Announcement
	*/
	IHTOPERATION        = 61,  
	/*!< HT Information                 
	*/
	ISECCHOFF           = 62,  
	/*!< Secondary Channel Offset      
	*/
	I2040COEX           = 72,  
	/*!< 20/40 Coexistence IE           
	*/
	I2040INTOLCHREPORT  = 73,  
	/*!< 20/40 Intolerant channel report
	*/
	IOBSSSCAN           = 74,  
	/*!< OBSS Scan parameters           
	*/
	IEXTCAP             = 127, 
	/*!< Extended capability            
	*/
	IWMM                = 221, 
	/*!< WMM parameters                 
	*/
	IWPAELEMENT         = 221 
	/*!< WPA Information Element        
	*/
}tenuInfoElementId;


/*!
@struct	\
	tenuWifiCapability

@brief
	Capability Information field bit assignments.  
*/
typedef enum{
	ESS            = 0x01,   
	/*!< ESS capability               
	*/
	IBSS           = 0x02,   
	/*!< IBSS mode                    
	*/
	POLLABLE       = 0x04,   
	/*!< CF Pollable                  
	*/
	POLLREQ        = 0x08,   
	/*!< Request to be polled         
	*/
	PRIVACY        = 0x10,   
	/*!< WEP encryption supported     
	*/
	SHORTPREAMBLE  = 0x20,   
	/*!< Short Preamble is supported  
	*/
	SHORTSLOT      = 0x400,  
	/*!< Short Slot is supported      
	*/
	PBCC           = 0x40,   
	/*!< PBCC                         
	*/
	CHANNELAGILITY = 0x80,   
	/*!< Channel Agility              
	*/
	SPECTRUM_MGMT  = 0x100,  
	/*!< Spectrum Management          
	*/
	DSSS_OFDM      = 0x2000  
	/*!< DSSS-OFDM                    
	*/
}tenuWifiCapability;


#endif





/*!
@typedef \
	tpfAppWifiCb

@brief	Application notification callback
	
	Pointer to a function delivering m2m_wifi events. 

@param [in]	u8MsgType
				Type of notification. Possible types are:
				- [M2M_WIFI_RESP_CURRENT_RSSI](@ref M2M_WIFI_RESP_CURRENT_RSSI)
				- [M2M_WIFI_RESP_CON_STATE_CHANGED](@ref M2M_WIFI_RESP_CON_STATE_CHANGED)
				- [M2M_WIFI_RESP_SCAN_DONE](@ref M2M_WIFI_RESP_SCAN_DONE)
				- [M2M_WIFI_RESP_SCAN_RESULT](@ref M2M_WIFI_RESP_SCAN_RESULT)
				- [M2M_WIFI_REQ_WPS](@ref M2M_WIFI_REQ_WPS)
				- [M2M_WIFI_RESP_IP_CONFIGURED](@ref M2M_WIFI_RESP_IP_CONFIGURED)
				- [M2M_WIFI_RESP_IP_CONFLICT](@ref M2M_WIFI_RESP_IP_CONFLICT) 
				
				- [M2M_WIFI_RESP_CLIENT_INFO](@ref M2M_WIFI_RESP_CLIENT_INFO)

@param [in]	pvMsg
				A pointer to a buffer containing the notification parameters (if any). It should be
				Casted to the correct data type corresponding to the notification type.
	
@sa
	tstrM2mWifiStateChanged
	tstrM2MWPSInfo
	
	tstrM2mScanDone
	tstrM2mWifiscanResult
*/
typedef void (*tpfAppWifiCb) (uint8 u8MsgType, void * pvMsg);

/*!
@typedef \
	tpfAppEthCb

@brief	Ethernet Application notification callback
	
	Pointer to a function delivering m2m_wifi events. 

@param [in]	u8MsgType
				Type of notification. Possible types are:
				- [M2M_WIFI_REQ_SEND_ETHERNET_PACKET](@ref M2M_WIFI_REQ_SEND_ETHERNET_PACKET)
				- [M2M_WIFI_RESP_ETHERNET_RX_PACKET](@ref M2M_WIFI_RESP_ETHERNET_RX_PACKET)
@param [in]	pvMsg
				A pointer to a buffer containing the notification parameters (if any). It should be
				casted to the correct data type corresponding to the notification type.
@param [in]	pvControlBuf
				A pointer to control buffer describing the accompanied message. 

*/
typedef void (*tpfAppEthCb) (uint8 u8MsgType, void * pvMsg,void * pvCtrlBuf);

/*!
@typedef	\
	tpfAppMonCb

@brief	
	Deliver a Wi-Fi packet from the monitoring driver. 

@param [in]	pstrWifiRxPacket
				Pointer to a structure holding the Wi-Fi packet header parameters.

@param [in]	pu8Payload
				Buffer holding the Wi-Fi packet payload information required by the application starting from the
				defined OFFSET by the application (when calling m2m_wifi_enable_monitoring_mode). It shall be NULL
				if the application does not need any data from the payload.

@param [in]	u16PayloadSize
				The size of the payload in bytes. It cannot exceed the buffer size given 
				through m2m_wifi_enable_monitoring_mode.
*/
typedef void (*tpfAppMonCb) (tstrM2MWifiRxPacketInfo *pstrWifiRxPacket, uint8 * pu8Payload, uint16 u16PayloadSize);

/**
@struct 	\
	tstrEthInitParam
	
@brief		
	Structure to hold Ethernet interface parameters.
*/
typedef struct {
	tpfAppWifiCb pfAppWifiCb;
	/*!<
		Callback for wifi notifications.
	*/
	tpfAppEthCb  pfAppEthCb;
	/*!<
		Callback for Ethernet interface.
	*/
	uint8 * au8ethRcvBuf;
	/*!<
		Pointer to Receive Buffer of Ethernet Packet
	*/
	uint16	u16ethRcvBufSize;
	/*!<
		Size of Receive Buffer for Ethernet Packet
	*/
} tstrEthInitParam;
/*!
@struct	\	
 	tstrM2mIpCtrlBuf
 	
@brief		
 	Ctrl buffer of packet sent to host in ByPass mode

 */
typedef struct{
	uint16	u16DataSize;
	uint16	u16RemainigDataSize;
} tstrM2mIpCtrlBuf;


/**
@struct		\
	tstrWifiInitParam

@brief		
	Structure to hold m2m_wifi_init() parameters.
*/
typedef struct {
	tpfAppWifiCb pfAppWifiCb;
	/*!<
		Callback for Wi-Fi notifications.
	*/
	tpfAppMonCb  pfAppMonCb;
	/*!<
		Callback for monitoring interface.
	*/
	tstrEthInitParam strEthInitParam ;
	/*!<
	Structure to hold Ethernet interface parameters.
	*/

} tstrWifiInitParam;

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#ifdef __cplusplus
     extern "C" {
#endif

/*!
@fn	\
	NMI_API void  m2m_wifi_download_mode(void);

@brief
	Initialize the M2M_WIFI chip and enter download mode

@return		
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8  m2m_wifi_download_mode(void);

/*!
@fn	\
	NMI_API void  m2m_wifi_init(void);

@brief
	Initialize the M2M_WIFI layer.

@param [in]	pWifiInitParam

@return		
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8  m2m_wifi_init(tstrWifiInitParam * pWifiInitParam);


/*!
@fn	\
	NMI_API sint8  m2m_wifi_deinit(void * arg);

@brief
	Free resources used by the M2M_WIFI layer.

@return		
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8  m2m_wifi_deinit(void * arg);


/*!
@fn	\
	NMI_API void  m2m_wifi_handle_events(void);

@brief
	M2M event handler loop

@return		
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_handle_events(void * arg);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_default_connect(void);

@brief
	Request a Wi-Fi connection from the WINC firmware. This firmware SHALL connect to the last successfully connected
	AP from the cached connections. If the connection fails, the firmware continues trying to connect to the next strored
	AP and so on until a connection is established or the end of list is reached. 
*/
NMI_API sint8 m2m_wifi_default_connect(void);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_connect(char *pcSsid, uint8 u8SsidLen, uint8 u8SecType, void *pvAuthInfo, uint16 u16Ch);

@brief
	Request a Wi-Fi connection to a specific AP.

@param [in]	pcSsid
				A buffer holding the SSID corresponding to the requested AP.
				
@param [in]	u8SsidLen
				Length of the given SSID (not including the NULL termination).

@param [in]	u8SecType
				Wi-Fi security type security for the network. It can be one of the following types:
				- [M2M_WIFI_SEC_OPEN](@ref M2M_WIFI_SEC_OPEN)
				- [M2M_WIFI_SEC_WEP](@ref M2M_WIFI_SEC_WEP)
				- [M2M_WIFI_SEC_WPA_PSK](@ref M2M_WIFI_SEC_WPA_PSK)
				- [M2M_WIFI_SEC_802_1X](@ref M2M_WIFI_SEC_802_1X)

@param [in]	pvAuthInfo
				Authentication parameters required for completing the connection. It is type is based on the Security type.

@param [in]	u16Ch
				Wi-Fi Channel number as in [tenuM2mScanCh](@ref tenuM2mScanCh).

@return		
	The function SHALL return 0 for success and a negative value otherwise.

@sa
	tuniM2MWifiAuth
	tstr1xAuthCredentials
	tstrM2mWifiWepParams
*/
NMI_API sint8 m2m_wifi_connect(char *pcSsid, uint8 u8SsidLen, uint8 u8SecType, void *pvAuthInfo, uint16 u16Ch);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_disconnect(void);

@brief
	Request a Wi-Fi disconnect from the currently connected AP.

@return		
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_disconnect(void);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_start_provision_mode(tstrM2MAPConfig *pstrAPConfig, uint8 *pu8ApIPAddr, char *pcHttpServerDomainName, uint8 bEnableHttpRedirect);

@brief
	Start the AP mode HTTP provisioning.
	
@param [in]	pstrAPConfig
				AP configuration parameters.

@param [in]	pcHttpServerDomainName
				Domain name of the HTTP Provision server.

@param [in]	bEnableHttpRedirect
				A flag to enable/disable the HTTP Redirect feature. 
				If it is enabled (set to a non-ZERO value), then all http traffic (http://URL) from the associated 
				device (Phone, PC, ...etc) will be redirected to the WINC HTTP Provisioning Web page.
				If it is disabled (set to ZER), then the associated device could open the provisioning page only when
				the HTTP Provision URL of the WINC HTTP Server is correctly written on the browser.  

@return
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_start_provision_mode(tstrM2MAPConfig *pstrAPConfig, char *pcHttpServerDomainName, uint8 bEnableHttpRedirect);


/*!
@fn	\
	sint8 m2m_wifi_stop_provision_mode(void);
	
@brief
	Stop the provision mode if it is active.

@return
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_stop_provision_mode(void);


/*!
@fn	\
	sint8 m2m_wifi_get_connection_info(void);
	
@brief
	Retrieve the status information of the currently connected AP. The result is passed to the Wi-Fi notification callback
	with [M2M_WIFI_RESP_CONN_INFO](@ref M2M_WIFI_RESP_CONN_INFO).
	
@sa
	M2M_WIFI_RESP_CONN_INFO,
	tstrM2MConnInfo
@return
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_get_connection_info(void);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_set_mac_address(uint8 au8MacAddress[6]);

@brief
	Assign MAC address to the NMC1500. It is used for non-production SW.

@return		
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_set_mac_address(uint8 au8MacAddress[6]);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_wps(uint8 u8TriggerType,const char * pcPinNumber);

@brief
	Initialize the M2M_WIFI host driver.

@param [in]	u8TriggerType
				WPS Trigger method. Could be:
				- [WPS_PIN_TRIGGER](@ref WPS_PIN_TRIGGER)
				- [WPS_PBC_TRIGGER](@ref WPS_PBC_TRIGGER)
				
@param [in]	pcPinNumber
				PIN number for WPS PIN method.

@return
	The function SHALL return 0 for success and a negative value otherwise.

@sa
	tenuWPSTrigger
*/
NMI_API sint8 m2m_wifi_wps(uint8 u8TriggerType,const char  *pcPinNumber);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_wps_disable(void);

@brief
	Disable the NMC1500 WPS operation.

@return
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_wps_disable(void);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_p2p(uint8 u8Channel);

@brief
	Enable the NMC1500 device to work in Wi-Fi direct mode (P2P).

@param [in]	u8Channel
				P2P Listen channel. It could be 1, 6 or 11 

@return
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_p2p(uint8 u8Channel);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_p2p_disconnect(void);

@brief
	Disable the NMC1500 device Wi-Fi direct mode (P2P).

@return
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_p2p_disconnect(void);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_enable_ap(CONST tstrM2MAPConfig* pstrM2MAPConfig);

@brief
	Enable the NMC1500 device Wi-Fi Hotspot mode (Soft AP).

@param [in]	pstrM2MAPConfig
				A structure holding the AP configurations.

@return
	The function SHALL return 0 for success and a negative value otherwise.

@sa
	tstrM2MAPConfig
*/
NMI_API sint8 m2m_wifi_enable_ap(CONST tstrM2MAPConfig* pstrM2MAPConfig);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_disable_ap(void);

@brief
	Disable the NMC1500 device Wi-Fi Hotspot mode.

@return
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_disable_ap(void);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_set_static_ip(tstrM2MIPConfig * pstrStaticIPConf);

@brief
	Force the use of a certain static IP Address configurations.

@param [in]	pstrStaticIPConf
				Pointer to a structure holding the static IP Configurations (IP, 
				Gateway, subnet mask and DNS address).
@return
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_set_static_ip(tstrM2MIPConfig * pstrStaticIPConf);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_request_dhcp_client(void);

@brief
	legacy API should be removed (dhcp requested by the firmware automatocally in STA/AP/P2P mode)

@return
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_request_dhcp_client(void);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_request_dhcp_server(void);


@brief
	legacy API should be removed (dhcp requested by the firmware automatocally in STA/AP/P2P mode)

@return
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_request_dhcp_server(uint8* addr);


/*!
@fn	\
	sint8 m2m_wifi_set_scan_options(uint8 u8NumOfSlot,uint8 u8SlotTime)


@brief
	Set Wi-Fi scan options .

@param [in]	u8NumOfSlot;
	The min number of slots is 2 for every channel, 
	every slot the soc will send Probe Req on air, and wait/listen for PROBE RESP/BEACONS for the u8slotTime in ms
	
@param [in]	 u8SlotTime;
		the time in ms that the Soc will wait on every channel listening to the frames on air
		when that time increaseed number of AP will increased in the scan results
		min time is 10 ms and the max is 250 ms 
			
@return
	The function SHALL return 0 for success and a negative value otherwise.

@sa
	tenuM2mScanCh
*/
NMI_API sint8 m2m_wifi_set_scan_options(uint8 u8NumOfSlot,uint8 u8SlotTime);


/*!
@fn	\
	sint8 m2m_wifi_set_scan_region(uint8 ScanRegion)

@brief
	Set Wi-Fi scan region,it will effect only the M2M_WIFI_CH_ALL macro

@param [in]	ScanRegion;
		ASIA = 14
		NORTH_AMERICA = 11

				
@return
	The function SHALL return 0 for success and a negative value otherwise.

@sa
	tenuM2mScanCh
*/
NMI_API sint8 m2m_wifi_set_scan_region(uint8  ScanRegion);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_request_scan(uint8 ch);

@brief
	Request Wi-Fi scan on the given channel.

@param [in]	ch
				RF Channel ID for SCAN operation. It should be set according to tenuM2mScanCh. 

		
@return
	The function SHALL return 0 for success and a negative value otherwise.

@sa
	tenuM2mScanCh
*/
NMI_API sint8 m2m_wifi_request_scan(uint8 ch);


/*!
@fn	\
	NMI_API uint8 m2m_wifi_get_num_ap_found(void);

@brief
	Reads the number of APs found in the last SCAN process.

@return
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API uint8 m2m_wifi_get_num_ap_found(void);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_req_scan_result(uint8 index);

@brief
	Reads the AP information from the SCAN Result list with the given index

@param [in]	index
				Index for the requested result

@return
	The function SHALL return 0 for success and a negative value otherwise
*/
NMI_API sint8 m2m_wifi_req_scan_result(uint8 index);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_req_curr_rssi(void);

@brief
	Request the current RSSI for the current connected AP.

@return
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_req_curr_rssi(void);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_get_otp_mac_address(uint8 *pu8MacAddr, uint8 * pu8IsValid);

@brief
	Request the MAC address stored on the OTP memory of the device.

@param [out]	pu8MacAddr
		Output MAC address buffer of 6 bytes size. Valid only if *pu8Valid=1.
@param [out]	pu8IsValid
		A output boolean value to indicate the validity of pu8MacAddr in OTP. 
		Output zero if the OTP memory is not programmed, non-zero otherwise.	
@return
	The function SHALL return 0 for success and a negative value otherwise.
\sa

	m2m_wifi_get_mac_address
*/
NMI_API sint8 m2m_wifi_get_otp_mac_address(uint8 *pu8MacAddr, uint8 * pu8IsValid);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_get_mac_address(uint8 *pu8MacAddr)

@brief
	Request the current MAC address of the device.

@return
	The function SHALL return 0 for success and a negative value otherwise.
@sa
	m2m_wifi_get_otp_mac
*/
NMI_API sint8 m2m_wifi_get_mac_address(uint8 *pu8MacAddr);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_set_sleep_mode(uint8 PsTyp, uint8 BcastEn);

@brief
	Set the power saving mode for the NMC1500. 

@param [in]	PsTyp
				Desired power saving mode. Supported types are defined in tenuPowerSaveModes.

@param [in]	BcastEn
				Broadcast reception enable flag. 
				If it is 1, the NMC1500 must be awake each DTIM Beacon for receiving Broadcast traffic.
				If it is 0, the NMC1500 will not wakeup at the DTIM Beacon, but its wakeup depends only 
				on the the configured Listen Interval. 

@return
	The function SHALL return 0 for success and a negative value otherwise.

@sa
	tenuPowerSaveModes
*/
NMI_API sint8 m2m_wifi_set_sleep_mode(uint8 PsTyp, uint8 BcastEn);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_request_sleep(void);

@brief
	Set the NMC1500 device to work in the current configured Power save mode.

@param [in]	u32SlpReqTime
				Request Sleep in ms 
@return
	The function SHALL return 0 for success and a negative value otherwise.

@sa
	tenuPowerSaveModes
*/
NMI_API sint8 m2m_wifi_request_sleep(uint32 u32SlpReqTime);


/*!
@fn	\
	NMI_API uint8 m2m_wifi_get_sleep_mode(void);

@brief
	Get the current Power save mode.

@return
	The current operating power saving mode.

@sa
	tenuPowerSaveModes
*/
NMI_API uint8 m2m_wifi_get_sleep_mode(void);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_req_client_ctrl(uint8 cmd);

@brief
	Send a command to the PS Client (An NMC1500 board running the ps_firmware)

@param [in]	cmd
				Control command sent from PS Server to PS Client.

@return
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_req_client_ctrl(uint8 cmd);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_req_server_init(uint8 ch);

@brief
	Initialize the PS Server

@param [in]	ch
				Server listening channel

@return
	The function SHALL return 0 for success and a negative value otherwise
*/
NMI_API sint8 m2m_wifi_req_server_init(uint8 ch);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_set_device_name(uint8 *pu8DeviceName, uint8 u8DeviceNameLength);

@brief
	Set the NMC1500 device name which is used as P2P device name.

@param [in]	pu8DeviceName
				Buffer holding the device name.

@param [in]	u8DeviceNameLength
				Length of the device name.

@return
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_set_device_name(uint8 *pu8DeviceName, uint8 u8DeviceNameLength);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_set_lsn_int(tstrM2mLsnInt * pstrM2mLsnInt);

@brief
	Set the Wi-Fi listen interval for power save operation. It is represented in units
	of AP Beacon periods.

@param [in]	pstrM2mLsnInt
				Structure holding the listen interval configurations.
	
@return
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_set_lsn_int(tstrM2mLsnInt *pstrM2mLsnInt);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_enable_monitoring_mode(tstrM2MWifiMonitorModeCtrl *pstrMtrCtrl, uint8 *pu8PayloadBuffer, \ 
										   uint16 u16BufferSize, uint16 u16DataOffset);

@brief
	Enable Wi-Fi monitoring mode (Promiscuous mode) with the given filtering information. All packets that meet the
	filtering criteria are passed to the application through calling the function wifi_monitoring_cb.

@param [in]	pstrMtrCtrl
				Pointer to a structure holding the Filtering parameters. The Channel ID must be set to a value
				between 1 and 11.

@param [in]	pu8PayloadBuffer
				Pointer to a Buffer allocated by the application. The buffer SHALL hold the Data field of 
				the Wi-Fi Rx Packet (Or a part from it). If it is set to NULL, the Wi-Fi data payload will 
				be discarded by the monitoring driver.

@param [in]	u16BufferSize
				The total size of the pu8PayloadBuffer in bytes.

@param [in]	u16DataOffset
				Starting offset in the DATA FIELD of the received Wi-Fi packet. The application may be interested
				in reading specific information from the received packet. It must assign the offset to the starting
				position of it relative to the DATA payload start.
				Example, if the SSID is needed to be read from a PROBE REQ packet, the u16Offset MUST be set to 0.

@return		
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_enable_monitoring_mode(tstrM2MWifiMonitorModeCtrl *pstrMtrCtrl, uint8 *pu8PayloadBuffer, 
										   uint16 u16BufferSize, uint16 u16DataOffset);

/*!
@fn	\
	NMI_API sint8 m2m_wifi_disable_monitoring_mode(void)

@brief
	Disable Wi-Fi monitoring mode (Promiscuous mode).
*/
NMI_API sint8 m2m_wifi_disable_monitoring_mode(void);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_send_wlan_pkt(uint8 *pu8WlanPacket, uint16 u16WlanHeaderLength, uint16 u16WlanPktSize);

@brief	Transmit a Wi-Fi RAW packet.

	The Wi-Fi packet composition is left to the application developer. 

@param [in]	pu8WlanPacket
				Pointer to a buffer holding the whole Wi-Fi frame.

@param [in]	u16WlanHeaderLength
				The size of the Wi-Fi packet header.

@param [in]	u16WlanPktSize
				The size of the whole packet in bytes. 

@return		
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_send_wlan_pkt(uint8 *pu8WlanPacket, uint16 u16WlanHeaderLength, uint16 u16WlanPktSize);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_send_ethernet_pkt(uint8* pu8Packet,uint16 u16PacketSize);

@brief	Transmit an Ethernet packet.

	The Ethernet packet composition is left to the application developer. 

@param [in]	pu8Packet
				Pointer to a buffer holding the whole Ethernet frame.

@param [in]	u16PacketSize
				The size of the whole packet in bytes. 

@return		
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_send_ethernet_pkt(uint8* pu8Packet,uint16 u16PacketSize);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_enable_sntp(uint8 bEnable);

@brief
	Enable/Disable the native SNTP client in the m2m firmware. The SNTP is enabled by default at start-up.
	The SNTP client at firmware is used to sync the system clock to the UTC time from a well known time
	servers (eg. "time-c.nist.gov"). The SNTP client uses a default update cycle of 1 day.
	The UTC is important for checking the expiration date of X509 certificates used while establishing
	TLS (Transport Layer Security) connections.
	It is highly recommended to use it if there is no other means to get the UTC time. If there is a RTC
	on the host MCU, the SNTP could be disabled and the host should set the system time to the firmware 
	using the API m2m_wifi_set_system_time.

@param [in]	bEnable
				A flag to indicate the SNTP state. 
				- 1  Enable the SNTP operation.

@return		
	The function SHALL return 0 for success and a negative value otherwise.

@sa
	m2m_wifi_set_sytem_time
*/
NMI_API sint8 m2m_wifi_enable_sntp(uint8 bEnable);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_set_sytem_time(tstrSystemTime *pstrSysTime);

@brief
	Set the system time in time/date format.

@param [in]	u32RTCSeconds
				UTC value in seconds.

@return		
	The function SHALL return 0 for success and a negative value otherwise.

@sa
	m2m_wifi_enable_sntp
*/
NMI_API sint8 m2m_wifi_set_sytem_time(uint32 u32UTCSeconds);


/*!
@fn	\
	NMI_API sint8 m2m_wifi_set_cust_InfoElement(uint8* pau8M2mCustInfoElement);

@brief
	Add a user-defined Information Element to the Wi-Fi Beacon and Probe Response frames.

@param [in]	pau8M2mCustInfoElement
				Buffer containing the IE to be sent. It is the responsibility of the application developer who
				needs this feature is responsible for the contents of the information element. 

@return		
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_set_cust_InfoElement(uint8* pau8M2mCustInfoElement);

#ifdef ETH_MODE
/*!
@fn	\
	 NMI_API sint8 m2m_wifi_enable_mac_mcast(uint8* pu8MulticastMacAddress, uint8 u8AddRemove)

@brief
	Add MAC filter to receive Multicast packets.

@param [in]	pu8MulticastMacAddress
				Pointer to the MAC address.
@param [in] u8AddRemove
				Flag to Add/Remove MAC address.
@return		
	The function SHALL return 0 for success and a negative value otherwise.
*/

NMI_API sint8 m2m_wifi_enable_mac_mcast(uint8* pu8MulticastMacAddress, uint8 u8AddRemove);

/*!
@fn	\
	NMI_API sint8  m2m_wifi_set_receive_buffer(void* pvBuffer,uint16 u16BufferLen);

@brief
	set the Ethernet receive buffer, should be called in the receive call back.

@param [in]	pvBuffer
				Pointer to the Ethernet receive buffer.
@param [in] u16BufferLen
				Length of the buffer.

@return		
	The function SHALL return 0 for success and a negative value otherwise.
*/
NMI_API sint8 m2m_wifi_set_receive_buffer(void* pvBuffer,uint16 u16BufferLen);
#endif


#ifdef __cplusplus
}
#endif
#endif /* __M2M_WIFI_H__ */
