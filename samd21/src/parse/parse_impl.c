/*
 * parse.c
 *
 * Created: 2014-11-26 오후 12:29:15
 *  Author: Atmel
 */

#include "parse_impl.h"
#include "asf.h"
#include "driver/include/m2m_wifi.h"
#include "driver/source/nmasic.h"
#include "nm_fluffy.h"
#include "nm_fluffy_trans.h"
#include "mdns/mdnsd.h"


//#define ENABLE_MDNS

#ifdef ENABLE_MDNS
#define TIMER_CB_INTERVAL			3 // second
/** Instance of Timer module. */
struct tcc_module tempTccModule;
static int gTimerCbMdns = 0;
#endif

#define DELAY_FOR_EVENT_LOOP		(50)
#define DELAY_FOR_MODE_CHANGE		(500)


static uint8_t gParseMode = M2M_WIFI_MODE_UNKNOWN;


/****************************************************************************/
/* Local Functions */
/****************************************************************************/

#ifdef ENABLE_MDNS
/**
 * \brief Timer callback function.
 */
static void timer_cb(struct tcc_module *const module)
{
	gTimerCbMdns = 1;
}

/**
 * \brief Configure Timer module.
 */
static void configure_timer(void)
{
	/**
	 * Timer period is 1s = Prescaler(256) * Period(46875) / Clock(12Mhz).
	 */
	struct tcc_config tcc_conf;
	tcc_get_config_defaults(&tcc_conf, TCC0);
	tcc_conf.counter.period = 46875 * TIMER_CB_INTERVAL;
	tcc_conf.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV256;

	tcc_init(&tempTccModule, TCC0, &tcc_conf);
	/* Register timer callback for the Request RSSI. */
	tcc_register_callback(&tempTccModule, timer_cb, TCC_CALLBACK_CHANNEL_3);
	tcc_enable(&tempTccModule);
}

static void deinit_timer(void)
{
	tcc_disable_callback(&tempTccModule, TCC_CALLBACK_CHANNEL_3);
	tcc_disable(&tempTccModule);
}
#endif

static volatile int s_request_in_process = 0;
static parseRequestCallback currentCallback = NULL;
static void parseCommonRequestCallback(ParseClient client, int error, int httpStatus, const char* httpResponseBody) {
  parseRequestCallback tmpCallback = currentCallback;
  s_request_in_process = 0;
  currentCallback = NULL;
  if (tmpCallback)
    tmpCallback(client, error, httpStatus, httpResponseBody);
}

void localWaitForRequestToFinish(ParseClientInternal* parseClient) {
  while (s_request_in_process) {
    localParseEventLoop(parseClient);
  }
}


/****************************************************************************/
/* Public Functions */
/****************************************************************************/

LocalParseStatus initWifiModule(void)
{
	sint8 ret = 0;
	tstrWifiInitParam param;

	/* Initialize the BSP. */
	nm_bsp_init();

	/* Initialize WIFI parameters structure. */
	memset((uint8_t *)&param, 0, sizeof(tstrWifiInitParam));
	param.pfAppWifiCb = fluffy_wifi_noti_cb;

	/* Initialize WINC1500 WIFI driver with data and status callbacks. */
	ret = m2m_wifi_init(&param);
	if (M2M_SUCCESS != ret)
	{
		printf("initWifiModule / m2m_wifi_init call error!(%d)\r\n", ret);
		return LOCAL_PARSE_ERROR_UNKNOWN;
	}

	/* Initialize Socket module */
	fluffy_trans_init_socket();

#ifdef ENABLE_MDNS
	/* Initialize the Timer. */
	configure_timer();
#endif

	return LOCAL_PARSE_SUCCESS;
}

LocalParseStatus loadSettingsOrProvisionBoard(char* ssid, char* pw, int secType)
{
	/* Check AP Information */
	if (fluffy_check_home_ap(ssid, pw, (uint8_t)secType))
	{
		gParseMode = M2M_WIFI_MODE_STA_ENABLE;
		return LOCAL_PARSE_SUCCESS;
	}
	else
	{
		gParseMode = M2M_WIFI_MODE_AP_ENABLE;
		return LOCAL_PARSE_PROVISIONING;
	}
}

LocalParseStatus localParseDeinitialize(void)
{
#ifdef ENABLE_MDNS
	deinit_timer();
#endif

	fluffy_trans_socket_close();

	m2m_wifi_deinit(NULL);

	nm_bsp_deinit();
	return LOCAL_PARSE_SUCCESS;
}

LocalParseStatus localParseSetRequestCallback(ParseClientInternal* parseClient, parseRequestCallback callback, void* inParam)
{
  localWaitForRequestToFinish(parseClient);
  currentCallback = callback;

  fluffy_trans_set_parse_request_callback(parseCommonRequestCallback, inParam);
  return LOCAL_PARSE_SUCCESS;
}

LocalParseStatus localParseSetPushCallback(parsePushCallback callback, void* inParam)
{
	fluffy_trans_set_parse_push_callback(callback, inParam);
	return LOCAL_PARSE_SUCCESS;
}

LocalParseStatus localParseSendRequest(ParseClientInternal* parseClient, char* httpVerb, char* httpPath, char* httpRequestBody, int addInstallation)
{
  s_request_in_process = 1;
  if( M2M_SUCCESS != fluffy_trans_send(parseClient, httpVerb, httpPath, httpRequestBody, addInstallation) )
  {
    s_request_in_process = 0;
    printf("localParseSendRequest / fluffy_trans_send call error !!!!!\r\n");
    return LOCAL_PARSE_ERROR_UNKNOWN;
  }

  return LOCAL_PARSE_SUCCESS;
}

