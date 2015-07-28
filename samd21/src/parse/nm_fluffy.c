/*
 * nm_fluffy.c
 *
 * Created: 2014-11-05
 *  Author: jini
 */

 
#include "asf.h"
#include "nm_fluffy.h"
#include "common/include/nm_common.h"
#include "driver/include/m2m_wifi.h"
#include "parse_impl.h"


/** AP mode Settings */
#define AP_WLAN_SSID				"TL03-PUFFY-"
#define AP_WLAN_AUTH				M2M_WIFI_SEC_OPEN
#define AP_WLAN_AP_CHANNEL		(6)


typedef struct {
	char apSSID[M2M_MAX_SSID_LEN];
	char apPW[M2M_MAX_PSK_LEN];
	uint8_t apSecType;
	uint8_t loaded;
} tHomeAPInfo;

/** Wi-Fi connection state */
static uint8_t gWifiConnected = 0;
uint8_t gIpAddressFromAp[36] = { 0, };

static tHomeAPInfo gHomeAPInfo;

#define MAX_KEY_COUNT	16

struct addKeys{
	char* key[ MAX_KEY_COUNT ];
	char* label[ MAX_KEY_COUNT ];
	char* value[ MAX_KEY_COUNT ];
	
	uint8_t count;
};

struct addKeys gAddKeys;


/** NVM Address */
//#define NVM_ADDR(x)							(NVMCTRL_PAGE_SIZE * NVMCTRL_PAGES) - (NVMCTRL_ROW_SIZE * x)
#define NVM_ADDR(x)							NVMCTRL_ROW_SIZE * (NVMCTRL_PAGES/NVMCTRL_ROW_PAGES - x)
#define NVM_ADDR_ROW_INTERVAL(x)			NVMCTRL_ROW_SIZE * x
#define NVM_ADDR_HOME_AP_INFO_SAVED			NVM_ADDR(1)
#define NVM_ADDR_FLUFFY_INFO_SAVED			NVM_ADDR(2)
#define NVM_ADDR_HOME_AP_SSID				NVM_ADDR(3)
#define NVM_ADDR_HOME_AP_SECTYPE			NVM_ADDR(4)
#define NVM_ADDR_HOME_AP_PASSWORD			NVM_ADDR(5)
#define NVM_ADDR_FLUFFY_KEY					NVM_ADDR(6)
#define NVM_ADDR_FLUFFY_KEY_VALUE			NVM_ADDR_FLUFFY_KEY - NVM_ADDR_ROW_INTERVAL(MAX_KEY_COUNT)


void fluffy_wifi_noti_cb(uint8_t u8MsgType, void * pvMsg)
{
	switch (u8MsgType) 
	{
		/* M2M_STA_CMD_BASE */
		case M2M_WIFI_RESP_CON_STATE_CHANGED:
		{
			tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)pvMsg;
			if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED)
			{
				printf("wifi_cb: [STA] M2M_WIFI_RESP_CON_STATE_CHANGED: CONNECTED\r\n");
				m2m_wifi_request_dhcp_client();
			}
			else if (pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED)
			{
				printf("wifi_cb: [STA] M2M_WIFI_RESP_CON_STATE_CHANGED: DISCONNECTED\r\n");
				if (gWifiConnected)
				{
					gWifiConnected = 0;
					m2m_wifi_connect((char *)gHomeAPInfo.apSSID,
									strlen(gHomeAPInfo.apSSID),
									gHomeAPInfo.apSecType,
									(char *)gHomeAPInfo.apPW,
									M2M_WIFI_CH_ALL);
				}
			}

			break;
		}

		case M2M_WIFI_REQ_DHCP_CONF:
		{
			uint8_t *pu8IPAddress = (uint8_t *)pvMsg;
			gWifiConnected = 1;
			
			printf("wifi_cb: [STA] M2M_WIFI_REQ_DHCP_CONF: IP is %u.%u.%u.%u\r\n",
			pu8IPAddress[0], pu8IPAddress[1], pu8IPAddress[2], pu8IPAddress[3]);

			memset( gIpAddressFromAp, 0, 36 );
			sprintf( gIpAddressFromAp, "%d.%d.%d.%d", pu8IPAddress[0], pu8IPAddress[1], pu8IPAddress[2], pu8IPAddress[3] );

			break;
		}
		
		case M2M_WIFI_REQ_DISCONNECT:
		{
			printf("wifi_cb: [AP] M2M_WIFI_REQ_DISCONNECT\r\n");
			gWifiConnected = 0;
			break;
		}
		
		/* M2M_AP_CMD_BASE */
		case M2M_WIFI_REQ_ENABLE_AP:
		{
			printf("wifi_cb: [AP] M2M_WIFI_REQ_ENABLE_AP\r\n");
			break;
		}
		
		case M2M_WIFI_REQ_DISABLE_AP:
		{
			printf("wifi_cb: [AP] M2M_WIFI_REQ_DISABLE_AP\r\n");
			break;
		}
		
		default:
		{
			break;
		}
	}
}

