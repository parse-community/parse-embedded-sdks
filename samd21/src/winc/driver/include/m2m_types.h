/**
 *
 * \file
 *
 * \brief NMC1500 IoT Application Interface Internal Types.
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

#ifndef __M2M_WIFI_TYPES_H__
#define __M2M_WIFI_TYPES_H__


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#ifndef	_BOOT_
#ifndef _FIRMWARE_
#include "common/include/nm_common.h"
#else
#include "m2m_common.h"
#endif
#endif


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
MACROS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#define MAKE_VERSION(major, minor, patch) (((uint16)(major) << 8) | ((minor << 4) | (patch) ))

/*======*======*======*======*
		FIRMWARE VERSION NO INFO
 *======*======*======*======*/

#define M2M_FIRMWARE_VERSION_MAJOR_NO 					(18)
/*!< Firmware Major release version number.
*/


#define M2M_FIRMWARE_VERSION_MINOR_NO					(0)
/*!< Firmware Minor release version number.
*/

#define M2M_FIRMWARE_VERSION_PATCH_NO					(0)
/*!< Firmware patch release version number.
*/

/*======*======*======*======*
  SUPPORTED DRIVER VERSION NO INFO
 *======*======*======*======*/

#define M2M_DRIVER_VERSION_MAJOR_NO 					(18)
/*!< Driver Major release version number.
*/


#define M2M_DRIVER_VERSION_MINOR_NO						(0)
/*!< Driver Minor release version number.
*/

#define M2M_DRIVER_VERSION_PATCH_NO						(0)
/*!< Driver patch release version number.
*/


#if !defined(M2M_FIRMWARE_VERSION_MAJOR_NO) || !defined(M2M_FIRMWARE_VERSION_MINOR_NO)
#error Undefined version number
#endif

#define M2M_BUFFER_MAX_SIZE								(1600UL - 4)
/*!< Maximum size for the shared packet buffer.
 */


#define M2M_MAC_ADDRES_LEN                                 				6
/*!< The size fo 802 MAC address.
 */

#define M2M_ETHERNET_HDR_OFFSET							34
/*!< The offset of the Ethernet header within the WLAN Tx Buffer.
 */


#define M2M_ETHERNET_HDR_LEN								14
/*!< Length of the Etherenet header in bytes.
*/


#define M2M_MAX_SSID_LEN 									33
/*!< Maximum size for the Wi-Fi SSID including the NULL termination.
 */


#define M2M_MAX_PSK_LEN           								65
/*!< Maximum size for the WPA PSK including the NULL termination.
 */


#define M2M_DEVICE_NAME_MAX								48
/*!< Maximum Size for the device name including the NULL termination.
 */
 

#define M2M_LISTEN_INTERVAL 								1
/*!<
*/


#define M2M_1X_USR_NAME_MAX								21
/*!< The maximum size of the user name including the NULL termination. 
	It is used for RADIUS authentication in case of connecting the device to
	an AP secured with WPA-Enterprise. 
*/


#define M2M_1X_PWD_MAX									41
/*!< The maximum size of the password including the NULL termination. 
	It is used for RADIUS authentication in case of connecting the device to
	an AP secured with WPA-Enterprise. 
*/

#define M2M_CUST_IE_LEN_MAX								252
/*!< The maximum size of IE (Information Element).
*/


#define M2M_CONFIG_CMD_BASE								1
#define M2M_SERVER_CMD_BASE								20
#define M2M_STA_CMD_BASE									40
#define M2M_AP_CMD_BASE										70
#define M2M_P2P_CMD_BASE									90
#define M2M_OTA_CMD_BASE									100


#define WEP_40_KEY_SIZE 								((uint8)5)
#define WEP_104_KEY_SIZE 								((uint8)13)
#define WEP_40_KEY_STRING_SIZE 						((uint8)10)
#define WEP_104_KEY_STRING_SIZE 						((uint8)26)
#define WEP_KEY_MAX_INDEX								((uint8)4)

#define M2M_SCAN_MIN_NUM_SLOTS							(2)

#define M2M_SCAN_MIN_SLOT_TIME							(10)

#define M2M_SCAN_FAIL 									((uint8)1)
#define M2M_JOIN_FAIL	 								((uint8)2)
#define M2M_AUTH_FAIL 									((uint8)3)
#define M2M_ASSOC_FAIL 								((uint8)4)

#define M2M_SCAN_ERR_WIFI   	 						((sint8)-2)
#define M2M_SCAN_ERR_IP      							((sint8)-3)
#define M2M_SCAN_ERR_AP      							((sint8)-4)		
#define M2M_SCAN_ERR_P2P      							((sint8)-5)		
#define M2M_SCAN_ERR_WPS      							((sint8)-6)		


#define M2M_DEFAULT_CONN_EMPTY_LIST					((sint8)-20)
/*!<
	m2m_default_connect fails beacuase of empty network list.
*/


