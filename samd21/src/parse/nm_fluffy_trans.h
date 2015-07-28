/*
 * nm_fluffy_trans.h
 *
 * Created: 2014-11-11
 *  Author: jini
 */


#ifndef NM_FLUFFY_TRANS_H_
#define NM_FLUFFY_TRANS_H_

#include "parse_impl.h"


#define RESP_BUFFER_MAX_SIZE		512

typedef enum {
	FLUFFY_SOCK_TYPE_UNKNOWN,
	FLUFFY_SOCK_TYPE_AP_SVR,
	FLUFFY_SOCK_TYPE_AP_SVR_CLT,
	FLUFFY_SOCK_TYPE_STA_SVR,
	FLUFFY_SOCK_TYPE_STA_SVR_CLT,
	FLUFFY_SOCK_TYPE_PARSE,
	FLUFFY_SOCK_TYPE_PUSH,
	FLUFFY_SOCK_TYPE_MDNS
} eSockType;


void fluffy_trans_init_socket(void);

void fluffy_trans_ap_provision(void);

void fluffy_trans_sta_socket(void);

void fluffy_trans_socket_close(void);

int fluffy_trans_send(ParseClientInternal* parseClient, char* httpVerb, char* httpPath, char* httpRequestBody, int addInstallation);

void fluffy_trans_send_push_server(uint8_t sock);

void fluffy_trans_socket_send_data_for_response(uint8_t sock, char* data);

void fluffy_trans_set_parse_request_callback(parseRequestCallback callback, void* inParam);

void fluffy_trans_set_parse_push_callback(parsePushCallback callback, void* inParam);


#endif /* NM_FLUFFY_TRANS_H_ */