static uint8_t load_home_ap_info(void)
{
	char *cp = NULL;
	uint8_t buf[NVMCTRL_PAGE_SIZE] = {0, };
	uint8_t saved = false;
		
	enum status_code status = nvm_read_buffer(NVM_ADDR_HOME_AP_INFO_SAVED, &saved, 1);

	if(status != STATUS_OK) {
		printf("load_home_ap_info / read nvm error (0x%x) \r\n", status);
		return false;
	}
	
	if(saved != true) {
		printf("load_home_ap_info / ap info is not exist in nvm. \r\n");
		return false;
	}

	char apSSID_Temp[M2M_MAX_SSID_LEN] = { 0, };
	char apPW_Temp[M2M_MAX_PSK_LEN] = { 0, };
	uint8_t apSecType_Temp = 0;

	status = nvm_read_buffer(NVM_ADDR_HOME_AP_SSID, apSSID_Temp, M2M_MAX_SSID_LEN - 1);
	if(status != STATUS_OK) {
		printf("load_home_ap_info / read HOME_AP_SSID error (0x%x)\r\n", status);
		return false;
	}

	status = nvm_read_buffer(NVM_ADDR_HOME_AP_SECTYPE, &apSecType_Temp, 1);
	if(status != STATUS_OK) {
		printf("load_home_ap_info / read HOME_AP_SECTYPE error (0x%x)\r\n", status);
		return false;
	}
	
	status = nvm_read_buffer(NVM_ADDR_HOME_AP_PASSWORD, apPW_Temp, M2M_MAX_PSK_LEN - 1);
	if(status != STATUS_OK) {
		printf("load_home_ap_info / read HOME_AP_PASSWORD error (0x%x)\r\n", status);
		return false;
	}

#if 1
	printf("load_home_ap_info / SSID (%s)\r\n", apSSID_Temp);
	printf("load_home_ap_info / PW (%s)\r\n", apPW_Temp);
	printf("load_home_ap_info / SecType (%d)\r\n", apSecType_Temp);
#endif

	fluffy_set_home_ap_info(apSSID_Temp, apPW_Temp, apSecType_Temp, false);
		
	return true;
}