#define M2M_DEFAULT_CONN_SCAN_MISMATCH				((sint8)-21)
/*!<
	m2m_default_connect failes beacuse no one of the cached 
	networks found in the scan results.
*/


/*======*======*======*======*
	MONTIORING MODE DEFINITIONS
 *======*======*======*======*/

#define M2M_WIFI_FRAME_TYPE_ANY							0xFF

#define M2M_WIFI_FRAME_SUB_TYPE_ANY						0xFF


/*======*======*======*======*
	OTA DEFINITIONS
 *======*======*======*======*/
 
#define OTA_ROLLB_STATUS_VALID			(0x12526285)
#define OTA_ROLLB_STATUS_INVALID		(0x23987718)
#define OTA_MAGIC_VALUE					(0x1ABCDEF9)

#define OTA_SUCCESS 						(0)
#define OTA_ERR_WORKING_IMAGE_LOAD_FAIL		((sint8)-1)
#define OTA_ERR_INVAILD_CONTROL_SEC			((sint8)-2)
#define M2M_ERR_OTA_SWITCH_FAIL     		((sint8)-3)
#define M2M_ERR_OTA_START_UPDATE_FAIL     	((sint8)-4)
#define M2M_ERR_OTA_ROLLBACK_FAIL     		((sint8)-5)
#define M2M_ERR_OTA_INVAILD_FLASH_SIZE     	((sint8)-6)
#define M2M_ERR_OTA_INVAILD_ARG		     	((sint8)-7)
/*
SPI FLASH COMMANDS
*/
#if 0
#define OTA_CMD_W						((uint8)0x81)		/*!< write from chip memory memory to SPI flash */
#define OTA_CMD_R						((uint8)0x83)		/*!< Read from SPI flash to chip memory */
#define OTA_CMD_E						((uint8)0x82)		/*!< Erase SPI flash memory of size */
#endif
#define OTA_SHA256_DIGEST_SIZE 				(32)


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
DATA TYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*!
@enum	\
	tenuM2mConfigCmd
	
@brief
	
*/
typedef enum {
	M2M_WIFI_REQ_RESTART = M2M_CONFIG_CMD_BASE,
	M2M_WIFI_REQ_SET_MAC_ADDRESS,
	M2M_WIFI_REQ_CURRENT_RSSI,
	M2M_WIFI_RESP_CURRENT_RSSI,
	M2M_WIFI_REQ_SET_DEVICE_NAME,
	M2M_WIFI_REQ_START_PROVISION_MODE,
	/*!<
		Start the provisioning mode for the M2M Device.
	*/
	M2M_WIFI_RESP_PROVISION_INFO,
	/*!<
		Send the provisioning information to the host.
	*/
	M2M_WIFI_REQ_STOP_PROVISION_MODE,
	/*!<
		Stop the current running provision mode.
	*/
	M2M_WIFI_REQ_SET_SYS_TIME,
	/*!<
		Set time of day from host.
	*/
	M2M_WIFI_REQ_ENABLE_SNTP_CLIENT,
	/*!<
	*/
	M2M_WIFI_REQ_DISABLE_SNTP_CLIENT,
	/*!<
	*/
	M2M_WIFI_RESP_MEMORY_RECOVER,
	M2M_WIFI_REQ_CUST_INFO_ELEMENT
	/*!< Add Custom ELement to Beacon Managament Frame.
	*/
}tenuM2mConfigCmd;


/*!
@enum	\
	tenuM2mServerCmd
	
@brief

*/
typedef enum {
	M2M_WIFI_REQ_CLIENT_CTRL = M2M_SERVER_CMD_BASE,
	M2M_WIFI_RESP_CLIENT_INFO,
	M2M_WIFI_REQ_SERVER_INIT
}tenuM2mServerCmd;


/*!
@enum	\
	tenuM2mStaCmd
	
@brief

*/
typedef enum {
	M2M_WIFI_REQ_CONNECT = M2M_STA_CMD_BASE,
	M2M_WIFI_REQ_DEFAULT_CONNECT,
	M2M_WIFI_RESP_DEFAULT_CONNECT,
	M2M_WIFI_REQ_GET_CONN_INFO,
	M2M_WIFI_RESP_CONN_INFO,
	M2M_WIFI_REQ_DISCONNECT,
	M2M_WIFI_RESP_CON_STATE_CHANGED,
	M2M_WIFI_REQ_SLEEP,
	M2M_WIFI_REQ_SCAN,
	M2M_WIFI_REQ_WPS_SCAN,
	M2M_WIFI_RESP_SCAN_DONE,
	M2M_WIFI_REQ_SCAN_RESULT,
	M2M_WIFI_RESP_SCAN_RESULT,
	M2M_WIFI_REQ_WPS,
	M2M_WIFI_REQ_START_WPS,
	M2M_WIFI_REQ_DISABLE_WPS,
	M2M_WIFI_REQ_DHCP_CONF,
	M2M_WIFI_RESP_IP_CONFIGURED,
	M2M_WIFI_RESP_IP_CONFLICT,
	M2M_WIFI_REQ_ENABLE_MONITORING,
	M2M_WIFI_REQ_DISABLE_MONITORING,
	M2M_WIFI_RESP_WIFI_RX_PACKET,
	M2M_WIFI_REQ_SEND_WIFI_PACKET,
	M2M_WIFI_REQ_LSN_INT,
	M2M_WIFI_REQ_SEND_ETHERNET_PACKET,
	M2M_WIFI_RESP_ETHERNET_RX_PACKET,
	M2M_WIFI_REQ_SET_SCAN_OPTION,
	M2M_WIFI_REQ_SET_SCAN_REGION,
	M2M_WIFI_REQ_DOZE,
	M2M_WIFI_REQ_SET_MAC_MCAST
} tenuM2mStaCmd;


