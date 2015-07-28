/*
 * nm_fluffy_http.h
 *
 * Created: 2014-11-14
 *  Author: jini
 */ 


#ifndef NM_FLUFFY_HTTP_H_
#define NM_FLUFFY_HTTP_H_


#define LocalControl_HttpProvision				"GET / HTTP/1.1"
#define LocalControl_HttpProvisionData			"POST"
#define LocalControl_FaviconProvision				"GET /favicon.ico HTTP/1.1"


void http_process_ap_recv_data(uint8_t socket, uint8_t* payload, uint16_t payloadSize, uint16_t remainingSize);

void http_process_sta_recv_data(uint8_t socket, uint8_t* payload, uint16_t payloadSize, uint16_t remainingSize);


#endif /* NM_FLUFFY_HTTP_H_ */