uint8_t fluffy_load_fluffy_info(void)
{
	uint8_t count = 0;
	
	enum status_code status = nvm_read_buffer(NVM_ADDR_FLUFFY_INFO_SAVED, &count, 1);
	
	if(count == 255)
		count = 0;
		
	if(status != STATUS_OK) {
		printf("fluffy_load_fluffy_info / nvm_read_buffer error (%d) fail\r\n", status);
		return false;
	}
	
	if(count <= 0) {
		printf("fluffy_load_fluffy_info / fluffy info is not exist in nvm.\r\n");
		return false;
	}

	for(int i=0; i<count; i++) {
		uint8_t key[NVMCTRL_PAGE_SIZE] = {0, }, value[NVMCTRL_PAGE_SIZE] = {0, };
		status = nvm_read_buffer(NVM_ADDR_FLUFFY_KEY - NVM_ADDR_ROW_INTERVAL(i), key, NVMCTRL_PAGE_SIZE);
		if(status == STATUS_OK)
			status = nvm_read_buffer(NVM_ADDR_FLUFFY_KEY_VALUE - NVM_ADDR_ROW_INTERVAL(i), value, NVMCTRL_PAGE_SIZE);
			
		if(status != STATUS_OK)
			break;
		else {
			fluffy_device_config_add_key(key, NULL, value);
		}
			
#if 0
		printf("fluffy_load_fluffy_info / key = %s, value = %s \r\n", gAddKeys.key[ i ], gAddKeys.value[ i ]);
#endif			
	}

	printf( "fluffy_load_fluffy_info / %d\r\n", gAddKeys.count );
	
	return true;
}

uint8_t fluffy_check_home_ap(char* ssid, char* pw, uint8_t secType)
{
	memset(&gAddKeys, 0, sizeof(gAddKeys) );

	if( ssid )
	{
		fluffy_set_home_ap_info(ssid, pw, secType, false);
		return 1;
	}

	/* To do : Check Saved file for Home AP Information */
	if (load_home_ap_info())
	{
		fluffy_load_fluffy_info();
		return 1;
	}
	else
	{
		return 0;
	}
}

void fluffy_set_home_ap_info(char* ssid, char* pw, uint8_t secType, uint8_t fileWrite)
{
	if( !ssid || !pw )
	{
		printf("fluffy_set_home_ap_info / Input param error !!!!!\r\n");
		return;
	}
	
	memset(&gHomeAPInfo, 0, sizeof(tHomeAPInfo));
	
	strcpy(gHomeAPInfo.apSSID, ssid);
	strcpy(gHomeAPInfo.apPW, pw);
	gHomeAPInfo.apSecType = secType;
	gHomeAPInfo.loaded = 1;

	/* file write : home ap information */
	if( fileWrite )
	{
		uint8_t saved = true;
		
		enum status_code status = nvm_write_buffer(NVM_ADDR_HOME_AP_INFO_SAVED, &saved, 1);
		
		if(status == STATUS_OK)
			status = nvm_write_buffer(NVM_ADDR_HOME_AP_SSID, ssid, strlen(ssid) + 1);
		
		if(status == STATUS_OK)
			status = nvm_write_buffer(NVM_ADDR_HOME_AP_PASSWORD, pw, strlen(pw) + 1);
			
		if(status == STATUS_OK)
			status = nvm_write_buffer(NVM_ADDR_HOME_AP_SECTYPE, &secType, 1);
		
		if(status != STATUS_OK)
			printf("fluffy_set_home_ap_info / nvm write Error!!!!!\r\n");
	}
}

int fluffy_remove_home_ap_info(void)
{
	enum status_code status = nvm_erase_row(NVM_ADDR_HOME_AP_INFO_SAVED);
		
	if(status == STATUS_OK)
		status = nvm_erase_row(NVM_ADDR_HOME_AP_SSID);
		
	if(status == STATUS_OK)
		status = nvm_erase_row(NVM_ADDR_HOME_AP_SECTYPE);
		
	if(status == STATUS_OK)
		status = nvm_erase_row(NVM_ADDR_HOME_AP_PASSWORD);
		
	if(status != STATUS_OK)
		printf("fluffy_remove_home_ap_info / nvm erase Error!!!!!\r\n");

	return true;
}