/*!
@enum	\
	tenuM2mP2pCmd
	
@brief

*/
typedef enum {
	M2M_WIFI_REQ_P2P_INT_CONNECT = M2M_P2P_CMD_BASE,
	M2M_WIFI_REQ_ENABLE_P2P,
	M2M_WIFI_REQ_DISABLE_P2P,
	M2M_WIFI_REQ_P2P_REPOST
}tenuM2mP2pCmd;


/*!
@enum	\
	tenuM2mApCmd
	
@brief

*/
typedef enum {
	M2M_WIFI_REQ_ENABLE_AP = M2M_AP_CMD_BASE,
	M2M_WIFI_REQ_DISABLE_AP,
}tenuM2mApCmd;

/*!
@enum	\
	tenuM2mOtaCmd
	
@brief

*/
typedef enum {
	M2M_OTA_REQ_NOTIF_SET_URL = M2M_OTA_CMD_BASE,
	M2M_OTA_REQ_NOTIF_CHECK_FOR_UPDATE,
	M2M_OTA_REQ_NOTIF_SCHED,
	M2M_OTA_REQ_START_UPDATE,
	M2M_OTA_REQ_SWITCH_FIRMWARE,
	M2M_OTA_REQ_ROLLBACK,
	M2M_OTA_RESP_NOTIF_UPDATE_INFO,
	M2M_OTA_RESP_UPDATE_STATUS,
	M2M_OTA_REQ_TEST
}tenuM2mOtaCmd;


/*!
@enum	\
	tenuM2mIpCmd
	
@brief

*/
typedef enum {
	/* Request IDs corresponding to the IP GROUP. */
	M2M_IP_REQ_STATIC_IP_CONF = ((uint8) 10)
} tenuM2mIpCmd;


/*!
@enum	\
	tenuM2mConnState
	
@brief
	Wi-Fi Connection State.
*/
typedef enum {
	M2M_WIFI_DISCONNECTED = 0,
	/*!< Wi-Fi state is disconnected.
	*/
	M2M_WIFI_CONNECTED,
	/*!< Wi-Fi state is connected.
	*/
	M2M_WIFI_UNDEF = 0xff
	/*!< Undefined Wi-Fi State.
	*/
}tenuM2mConnState;


/*!
@enum	\
	tenuM2mSecType
	
@brief
	Wi-Fi Supported Security types.
*/
typedef enum {
	M2M_WIFI_SEC_INVALID = 0,
	/*!< Invalid security type.
	*/
	M2M_WIFI_SEC_OPEN,
	/*!< Wi-Fi network is not secured.
	*/
	M2M_WIFI_SEC_WPA_PSK,
	/*!< Wi-Fi network is secured with WPA/WPA2 personal(PSK).
	*/
	M2M_WIFI_SEC_WEP,
	/*!< Security type WEP (40 or 104) OPEN OR SHARED.
	*/
	M2M_WIFI_SEC_802_1X
	/*!< Wi-Fi network is secured with WPA/WPA2 Enterprise.IEEE802.1x user-name/password authentication.
	 */
}tenuM2mSecType;


/*!
@enum	\
	tenuM2mSecType
	
@brief
	Wi-Fi Supported Security types.
*/
typedef enum {
	SSID_MODE_VISIBLE = 0,
	/*!< SSID is visible to others.
	*/
	SSID_MODE_HIDDEN
	/*!< SSID is hidden.
	*/
}tenuM2mSsidMode;

/*!
@enum	\
	tenuM2mScanCh
	
@brief
	Wi-Fi RF Channels.
*/
typedef enum {
	M2M_WIFI_CH_1 = ((uint8) 0),
	M2M_WIFI_CH_2,
	M2M_WIFI_CH_3,
	M2M_WIFI_CH_4,
	M2M_WIFI_CH_5,
	M2M_WIFI_CH_6,
	M2M_WIFI_CH_7,
	M2M_WIFI_CH_8,
	M2M_WIFI_CH_9,
	M2M_WIFI_CH_10,
	M2M_WIFI_CH_11,
	M2M_WIFI_CH_12,
	M2M_WIFI_CH_13,
	M2M_WIFI_CH_14,
	M2M_WIFI_CH_ALL = ((uint8) 255)
}tenuM2mScanCh;