void deviceConfigAddKey( int handle, const char* key, const char* label, const char* value )
{
	fluffy_device_config_add_key( key, label, value );
	return LOCAL_PARSE_SUCCESS;
}

const char * deviceConfigGetValue( int handle, const char *key )
{
	return fluffy_device_config_get_value( key );
}

void deviceConfigSetValue(int handle, const char *key, const char *value) {
  int total_keys = fluffy_device_config_get_count();
  int index = -1;
  for (int i = 0; i < total_keys; ++i) {
    if (!strcmp(fluffy_device_config_get_key(i), key)) {
      index = i;
      break;
    }
  }
  if (index == -1) {
    index = total_keys;
    deviceConfigAddKey(handle, key, key, NULL);
  }
  fluffy_device_config_set_value(index, value);
}

void deviceConfigCommitValues(int handle) {
  fluffy_remove_fluffy_info();
  fluffy_set_fluffy_info(true);
}

tenuWifiMode localParseEventLoop(ParseClientInternal *parseClient)
{
	tenuWifiMode wifiStatus = M2M_WIFI_MODE_UNKNOWN;

	while (m2m_wifi_handle_events(NULL) != M2M_SUCCESS) {
	}

#ifdef ENABLE_MDNS
	if(gTimerCbMdns == 1)
	{
		gTimerCbMdns = 0;
		printf("Send mDNS====\r\n");
		mdnsd_send_response();
	}
#endif

	switch (gParseMode)
	{
		case M2M_WIFI_MODE_UNKNOWN:
			break;

		case M2M_WIFI_MODE_AP_ENABLE:
		{
			uint8 macAddr[6] = {0,};
			uint8_t macInfo[2] = {0,};

			m2m_wifi_get_mac_address(macAddr);
			macInfo[0] = macAddr[4];
			macInfo[1] = macAddr[5];

			deviceConfigAddKey( 0, "applicationId", "App ID", NULL );
			deviceConfigAddKey( 0, "clientKey", "Client Key", NULL );
			deviceConfigAddKey( 0, "installationId", "Installation ID", NULL );
			deviceConfigAddKey( 0, "sessionToken", "Session Token", NULL );
			deviceConfigAddKey( 0, "deviceName", "Device Name", NULL );

			/* AP Mode : Enable*/
			fluffy_wifi_ap_mode(1, macInfo);
			gParseMode = M2M_WIFI_MODE_AP_PROV;

			wifiStatus = M2M_WIFI_MODE_AP_PROV;
			break;
		}

		case M2M_WIFI_MODE_AP_PROV:
		{
			/* Provision AP or HTTP */
			fluffy_trans_ap_provision();
			gParseMode = M2M_WIFI_MODE_AP;

			wifiStatus = M2M_WIFI_MODE_AP;
			break;
		}

		case M2M_WIFI_MODE_AP:
		{
			if (fluffy_get_ap_mode_completed())
			{
				printf("WIFI : Complete AP Provision\r\n");

				nm_bsp_sleep(DELAY_FOR_MODE_CHANGE);

				/* AP Mode : Disable*/
				fluffy_trans_socket_close();
				fluffy_wifi_ap_mode(0, NULL);
				nm_bsp_sleep(DELAY_FOR_MODE_CHANGE);
				gParseMode = M2M_WIFI_MODE_STA_ENABLE;

				wifiStatus = M2M_WIFI_MODE_STA_ENABLE;
			}

			break;
		}

		// AP mode for provisioning
		////////////////////////////////////////////////////////
		// Station mode for connecting parse server (request & push)

		case M2M_WIFI_MODE_STA_ENABLE:
		{
/*
printf( "applicationId = %s\r\n", deviceConfigGetValue( 0, "applicationId" ) );
printf( "clientKey = %s\r\n", deviceConfigGetValue( 0, "clientKey" ) );
printf( "installationId = %s\r\n", deviceConfigGetValue( 0, "installationId" ) );
printf( "deviceId = %s\r\n", deviceConfigGetValue( 0, "sessionToken" ) );
printf( "name = %s\r\n", deviceConfigGetValue( 0, "deviceName" ) );
*/

			/* Station Mode : Enable */
			fluffy_wifi_stat_mode(1);
			gParseMode = M2M_WIFI_MODE_STA_CON;

			wifiStatus = M2M_WIFI_MODE_STA_CON;
			break;
		}

		case M2M_WIFI_MODE_STA_CON:
		{
			if (fluffy_get_wifi_connected())
			{
#ifdef ENABLE_MDNS
				if ( !mdnsd_start( deviceConfigGetValue( 0, "deviceName" ) ) )
				{
					printf("Error, mDNS Start\r\n");
				}

				tcc_enable_callback(&tempTccModule, TCC_CALLBACK_CHANNEL_3);
#endif

				gParseMode = M2M_WIFI_MODE_STA;

				wifiStatus = M2M_WIFI_MODE_STA;
			}

			break;
		}

		case M2M_WIFI_MODE_STA:
		{
			if( parseClient && parseClient->isStartPushService )
			{
				parseClient->isStartPushService = 0;
				fluffy_trans_sta_socket();
			}

			wifiStatus = M2M_WIFI_MODE_STA;
			break;
		}

		default:
		{
			printf("Unknown WIFI Mode : %d\r\n", gParseMode);
			wifiStatus = gParseMode;
			break;
		}
	}

	return wifiStatus;
}