void fluffy_set_fluffy_info( uint8_t fileWrite )
{
	if( fileWrite )
	{
		enum status_code status;
		
		fluffy_remove_fluffy_info();
	
		status = nvm_write_buffer(NVM_ADDR_FLUFFY_INFO_SAVED, &(gAddKeys.count), 1);
		
		if(status == STATUS_OK) {
			for( int i = 0; i < gAddKeys.count; i++ )
			{
				status = nvm_write_buffer(NVM_ADDR_FLUFFY_KEY - NVM_ADDR_ROW_INTERVAL(i), gAddKeys.key[ i ], NVMCTRL_PAGE_SIZE);

				if(status == STATUS_OK)
					status = nvm_write_buffer(NVM_ADDR_FLUFFY_KEY_VALUE - NVM_ADDR_ROW_INTERVAL(i), gAddKeys.value[ i ], NVMCTRL_PAGE_SIZE);
					
				if(status != STATUS_OK)
					break;

#if 0
				printf("fluffy_set_fluffy_info / write ok. key = %s, value = %s\r\n", gAddKeys.key[ i ], gAddKeys.value[ i ]);
#endif
			}
		}

		if(status != STATUS_OK)
		{
			printf("fluffy_set_fluffy_info / nvm write error ( %d ) Error!!!!!\r\n", status );
		}
	}
}

int fluffy_remove_fluffy_info(void)
{	
	enum status_code status = nvm_erase_row(NVM_ADDR_FLUFFY_INFO_SAVED);
	
	if(status == STATUS_OK) {
		for(int i=0; i<MAX_KEY_COUNT; i++) {
			status = nvm_erase_row( NVM_ADDR_FLUFFY_KEY - NVM_ADDR_ROW_INTERVAL(i) );
			if(status == STATUS_OK)
				status = nvm_erase_row( NVM_ADDR_FLUFFY_KEY_VALUE - NVM_ADDR_ROW_INTERVAL(i) );
			else
				break;
		}
	}
	
	if(status != STATUS_OK)
		printf("fluffy_remove_fluffy_info / nvm erase error!!! \r\n");

	return true;
}

int fluffy_device_config_add_key( const char* key, const char* label, const char* value )
{
	if( key == NULL || (label == NULL && value == NULL) )
		return false;
	
	int keyLength = strlen( key );
	int labelLength = strlen( label );
	int valueLength = strlen( value );
	
	gAddKeys.key[ gAddKeys.count ] = malloc( keyLength + 1 );
	strcpy( gAddKeys.key[ gAddKeys.count ], key );
	gAddKeys.key[ gAddKeys.count ][ keyLength ] = 0;
	
	if(label) {
		gAddKeys.label[ gAddKeys.count ] = malloc( labelLength + 1 );
		strcpy( gAddKeys.label[ gAddKeys.count ], label );
		gAddKeys.label[ gAddKeys.count ][ labelLength ] = 0;
	}
	
	if(value) {
		gAddKeys.value[ gAddKeys.count ] = malloc( valueLength + 1 );
		strcpy( gAddKeys.value[ gAddKeys.count ], value );
		gAddKeys.value[ gAddKeys.count ][ valueLength ] = 0;
	}

	gAddKeys.count++;

	return true;
}

const char * fluffy_device_config_get_value( const char *key )
{
	if( key == NULL )
		return NULL;
	
	for( int i = 0; i < gAddKeys.count; i++ )
	{
		if( 0 == strcmp( key, gAddKeys.key[ i ] ) )
		{
			return gAddKeys.value[ i ];
		}
	}

	return NULL;
}

const char * fluffy_device_config_get_value_by_index( int index )
{
	return gAddKeys.value[ index ];
}

void fluffy_device_config_set_value( int index, char* value )
{
	if( gAddKeys.count <= index )
		return;

	gAddKeys.value[ index ] = malloc( strlen( value ) + 1 );
	memset( gAddKeys.value[ index ], 0, strlen( value ) + 1 );
	strcpy( gAddKeys.value[ index ], value );
}