/*!
@enum	\
	tenuM2mScanRegion
	
@brief
	Wi-Fi RF Channels.
*/
typedef enum {
	NORTH_AMERICA = ((uint8) 11),
	ASIA		=       ((uint8) 14)
}tenuM2mScanRegion;


/*!
@enum	\
	tenuPowerSaveModes
	
@brief
	Power Save Modes.
*/
typedef enum {
	M2M_NO_PS,
	M2M_PS_AUTOMATIC,
	M2M_PS_H_AUTOMATIC,
	M2M_PS_DEEP_AUTOMATIC,
	M2M_PS_MANUAL
}tenuPowerSaveModes;


/*!
@enum	\
	tenuWPSTrigger
	
@brief
	WPS Triggering Methods.
*/
typedef enum{
	WPS_PIN_TRIGGER = 0,
	/*!< WPS is triggered in PIN method.
	*/
	WPS_PBC_TRIGGER = 4
	/*!< WPS is triggered via push button.
	*/
}tenuWPSTrigger;


/*!
@struct	\
	tstrM2mWifiWepParams

@brief
	WEP security key parameters.
*/
typedef struct{
	uint8	u8KeyIndx;
	/*!< Wep key Index.
	*/
	uint8	u8KeySz;
	/*!< Wep key Size.
	*/
	uint8	au8WepKey[WEP_104_KEY_STRING_SIZE + 1];
	/*!< WEP Key represented as a NULL terminated ASCII string.
	*/
	uint8	__PAD24__[3];
	/*!< Padding bytes to keep the structure word alligned.
	*/
}tstrM2mWifiWepParams;


/*!
@struct	\
	tstr1xAuthCredentials

@brief
	Credentials for the user to authenticate with the AAA server (WPA-Enterprise Mode IEEE802.1x).
*/
typedef struct{
	uint8	au8UserName[M2M_1X_USR_NAME_MAX];
	/*!< User Name. It must be Null terminated string.
	*/
	uint8	au8Passwd[M2M_1X_PWD_MAX];
	/*!< Password corresponding to the user name. It must be Null terminated string.
	*/
}tstr1xAuthCredentials;


/*!
@union	\
	tuniM2MWifiAuth

@brief
	Wi-Fi Security Parameters for all supported security modes.
*/
typedef union{
	uint8				au8PSK[M2M_MAX_PSK_LEN];
	/*!< Pre-Shared Key in case of WPA-Personal security.
	*/
	tstr1xAuthCredentials	strCred1x;
	/*!< Credentials for RADIUS server authentication in case of WPA-Enterprise security.
	*/
	tstrM2mWifiWepParams	strWepInfo;
	/*!< WEP key parameters in case of WEP security.
	*/
}tuniM2MWifiAuth;


