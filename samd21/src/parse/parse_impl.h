/*
 * parse.h
 *
 * Created: 2014-11-26
 *  Author: Atmel
 */


#ifndef PARSE_IMPL_H_
#define PARSE_IMPL_H_


#include "parse.h"


#ifdef __cplusplus
extern "C"
{
#endif


typedef enum {
	LOCAL_PARSE_ERROR_UNKNOWN = -1,
	LOCAL_PARSE_PROVISIONING = 1,
	LOCAL_PARSE_SUCCESS = 0,
} LocalParseStatus;

typedef enum {
	M2M_WIFI_MODE_UNKNOWN,
	M2M_WIFI_MODE_AP_ENABLE,
	M2M_WIFI_MODE_AP_PROV,
	M2M_WIFI_MODE_AP,
	M2M_WIFI_MODE_STA_ENABLE,
	M2M_WIFI_MODE_STA_CON,
	M2M_WIFI_MODE_STA
}tenuWifiMode;

typedef struct _ParseClientInternal {
    char applicationId[APPLICATION_ID_MAX_LEN+1];
    char clientKey[CLIENT_KEY_MAX_LEN+1];			// is equal to 'REST API Key'
    char installationId[INSTALLATION_ID_MAX_LEN+1];
    char installationObjectId[INSTALLATION_ID_MAX_LEN+1];
    char sessionToken[SESSION_TOKEN_MAX_LEN+1];	// What is it
    parsePushCallback parsePushCallback;
    parseRequestCallback parseRequestCallback;

    int isStartPushService;		// do like toggle, parseStartPushService -> set 1 -> set 0 after starting push service
} ParseClientInternal;


LocalParseStatus initWifiModule(void);

/**
 * Will either load the defaults or provision board ad restart.
 */
LocalParseStatus loadSettingsOrProvisionBoard(char* ssid, char* pw, int secType);

LocalParseStatus localParseDeinitialize(void);

LocalParseStatus localParseSetRequestCallback(ParseClientInternal* parseClient, parseRequestCallback callback, void* inParam);

LocalParseStatus localParseSetPushCallback(parsePushCallback callback, void* inParam);

LocalParseStatus localParseSendRequest(ParseClientInternal* parseClient, char* httpVerb, char* httpPath, char* httpRequestBody, int addInstallation);

void deviceConfigAddKey(int handle, const char* key, const char* label, const char* value);

const char * deviceConfigGetValue(int handle, const char *key);
void deviceConfigSetValue(int handle, const char *key, const char *value);
void deviceConfigCommitValues(int handle);

void localWaitForRequestToFinish(ParseClientInternal* parseClient);
tenuWifiMode localParseEventLoop(ParseClientInternal* parseClient);

#ifdef __cplusplus
}
#endif

#endif /* PARSE_IMPL_H_ */