const char * fluffy_device_config_get_key( int index )
{
	return gAddKeys.key[ index ];
}

const char * fluffy_device_config_get_label( int index )
{
	return gAddKeys.label[ index ];
}

int fluffy_device_config_get_count( void )
{
	return gAddKeys.count;
}

uint8_t fluffy_get_wifi_connected(void)
{
	return gWifiConnected;
}

uint8_t fluffy_get_ap_mode_completed(void)
{
	return gHomeAPInfo.loaded;
}

/************************************************************************/
/* WIFI AP Mode                                                         */
/************************************************************************/
void fluffy_wifi_ap_mode(int8_t isAPEnable, uint8_t* macAddr)
{
	int8_t ret = M2M_SUCCESS;
	tstrM2MAPConfig strM2MAPConfig;
	
	gWifiConnected = 0;
	
	if (isAPEnable)
	{
		char ssidMac[sizeof(AP_WLAN_SSID) + 5] = {0,};
			
		printf("=== AP Mode, Start ===\r\n");
		
		if (macAddr)
		{
			sprintf(ssidMac, "%s%02X%02X", AP_WLAN_SSID, macAddr[0], macAddr[1]);
			printf("ssid : %s\r\n", ssidMac);
			printf("You can connect to %s.\r\n", ssidMac);			
		}
		else
		{
			sprintf(ssidMac, "%sXXYY", AP_WLAN_SSID);
			printf("Wrong, MAC Address is NULL\r\n");
		}
		
		/* Initialize AP mode parameters structure with SSID, channel and OPEN security type. */
		memset(&strM2MAPConfig, 0x00, sizeof(tstrM2MAPConfig));
		strcpy((char *)&strM2MAPConfig.au8SSID, ssidMac);
		strM2MAPConfig.u8ListenChannel = AP_WLAN_AP_CHANNEL;
		strM2MAPConfig.u8SecType = AP_WLAN_AUTH;
		strM2MAPConfig.au8DHCPServerIP[0] = 0xC0; /* 192 */
		strM2MAPConfig.au8DHCPServerIP[1] = 0xA8; /* 168 */
		strM2MAPConfig.au8DHCPServerIP[2] = 0x01; /* 1 */
		strM2MAPConfig.au8DHCPServerIP[3] = 0x01; /* 1 */

		/* Bring up AP mode with parameters structure. */
		ret = m2m_wifi_enable_ap(&strM2MAPConfig);
		if (ret != M2M_SUCCESS)
		{
			printf("Error, enable AP Mode(%d)\r\n", ret);
		}
	}
	else
	{
		/* Stop AP mode. */
		ret = m2m_wifi_disable_ap();
		if (ret != M2M_SUCCESS) 
		{
			printf("m2m_wifi_disable_ap call error!\r\n");
		}

		printf("=== AP mode, End ===\r\n");
	}	
}


/************************************************************************/
/* WIFI Station Mode                                                    */
/************************************************************************/
void fluffy_wifi_stat_mode(int8_t isStaEnable)
{
	int8_t ret = M2M_SUCCESS;
	
	if (isStaEnable)
	{
		printf("=== Station Mode, Start, %s ===\r\n", gHomeAPInfo.apSSID);

		/* Connect to router. */
		ret = m2m_wifi_connect((char *)gHomeAPInfo.apSSID,
								strlen(gHomeAPInfo.apSSID),
								gHomeAPInfo.apSecType,
								(char *)gHomeAPInfo.apPW,
								M2M_WIFI_CH_ALL);
		if (ret != M2M_SUCCESS)
		{
			printf("m2m_wifi_connect call error!\r\n");
		}
	}
	else
	{
		gWifiConnected = 0;
		
		ret = m2m_wifi_disconnect();
		if (ret != M2M_SUCCESS)
		{
			printf("m2m_wifi_disconnect call error!\r\n");
		}
		
		printf("=== Station Mode, End ===\r\n");
	}
}