/*!
@struct	\
	tstrM2MWifiSecInfo

@brief
	Authentication credentials to connect to a Wi-Fi network.
*/
typedef struct{
	tuniM2MWifiAuth		uniAuth;
	/*!< Union holding all possible authentication parameters corresponding the current security types.
	*/
	uint8				u8SecType;
	/*!< Wi-Fi network security type. See tenuM2mSecType for supported security types.
	*/
#define __PADDING__		(4 - ((sizeof(tuniM2MWifiAuth) + 1) % 4))
	uint8				__PAD__[__PADDING__];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2MWifiSecInfo;


/*!
@struct	\
	tstrM2mWifiConnect

@brief	
	Wi-Fi Connect Request
*/
typedef struct{
	tstrM2MWifiSecInfo		strSec;
	/*!< Security parameters for authenticating with the AP.
	*/
	uint16				u16Ch;
	/*!< RF Channel for the target SSID.
	*/
	uint8				au8SSID[M2M_MAX_SSID_LEN];
	/*!< SSID of the desired AP. It must be NULL terminated string.
	*/
#define __CONN_PAD_SIZE__		(4 - ((sizeof(tstrM2MWifiSecInfo) + M2M_MAX_SSID_LEN + 2) % 4))
	uint8				__PAD__[__CONN_PAD_SIZE__];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2mWifiConnect;


/*!
@struct	\
	tstrM2MWPSConnect

@brief	
	WPS Configuration parameters

@sa 
	tenuWPSTrigger
*/
typedef struct {
	uint8 	u8TriggerType;
	/*!< WPS triggering method (Push button or PIN)
	*/
	char         acPinNumber[8];
	/*!< WPS PIN No (for PIN method)
	*/
	uint8	__PAD24__[3];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2MWPSConnect;


/*!
@struct	\
	tstrM2MWPSInfo

@brief	WPS Result

	This structure is passed to the application in response to a WPS request. If the WPS session is completed successfully, the
	structure will have Non-ZERO authentication type. If the WPS Session fails (due to error or timeout) the authentication type
	is set to ZERO.

@sa
	tenuM2mSecType
	
@todo
	Use different error codes to differentaite error types.
*/
typedef struct{
	uint8	u8AuthType;
	/*!< Network authentication type.
	*/
	uint8   	u8Ch;
	/*!< RF Channel for the AP.
	*/
	uint8	au8SSID[M2M_MAX_SSID_LEN];
	/*!< SSID obtained from WPS.
	*/
	uint8	au8PSK[M2M_MAX_PSK_LEN];
	/*!< PSK for the network obtained from WPS.
	*/
}tstrM2MWPSInfo;


/*!
@struct	\
	tstrM2MDefaultConnectResp

@brief
	Response error of the m2m_default_connect

@sa
	M2M_DEFAULT_CONN_SCAN_MISMATCH, M2M_DEFAULT_CONN_EMPTY_LIST
*/
typedef struct{
	sint8		s8ErrorCode;
	/*!<
		Default connect error code. possible values are:
		- M2M_DEFAULT_CONN_EMPTY_LIST
		- M2M_DEFAULT_CONN_SCAN_MISMATCH
	*/
	uint8	__PAD24__[3];
}tstrM2MDefaultConnResp;

/*!
@struct	\
	tstrM2MScan
	
@brief	
	Wi-Fi Scan Request

@sa 
	tenuM2mScanCh
*/
typedef struct {
	uint8   u8NumOfSlot;
	/*|< The min number of slots is 2 for every channel, 
	every slot the soc will send Probe Req on air, and wait/listen for PROBE RESP/BEACONS for the u16slotTime
	*/
	uint8   u8SlotTime;
	/*|< the time that the Soc will wait on every channel listening to the frames on air
		when that time increaseed number of AP will increased in the scan results
		min time is 10 ms and the max is 250 ms 
	*/
	uint8 __PAD16__[2];

}tstrM2MScanOption;

/*!
@struct	\
	tstrM2MScanRegion
	
@brief	
	Wi-Fi channel regulation region information.

@sa 
	tenuM2mScanRegion
*/
typedef struct {
	uint8   u8ScanRegion;
	/*|< Specifies the number of channels allowed in the region (e.g. North America = 11 ... etc.).
	*/
	uint8 __PAD24__[3];

}tstrM2MScanRegion;

/*!
@struct	\
	tstrM2MScan
	
@brief	
	Wi-Fi Scan Request

@sa 
	tenuM2mScanCh
*/
typedef struct {
	uint8 	u8ChNum;
	/*!< The Wi-Fi RF Channel number
	*/
	uint8	__PAD24__[3];
	/*!< Padding bytes for forcing 4-byte alignment
	*/

}tstrM2MScan;


/*!
@struct	\	
	tstrM2mScanDone

@brief	
	Wi-Fi Scan Result
*/
typedef struct{
	uint8 	u8NumofCh;
	/*!< Number of found APs
	*/
	sint8 	s8ScanState;
	/*!< Scan status
	*/
	uint8	__PAD16__[2];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2mScanDone;


/*!
@struct	\
	tstrM2mReqScanResult

@brief	Scan Result Request

	The Wi-Fi Scan results list is stored in Firmware. The application can request a certain scan result by its index.
*/
typedef struct {
	uint8 	u8Index;
	/*!< Index of the desired scan result
	*/
	uint8	__PAD24__[3];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2mReqScanResult;


/*!
@struct	\
	tstrM2mWifiscanResult

@brief	Wi-Fi Scan Result

	Information corresponding to an AP in the Scan Result list identified by its order (index) in the list.
*/
typedef struct {
	uint8 	u8index; 
	/*!< AP index in the scan result list.
	*/
	sint8 	s8rssi; 
	/*!< AP signal strength.
	*/
	uint8 	u8AuthType; 
	/*!< AP authentication type.
	*/
	uint8 	u8ch; 
	/*!< AP RF channel.
	*/
	uint8	au8BSSID[6];
	/*!< BSSID of the AP.
	*/
	uint8 	au8SSID[M2M_MAX_SSID_LEN]; 
	/*!< AP ssid.
	*/
	uint8 	_PAD8_;
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2mWifiscanResult;


/*!
@struct	\		
	tstrM2mWifiStateChanged

@brief		
	Wi-Fi Connection State

@sa			
	M2M_WIFI_DISCONNECTED, M2M_WIFI_CONNECTED, M2M_WIFI_REQ_CON_STATE_CHANGED
*/
typedef struct {
	uint8	u8CurrState;
	/*!< Current Wi-Fi connection state
	*/
	uint8  u8ErrCode;
	/*!< Error type
	*/
	uint8	__PAD16__[2];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2mWifiStateChanged;


/*!
@struct	\
	tstrM2mPsType

@brief		
	Power Save Configuration

@sa
	tenuPowerSaveModes
*/
typedef struct{
	uint8 	u8PsType;
	/*!< Power save operating mode
	*/
	uint8 	u8BcastEn;
	/*!<
	*/
	uint8	__PAD16__[2];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2mPsType;


/*!
@struct	\
	tstrM2mSlpReqTime

@brief		
	Manual power save request sleep time 


*/
typedef struct {
	/*!< Sleep time in ms
	*/
	uint32 u32SleepTime;

} tstrM2mSlpReqTime;

/*!
@struct	\
	tstrM2mLsnInt

@brief	Listen interval 

	It is the value of the Wi-Fi STA listen interval for power saving. It is given in 
	units of Beacon period. Periodically after the listen interval fires, the STA is
	wakeup requesting any buffered frames for it from the AP (Ps-POLL).	
*/
typedef struct {
	uint16 	u16LsnInt;
	/*!< Listen interval in Beacon period count.
	*/
	uint8	__PAD16__[2];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2mLsnInt;


/*!
@struct	\
	tstrM2MWifiMonitorModeCtrl

@brief	Wi-Fi Monitor Mode Filter

	This structure sets the filtering criteria for Wlan packets when monitoring mode is enable. The received packets
	matching the filtering parameters, are passed directly to the application.
*/
typedef struct{
	uint8	u8ChannelID;
	/* RF Channel ID.
	*/
	uint8	u8FrameType;
	/*!< It must use values from tenuWifiFrameType. 
	*/
	uint8	u8FrameSubtype;
	/*!< It must use values from tenuSubTypes.
	*/
	uint8	au8SrcMacAddress[6];
	/* ZERO means DO NOT FILTER Source address.
	*/
	uint8	au8DstMacAddress[6];
	/* ZERO means DO NOT FILTER Destination address.
	*/
	uint8	au8BSSID[6];
	/* ZERO means DO NOT FILTER BSSID.
	*/
	uint8	__PAD24__[3];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2MWifiMonitorModeCtrl;


/*!
@struct	\
	tstrM2MWifiRxPacketInfo

@brief	Wi-Fi RX Frame Header

	The M2M application has the ability to allow Wi-Fi monitoring mode for receiving all Wi-Fi Raw frames matching
	a well defined filtering criteria. When a target Wi-Fi packet is received, the header information are extracted and
	assigned in this structure.
*/
typedef struct{
	uint8	u8FrameType;
	/*!< It must use values from tenuWifiFrameType. 
	*/
	uint8	u8FrameSubtype;
	/*!< It must use values from tenuSubTypes.
	*/
	uint8	u8ServiceClass;
	/*!< Service class from Wi-Fi header.
	*/
	uint8	u8Priority;
	/*!< Priority from Wi-Fi header.
	*/
	uint8	u8HeaderLength;
	/*!< Frame Header length.
	*/
	uint8	u8CipherType;
	/*!< Encryption type for the rx packet.
	*/
	uint8	au8SrcMacAddress[6];
	/* ZERO means DO NOT FILTER Source address.
	*/
	uint8	au8DstMacAddress[6];
	/* ZERO means DO NOT FILTER Destination address.
	*/
	uint8	au8BSSID[6];
	/* ZERO means DO NOT FILTER BSSID.
	*/
	uint16	u16DataLength;
	/*!< Data payload length (Header excluded).
	*/
	uint16	u16FrameLength;
	/*!< Total frame length (Header + Data).
	*/
	uint32	u32DataRateKbps;
	/*!< Data Rate in Kbps.
	*/
	sint8		s8RSSI;
	/*!< RSSI.
	*/
	uint8	__PAD24__[3];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2MWifiRxPacketInfo;


/*!
@struct	\
	tstrM2MWifiTxPacketInfo

@brief	Wi-Fi Tx Packet Info

	The M2M Application has the ability to compose a RAW Wi-Fi frames (under the application responsibility). When
	transmitting a Wi-Fi packet, the application must supply the firmware with this structure for sending the target frame.
*/
typedef struct{
	uint16	u16PacketSize;
	/*!< Wlan frame length.
	*/
	uint16	u16HeaderLength;
	/*!< Wlan frame header length.
	*/
}tstrM2MWifiTxPacketInfo;


/*!
 @struct	\	
 	tstrM2MP2PConnect

 @brief		
 	Set the device to operate in the Wi-Fi Direct (P2P) mode.
*/
typedef struct {
	uint8 	u8ListenChannel; 
	/*!< P2P Listen Channel (1, 6 or 11)
	*/
	uint8	__PAD24__[3];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2MP2PConnect;




/*!
@struct	\
	tstrM2MAPConfig

@brief	AP Configuration

	This structure holds the configuration parameters for the M2M AP mode. It should be set by the application when
	it requests to enable the M2M AP operation mode. The M2M AP mode currently supports only WEP security (with
	the NO Security option availabe of course).
*/
typedef struct {
	/*!<
		Configuration parameters for the WiFi AP.
	*/
	uint8 	au8SSID[M2M_MAX_SSID_LEN]; 
	/*!< AP SSID
	*/
	uint8 	u8ListenChannel; 
	/*!< Wi-Fi RF Channel which the AP will operate on
	*/
	uint8	u8KeyIndx; 
	/*!< Wep key Index
	*/
	uint8	u8KeySz; 
	/*!< Wep key Size 
	*/
	uint8	au8WepKey[WEP_104_KEY_STRING_SIZE + 1]; 
	/*!< Wep key 
	*/
	uint8 	u8SecType; 
	/*!< Security type: Open or WEP only in the current implementation
	*/
	uint8 	u8SsidHide;
	/*!< SSID Status "Hidden(1)/Visible(0)"
	*/
	uint8	au8DHCPServerIP[4];
	/*!< Ap IP server address
	*/
	uint8	__PAD24__[3];
	/*!< Padding bytes for forcing alignment
	*/
}tstrM2MAPConfig;



/*!
@struct	\	
	tstrM2mServerInit

@brief	
	PS Server initialization.
*/
typedef struct {
	uint8 	u8Channel;
	/*!< Server Listen channel
	*/
	uint8	__PAD24__[3];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2mServerInit;


/*!
@struct	\	
	tstrM2mClientState

@brief	
	PS Client State.
*/
typedef struct {
	uint8 	u8State;
	/*!< PS Client State
	*/
	uint8	__PAD24__[3];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2mClientState;


/*!
@struct	\	
	tstrM2Mservercmd

@brief
	PS Server Cmd
*/
typedef struct {
	uint8	u8cmd; 
	/*!< PS Server Cmd
	*/
	uint8	__PAD24__[3];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2Mservercmd;


/*!
@struct	\
	tstrM2mSetMacAddress
	
@brief		
	Sets the MAC address from application. It is only intended for testing purpose.
	This method is not used for production SW. Production SW reads MAC Address from EFUSE.

@note		
	It's recommended to call this only before calling connect
*/
typedef struct {
	uint8 	au8Mac[6]; 
	/*!< MAC address array 
	*/
	uint8	__PAD16__[2];
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2mSetMacAddress;


/*!
@struct	\
 	tstrM2MDeviceNameConfig
 	
@brief	Device name

	It is assigned by the application. It is used mainly for Wi-Fi Direct device 
	discovery and WPS device information.
*/
typedef struct {
	uint8 	au8DeviceName[M2M_DEVICE_NAME_MAX];
	/*!< NULL terminated device name
	*/
}tstrM2MDeviceNameConfig;


/*!
@struct	\	
 	tstrM2MIPConfig
 	
@brief		
 	Static IP configuration.

@note
 	All member IP addresses are expressed in Network Byte Order (eg. "192.168.10.1" will be expressed as 0x010AA8C0).
 */
typedef struct {
	uint32 	u32StaticIP;
	/*!< The static IP assigned to the device.
	*/
	uint32 	u32Gateway;
	/*!< IP of the Default internet gateway.
	*/
	uint32 	u32DNS;
	/*!< IP for the DNS server.
	*/
	uint32 	u32SubnetMask;
	/*!< Subnet mask for the local area network.
	*/
} tstrM2MIPConfig;

/*!
@struct	\	
 	tstrM2mIpRsvdPkt
 	
@brief		
 	Received Packet Size and Data Offset

 */
typedef struct{
	uint16	u16PktSz;
	uint16	u16PktOffset;
} tstrM2mIpRsvdPkt;


/*!
@struct	\	
 	tstrM2MProvisionModeConfig
 	
@brief		
 	M2M Provisioning Mode Configuration
 */

typedef struct {
	tstrM2MAPConfig	strApConfig;
	/*!<
		Configuration parameters for the WiFi AP.
	*/
	char				acHttpServerDomainName[64];
	/*!<
		The device domain name for HTTP provisioning.
	*/
	uint8			u8EnableRedirect;
	/*!<
		A flag to enable/disable HTTP redirect feature for the HTTP Provisioning server. If the Redirect is enabled,
		all HTTP traffic (http://URL) from the device associated with WINC AP will be redirected to the HTTP Provisioning Web page.
		- 0 : Disable HTTP Redirect.
		- 1 : Enable HTTP Redirect. 
	*/
	uint8			__PAD24__[3];
}tstrM2MProvisionModeConfig;


/*!
@struct	\	
 	tstrM2MProvisionInfo
 	
@brief		
 	M2M Provisioning Information obtained from the HTTP Provisioning server.
 */
typedef struct{
	uint8	au8SSID[M2M_MAX_SSID_LEN];
	/*!<
		Provisioned SSID.
	*/
	uint8	au8Password[M2M_MAX_PSK_LEN];
	/*!<
		Provisioned Password.
	*/
	uint8	u8SecType;
	/*!<
		Wifi Security type.
	*/
	uint8	u8Status;
	/*!<
		Provisioning status. It must be checked before reading the provisioning information. It may be
		- M2M_SUCCESS 	: Provision successful.
		- M2M_FAIL		: Provision Failed.
	*/
}tstrM2MProvisionInfo;


/*!
@struct	\	
 	tstrM2MProvisionInfo
 	
@brief		
 	M2M Provisioning Information obtained from the HTTP Provisioning server.
 */
typedef struct{
	char		acSSID[M2M_MAX_SSID_LEN];
	uint8	u8SecType;
	uint8	au8IPAddr[4];
	sint8		s8RSSI;
	uint8	__PAD8__;
}tstrM2MConnInfo;

/*!
@struct	\	
 	tstrOtaInitHdr
 	
@brief		
 	OTA Header 
 */

typedef struct{
	uint32 u32OtaMagicValue;
	uint32 u32OtaPayloadSzie;

}tstrOtaInitHdr;
/*!
@struct	\	
 	tstrOtaControlSec
 	
@brief		
 	Control section 
 */

typedef struct {
	uint32 u32OtaMagicValue;
	uint32 u32OtaFormatVersion;
	uint32 u32OtaSequenceNumber;
	uint32 u32OtaLastCheckTime;
	uint32 u32OtaCurrentworkingImagOffset;
	uint32 u32OtaCurrentworkingImagFirmwareVer;
	uint32 u32OtaRollbackImageOffset;
	uint32 u32OtaRollbackImageValidStatus;
	uint32 u32OtaRollbackImagFirmwareVer;
	uint32 u32OtaControlSecCrc;
} tstrOtaControlSec;

/*!
@enum	\
	tenuOtaUpdateStatus

@brief
	OTA return status 
*/
typedef enum {
	OTA_STATUS_SUCSESS        = 0,   
	/*!< OTA Success with not errors. */
	OTA_STATUS_FAIL           = 1,  
	/*!< OTA generic fail. */
	OTA_STATUS_INVAILD_ARG    = 2,
	/*!< Invalid or malformed download URL. */
	OTA_STATUS_INVAILD_RB_IMAGE    = 3,
	/*!< Invalid rollback image. */
	OTA_STATUS_INVAILD_FLASH_SIZE    = 4,
	/*!< Flash size on device is not enough for OTA. */
	OTA_STATUS_AlREADY_ENABLED    = 5,
	/*!< An OTA operation is already enabled. */
	OTA_STATUS_UPDATE_INPROGRESS    = 6,
	/*!< An OTA operation update is in progress */
} tenuOtaUpdateStatus;
/*!
@enum	\
	tenuOtaUpdateStatusType

@brief
	OTA update Status type
*/
typedef enum { 
	/*!< Download status
	*/
	DL_STATUS        = 1,   
	/*!< Download status
	*/
	SW_STATUS        = 2,   
	/*!< Software upgrade status
	*/
	RB_STATUS        = 3,   
	/*!< Rollback status
	*/
}tenuOtaUpdateStatusType;


/*!
@struct	\
	tstrOtaUpdateInfo

@brief	
	OTA Update Information

@sa 
	tenuWPSTrigger
*/
typedef struct {
	uint8	u8OtaUpdateStatusType;
	/*!< 
	*/
	uint8	u8OtaUpdateStatus;
	/*!< 
	*/
	uint8 _PAD16_[2];
}tstrOtaUpdateStatusResp;

/*!
@struct	\
	tstrOtaUpdateInfo

@brief	
	OTA Update Information

@sa 
	tenuWPSTrigger
*/
typedef struct {
	uint32	u8NcfUpgradeVersion;
	/*!< NCF OTA Upgrade Version 
	*/
	uint32	u8NcfCurrentVersion;
	/*!< NCF OTA Current firmware version 
	*/
	uint32	u8NcdUpgradeVersion;
	/*!< NCD (host) upgraded version (if the u8NcdRequiredUpgrade == true)
	*/
	uint8	u8NcdRequiredUpgrade;
	/*!< NCD Required upgrade to the above version 
	*/
	uint8 	u8DownloadUrlOffset;
	/*!< Download URL offset in the received packet
	*/
	uint8 	u8DownloadUrlSize;
	/*!< Download URL size in the received packet
	*/
	uint8	__PAD8__;
	/*!< Padding bytes for forcing 4-byte alignment
	*/
} tstrOtaUpdateInfo;

/*!
@struct	\	
	tstrSystemTime

@brief
*/
typedef struct{
	uint16	u16Year;
	uint8	u8Month;
	uint8	u8Day;
	uint8	u8Hour;
	uint8	u8Minute;
	uint8	u8Second;
}tstrSystemTime;

/*!
@struct	\	
 	tstrM2MMulticastMac
 	
@brief		
 	M2M add/remove multicast mac address
 */
 typedef struct {
	uint8 au8macaddress[M2M_MAC_ADDRES_LEN];
	/*!<
		Mac address needed to be added or removed from filter.
	*/
	uint8 u8AddRemove;
	/*!<
		set by 1 to add or 0 to remove from filter.
	*/
	uint8	__PAD8__;
	/*!< Padding bytes for forcing 4-byte alignment
	*/
}tstrM2MMulticastMac;

#endif
