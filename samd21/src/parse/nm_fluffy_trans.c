/*
 * nm_fluffy_trans.c
 *
 * Created: 2014-11-11
 *  Author: jini
 */

#include "asf.h"
#include "nm_fluffy.h"
#include "nm_fluffy_trans.h"
#include "nm_fluffy_http.h"
#include "common/include/nm_common.h"
#include "driver/include/m2m_wifi.h"
#include "socket/include/socket.h"

#include <errno.h>
#include "http_entity_parse.h"
#include "http_client.h"


//#define ENABLE_SOCKET_DEBUG_MESSAGE

#define DEFAULT_DEV_SERVER_PORT         80

#define HTTP_CLIENT_PARSE_SERVER_URL		"https://api.parse.com"
#define DEFAULT_PARSE_SERVER_NAME		"api.parse.com"
#define DEFAULT_PARSE_SERVER_PORT		443 /* HTTPS port */

#define DEFAULT_PUSH_SERVER_NAME		"push.parse.com"
#define DEFAULT_PUSH_SERVER_PORT		8253

#define PARSE_PUSH_DATA		"{\"installation_id\":\"%s\", \"oauth_key\":\"%s\", \"v\": \"a1.4.1\", \"last\": null, \"ack_keep_alive\": true}\r\n"

/** Using IP address. */
#define IPV4_BYTE(val, index)           ((val >> (index * 8)) & 0xFF)

#define DEFAULT_SOCKET_MAX_SIZE			TCP_SOCK_MAX

typedef struct _tSockInfo {
	SOCKET sock;
	uint8_t sockType;
	parsePushCallback parseCb;
	void* param;
	uint32_t ip;
	uint16_t port;
} tSockInfo;

static tSockInfo gSockInfo[DEFAULT_SOCKET_MAX_SIZE];

/** Receive buffer definition. */
static uint8_t socketBuffer[RESP_BUFFER_MAX_SIZE] = {0,};

/** Instance of Timer module. */
struct sw_timer_module swt_module_inst;

/** Instance of HTTP client module of parse server. */
struct http_client_module http_client_parse;
struct http_entity entity_parse = { 0, };

static parseRequestCallback g_requestCallback = NULL;
static void* g_requestCallback_inParam = NULL;

// For PUSH, temp
extern char g_applicationId[APPLICATION_ID_MAX_LEN+1];
extern char g_installationId[INSTALLATION_ID_MAX_LEN+1];


static void close_current_socket(SOCKET curSock)
{
	uint8_t i;

	for (i = 0; i < DEFAULT_SOCKET_MAX_SIZE; i++)
	{
		if (curSock == gSockInfo[i].sock)
		{
			close(gSockInfo[i].sock);
			memset(&gSockInfo[i], 0, sizeof(tSockInfo));
			gSockInfo[i].sock = -1;

			break;
		}
	}
}

static void close_current_socket_only(SOCKET curSock)
{
	uint8_t i;

	for (i = 0; i < DEFAULT_SOCKET_MAX_SIZE; i++)
	{
		if (curSock == gSockInfo[i].sock)
		{
			close(gSockInfo[i].sock);
			//memset(&gSockInfo[i], 0, sizeof(tSockInfo));
			gSockInfo[i].sock = -1;

			break;
		}
	}
}

static uint8_t set_tcp_socket(eSockType type, uint32_t hostIp, uint16_t port, uint8_t flags)
{
	struct sockaddr_in addr;
	SOCKET sock = -1;
	uint8_t i = 0;

	for(i = 0; i < DEFAULT_SOCKET_MAX_SIZE; i++)
	{
		if (gSockInfo[i].sock < 0)
		{
			if (gSockInfo[i].sockType == FLUFFY_SOCK_TYPE_PUSH)
			{
				if (type == FLUFFY_SOCK_TYPE_PUSH)
				{
					break;
				}
			}
			else
			{
				gSockInfo[i].sockType= type;
				break;
			}
		}
	}

	if (i== DEFAULT_SOCKET_MAX_SIZE)
	{
		printf("Error, Overflow socket info\r\n");
		return 0;
	}

	/* Initialize socket address structure. */
	addr.sin_family = AF_INET;
	addr.sin_port = _htons(port);
	addr.sin_addr.s_addr = hostIp;

	/* Open TCP socket */
	if ((sock = socket(AF_INET, SOCK_STREAM, flags)) < 0)
	{
		printf("main: failed to create TCP server socket error!\r\n");
		return 0;
	}

	if (hostIp)
	{
		/* Connect service : Client */
		connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
	}
	else
	{
		/* Bind service : Server */
		bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
	}

	gSockInfo[i].sock = sock;
	gSockInfo[i].ip = hostIp;
	gSockInfo[i].port = port;

	return 1;
}

static void socket_resolve_cb(uint8_t *hostName, uint32_t hostIp)
{
	uint8_t flag = 0;
	uint16_t port = 0;
	eSockType hostSockType = 0;

	printf("Host Name is %s\r\n", hostName);
	printf("Host IP is %d.%d.%d.%d\r\n", (int)IPV4_BYTE(hostIp, 0), (int)IPV4_BYTE(hostIp, 1),
	(int)IPV4_BYTE(hostIp, 2), (int)IPV4_BYTE(hostIp, 3));

	if (!strcmp(DEFAULT_PUSH_SERVER_NAME, (char*)hostName))
	{
		hostSockType = FLUFFY_SOCK_TYPE_PUSH;
		port = DEFAULT_PUSH_SERVER_PORT;
		flag = 0;

		if (!(set_tcp_socket(hostSockType, hostIp, port, flag)))
		{
			printf("Error !!!!!, create Sta Server Socket.\n");
		}
	}
	else if( !strcmp( DEFAULT_PARSE_SERVER_NAME, (char*)hostName ) )
	{
		http_client_socket_resolve_handler(hostName, hostIp);
	}
	else
	{
		printf("Error !!!!!, socket_resolve_cb\n");
	}
}

static tSockInfo* get_sockinfo_by_socktype(eSockType type)
{
	tSockInfo* retInfo = NULL;
	uint8_t i = 0;

	for(i = 0; i < DEFAULT_SOCKET_MAX_SIZE; i++)
	{
		if (gSockInfo[i].sockType == type && gSockInfo[i].sock >= 0)
		{
			retInfo = gSockInfo + i;
			break;
		}
	}

	return retInfo;
}

static eSockType get_socktype_by_sock(SOCKET sock)
{
	eSockType retType = FLUFFY_SOCK_TYPE_UNKNOWN;
	uint8_t i = 0;

	for(i = 0; i < DEFAULT_SOCKET_MAX_SIZE; i++)
	{
		if (gSockInfo[i].sock == sock)
		{
			retType = gSockInfo[i].sockType;
			break;
		}
	}

	return retType;
}

static void set_socket_info(SOCKET sock, eSockType type, parsePushCallback cb, void* inParam)
{
	uint8_t i;

	for (i = 0; i < DEFAULT_SOCKET_MAX_SIZE; i++)
	{
		if (gSockInfo[i].sockType == FLUFFY_SOCK_TYPE_UNKNOWN)
		{
			gSockInfo[i].sock = sock;
			gSockInfo[i].sockType = type;
			gSockInfo[i].parseCb = cb;
			gSockInfo[i].param = inParam;
			break;
		}
	}

	if( i == DEFAULT_SOCKET_MAX_SIZE )
		printf( "set_socket_info / sock overflow Error !!!!!\r\n" );
}

static void socket_event_cb(SOCKET sock, uint8_t u8Msg, void *pvMsg)
{
	if( FLUFFY_SOCK_TYPE_UNKNOWN == get_socktype_by_sock( sock ) )
	{
		http_client_socket_event_handler( sock, u8Msg, pvMsg );
		return;
	}

	// http client for parse request
	///////////////////////////////////////////////////////////////
	// tcp for push and provisioning

	switch (u8Msg)
	{
		/* Socket(Server) bind */
		case SOCKET_MSG_BIND:
		{
			tstrSocketBindMsg *pstrBind = (tstrSocketBindMsg *)pvMsg;
			if (pstrBind && pstrBind->status == 0)
			{
				listen(sock, 0);
			}
			else
			{
				printf("socket_cb: bind Error !!!!!\r\n");
				close_current_socket(sock);
				//exit(1);
			}

			break;
		}

		/* Socket(Server) listen */
		case SOCKET_MSG_LISTEN:
		{
			tstrSocketListenMsg *pstrListen = (tstrSocketListenMsg *)pvMsg;
			if (pstrListen && pstrListen->status == 0)
			{
				printf("socket_cb: Ready to listen.\r\n");
				accept(sock, NULL, 0);
			}
			else
			{
				printf("socket_cb: listen Error !!!!!\r\n");
				close_current_socket(sock);
				//exit(1);
			}
			break;
		}

		/* Socket(Server) accept */
		case SOCKET_MSG_ACCEPT:
		{
			tstrSocketAcceptMsg *pstrAccept = (tstrSocketAcceptMsg *)pvMsg;
			if (pstrAccept)
			{
				accept(sock, (struct sockaddr *)&pstrAccept->strAddr, NULL);

				if (pstrAccept->sock >= 0)
				{
					eSockType type = get_socktype_by_sock(sock);

					if (type == FLUFFY_SOCK_TYPE_AP_SVR)
					{
						set_socket_info(pstrAccept->sock, FLUFFY_SOCK_TYPE_AP_SVR_CLT, NULL, NULL);
						printf("socket_cb: AP Accepted socket(%d) is created.\r\n", pstrAccept->sock);

					}

					if (type == FLUFFY_SOCK_TYPE_STA_SVR)
					{
						set_socket_info(pstrAccept->sock, FLUFFY_SOCK_TYPE_STA_SVR_CLT, NULL, NULL);
						printf("socket_cb: STA Accepted socket(%d) is created.\r\n", pstrAccept->sock);
					}

					recv(pstrAccept->sock, socketBuffer, RESP_BUFFER_MAX_SIZE, 0);
				}
			}
			else
			{
				printf("socket_cb: accept Error !!!!! / sock : %d\r\n", sock );
				close_current_socket(sock);
				//exit(1);
			}

			break;
		}

		/* Socket(Client) connected */
		case SOCKET_MSG_CONNECT:
		{
			eSockType type = get_socktype_by_sock(sock);

			tstrSocketConnectMsg *pstrConnect = (tstrSocketConnectMsg *)pvMsg;
			if (pstrConnect && pstrConnect->s8Error >= 0)
			{
				if (type == FLUFFY_SOCK_TYPE_PUSH)
				{
					printf("socket_cb: PUSH server connect success / sock : %d, type : %d\r\n", sock, type );

					fluffy_trans_send_push_server(sock);
				}

				recv(sock, socketBuffer, RESP_BUFFER_MAX_SIZE, 0);
			}
			else
			{
				if( pstrConnect )
					printf("socket_cb: connect Error !!!!!  sock close, socket, connect... (sock:%d, error:%d)\r\n", sock, pstrConnect->s8Error );
				else
					printf("socket_cb: connect Error !!!!!  sock close, socket, connect... (sock:%d, error:UNKNOWN)\r\n", sock );

				if (type == FLUFFY_SOCK_TYPE_PUSH)
				{
					close_current_socket_only(sock);
					gethostbyname((uint8*)DEFAULT_PUSH_SERVER_NAME);
				}
				return ;
			}
			break;
		}

		/* Message send */
		case SOCKET_MSG_SEND:
		{
			eSockType type = get_socktype_by_sock(sock);
			printf("socket_cb: send success / sock : %d, type : %d\r\n", sock, type );

			switch (type)
			{
				case FLUFFY_SOCK_TYPE_AP_SVR_CLT:
				{
					http_process_ap_recv_data(sock, NULL, 0, 0);
					break;
				}
			}

			break;
		}

		case SOCKET_MSG_SENDTO:
		{
			//printf("socket_cb: sendto success!\r\n");
			break;
		}

		/* Message receive */
		case SOCKET_MSG_RECV:
		{
			eSockType type = get_socktype_by_sock(sock);

			tstrSocketRecvMsg *pstrRecv = (tstrSocketRecvMsg *)pvMsg;
			if (pstrRecv && pstrRecv->s16BufferSize > 0)
			{
				printf("socket_cb: recv success / sock : %d, type : %d\r\n", sock, type );

				/*
				printf("recv data : start~~~\r\n");
				for( int i = 0; i < pstrRecv->s16BufferSize; i++ )
				{
					printf( "%c", pstrRecv->pu8Buffer[i] );
				}
				printf("\r\nrecv data : end~~~\r\n");
				*/

				switch (type)
				{
					case FLUFFY_SOCK_TYPE_AP_SVR_CLT:
					{
						http_process_ap_recv_data(sock, pstrRecv->pu8Buffer, pstrRecv->s16BufferSize, pstrRecv->u16RemainingSize);
						break;
					}

					case FLUFFY_SOCK_TYPE_STA_SVR_CLT:
					{
						http_process_sta_recv_data(sock, pstrRecv->pu8Buffer, pstrRecv->s16BufferSize, pstrRecv->u16RemainingSize);
						break;
					}

					case FLUFFY_SOCK_TYPE_PUSH:
					{
						fluffy_trans_send_push_server(sock);

						char* pushBuffer = malloc(pstrRecv->s16BufferSize + 1);
						memset(pushBuffer, 0, pstrRecv->s16BufferSize + 1);
						memcpy(pushBuffer, pstrRecv->pu8Buffer, pstrRecv->s16BufferSize);
						fluffy_trans_socket_send_data_for_response( sock, pushBuffer );
						free(pushBuffer);
						break;
					}

					default:
					{
						printf("socket_cb:SOCKET_MSG_RECV, Unknown type(%d) Error !!!!!\r\n", type);
						break;
					}
				}

				recv(sock, socketBuffer, RESP_BUFFER_MAX_SIZE, 0);
			}
			else
			{
				printf("socket_cb: negative Error !!!!! / sock : %d, type : %d, pstrRecv->s16BufferSize : %d\r\n", sock, type, pstrRecv->s16BufferSize );

				switch( pstrRecv->s16BufferSize )
				{
					case SOCK_ERR_INVALID_ADDRESS:			// -1
					case SOCK_ERR_ADDR_ALREADY_IN_USE:	// -2
					case SOCK_ERR_MAX_TCP_SOCK:			// -3
					case SOCK_ERR_MAX_UDP_SOCK:			// -4
					case SOCK_ERR_INVALID_ARG:				// -6
					case SOCK_ERR_MAX_LISTEN_SOCK:		// -7
					case SOCK_ERR_INVALID:					// -9
					case SOCK_ERR_ADDR_IS_REQUIRED:		// -11
					case SOCK_ERR_BUFFER_FULL:				// -14
					case SOCK_ERR_CONN_ABORTED:			// -12
					case SOCK_ERR_TIMEOUT:					// -13
					{
						if( type == FLUFFY_SOCK_TYPE_PUSH )
						{
							tSockInfo* pushSock = get_sockinfo_by_socktype(FLUFFY_SOCK_TYPE_PUSH);
							if (pushSock)
							{
								pushSock->parseCb( pushSock->param, pstrRecv->s16BufferSize, NULL );
							}

							printf("socket_cb: Push socket receives Error( %d ) !!!!! / sock close, reconnect...\r\n", pstrRecv->s16BufferSize);
							close_current_socket_only(sock);
							gethostbyname((uint8*)DEFAULT_PUSH_SERVER_NAME);
						}
						return ;
					}

				}
			}

			break;
		}

		case SOCKET_MSG_RECVFROM:
		{
			//printf("socket_cb: recvfrom success!\r\n");
			break;
		}

		default:
			break;
	}
}

/**
 * \brief Callback of the HTTP client.
 *
 * \param[in]  module_inst     Module instance of HTTP client module.
 * \param[in]  type            Type of event.
 * \param[in]  data            Data structure of the event. \refer http_client_data
 */
static void http_client_callback(struct http_client_module *module_inst, int type, union http_client_data *data)
{
	switch (type) {
	case HTTP_CLIENT_CALLBACK_SOCK_CONNECTED:
		printf("Connected\r\n");
		break;

	case HTTP_CLIENT_CALLBACK_REQUESTED:
		printf("Request complete\r\n");
		break;

	case HTTP_CLIENT_CALLBACK_RECV_RESPONSE:
		printf("Received response %u data size %u\r\n",
				(unsigned int)data->recv_response.response_code,
				(unsigned int)data->recv_response.content_length);

		if( g_requestCallback )
		{
			char* contentData = malloc( data->recv_response.content_length + 1 );
			memcpy( contentData, data->recv_response.content, data->recv_response.content_length );
			contentData[ data->recv_response.content_length ] = 0;

			g_requestCallback( g_requestCallback_inParam, 0, data->recv_response.response_code, contentData );

			free( contentData );
		}
		break;

	case HTTP_CLIENT_CALLBACK_DISCONNECTED:
		printf("Disconnected reason:%d\r\n", data->disconnected.reason);

		if( g_requestCallback )
		{
			g_requestCallback( g_requestCallback_inParam, data->disconnected.reason, 0, NULL );
		}

		/* If disconnect reason is equals to -ECONNRESET(-104),
		 * It means Server was disconnected your connection by the keep alive timeout.
		 * This is normal operation.
		 */
		if (data->disconnected.reason == -EAGAIN) {
			printf("**************** Please check this log **************** \r\n");
			/* Server has not responded. retry it immediately. */
			/*
			http_client_send_request(&http_client_parse,
				HTTP_CLIENT_PARSE_SERVER_URL,
				HTTP_CLIENT_PARSE_SERVER_METHOD,
				NULL,
				NULL);
			*/
		}

		break;
	}
}

/**
 * \brief Configure HTTP client module.
 */
static void configure_http_client()
{
	struct http_client_config httpc_conf;
	int ret;

	http_client_get_config_defaults(&httpc_conf);

	httpc_conf.recv_buffer_size = 1024;
	httpc_conf.timer_inst = &swt_module_inst;
	httpc_conf.tls = 1;
	httpc_conf.port = DEFAULT_PARSE_SERVER_PORT;

	ret = http_client_init(&http_client_parse, &httpc_conf);
	if (ret < 0) {
		printf("Parse HTTP client initialization has failed(%s)\r\n", strerror(ret));
		while (1) {
		} /* Loop forever. */
	}

	http_client_register_callback(&http_client_parse, http_client_callback);
}

/**
 * \brief Configure Timer module.
 */
static void configure_http_timer(void)
{
	struct sw_timer_config swt_conf;
	sw_timer_get_config_defaults(&swt_conf);

	swt_conf.tcc_dev = 1;

	sw_timer_init(&swt_module_inst, &swt_conf);
	sw_timer_enable(&swt_module_inst);
}

void fluffy_trans_init_socket(void)
{
	uint8_t i;

	for (i = 0; i < DEFAULT_SOCKET_MAX_SIZE; i++)
	{
		memset(&gSockInfo[i], 0, sizeof(tSockInfo));
		gSockInfo[i].sock = -1;
	}

	/* Initialize Socket module */
	socketInit();
	registerSocketCallback(socket_event_cb, socket_resolve_cb);

	/* Initialize the Timer. */
	configure_http_timer();

	/* Initialize the HTTP client service. */
	configure_http_client();
}

void fluffy_trans_ap_provision(void)
{
	printf("WIFI Provision Mode\r\n");
	// Implement Provision AP
	if (!get_sockinfo_by_socktype(FLUFFY_SOCK_TYPE_AP_SVR))
	{
		if (!(set_tcp_socket(FLUFFY_SOCK_TYPE_AP_SVR, 0, DEFAULT_DEV_SERVER_PORT, 0)))
		{
			printf("Error, create AP Server Socket.\r\n");
		}
	}
}

void fluffy_trans_sta_socket(void)
{
	/* Create Client Socket : Push */
	if (!get_sockinfo_by_socktype(FLUFFY_SOCK_TYPE_PUSH))
	{
		/* Obtain the IP Address by network name */
		gethostbyname((uint8_t *)DEFAULT_PUSH_SERVER_NAME);
	}
}

int fluffy_trans_send(ParseClientInternal* parseClient, char* httpVerb, char* httpPath, char* httpRequestBody, int addInstallation)
{
	parse_server_entity_get( &entity_parse, httpRequestBody );

	char ext_header_buffer[ 512 ]  = { 0, };
	sprintf( ext_header_buffer, "User-Agent:samd21\r\nX-Parse-Client-Version:1.0\r\nX-Parse-Application-Id: %s\r\nX-Parse-Client-Key: %s\r\n",
//		fluffy_device_config_get_value( "applicationId" ),
//		fluffy_device_config_get_value( "clientKey" ) );
		parseClient->applicationId,
		parseClient->clientKey);

        if (addInstallation) {
          if (parseClient->installationId[0])
            sprintf(ext_header_buffer + strlen(ext_header_buffer), "X-Parse-Installation-Id:%s\r\n",  parseClient->installationId);
          if (parseClient->sessionToken[0])
            sprintf(ext_header_buffer + strlen(ext_header_buffer), "X-Parse-Session-Token:%s\r\n", parseClient->sessionToken);
        }

	char url_buffer[ 128 ] = { 0, }; // for our purposes that is enough. Standard allows up to 2048
	sprintf( url_buffer, "%s%s", HTTP_CLIENT_PARSE_SERVER_URL, httpPath );

	int httpMethod = 0;
	if( 0 == strcmp( httpVerb, "GET" ) )
		httpMethod = HTTP_METHOD_GET;
	else if( 0 == strcmp( httpVerb, "POST" ) )
		httpMethod = HTTP_METHOD_POST;
	else if( 0 == strcmp( httpVerb, "PUT" ) )
		httpMethod = HTTP_METHOD_PUT;
	else if( 0 == strcmp( httpVerb, "DELETE" ) )
		httpMethod = HTTP_METHOD_DELETE;

	http_client_send_request( &http_client_parse,
		url_buffer,
		httpMethod,
		&entity_parse,
		ext_header_buffer );

	return 0;
}

void fluffy_trans_send_push_server(uint8_t sock)
{
	char send_buf[256] = { 0, };

	sprintf(send_buf,
		PARSE_PUSH_DATA,
//		fluffy_device_config_get_value( "installationId" ),
//		fluffy_device_config_get_value( "applicationId" ));
		g_installationId,
		g_applicationId);

	printf("send msg to Push Server\r\n%s\r\n", send_buf);
	send((SOCKET)sock, send_buf, strlen(send_buf),0);
}

void fluffy_trans_socket_close(void)
{
	uint8_t i;

	for (i = 0; i < DEFAULT_SOCKET_MAX_SIZE; i++)
	{
		if (gSockInfo[i].sock >= 0)
		{
			close(gSockInfo[i].sock);
			memset(&gSockInfo[i], 0, sizeof(tSockInfo));
			gSockInfo[i].sock = -1;
		}
	}
}

void fluffy_trans_socket_send_data_for_response(uint8_t sock, char* data)
{
	SOCKET socket = (SOCKET)sock;
	eSockType type = get_socktype_by_sock(sock);

#ifdef ENABLE_SOCKET_DEBUG_MESSAGE
	printf("fluffy_trans_socket_send_data_for_response / send data : start~~~\r\n");
	for( int i = 0; i < strlen( data ); i++ )
		printf( "%c", data[i] );
	printf("\r\nfluffy_trans_socket_send_data_for_response / send data : end~~~\r\n");
#endif

	switch (type)
	{
		case FLUFFY_SOCK_TYPE_AP_SVR_CLT:
		case FLUFFY_SOCK_TYPE_STA_SVR_CLT:
		{
			send(socket, data, strlen( data ), 0);
			break;
		}

		case FLUFFY_SOCK_TYPE_PUSH:
		{
			tSockInfo* pushSock = get_sockinfo_by_socktype(FLUFFY_SOCK_TYPE_PUSH);
			if (pushSock)
			{
				pushSock->parseCb( pushSock->param, 0, (char*)data );
			}
			break;
		}

		default:
		{
			printf("send_data_for_response: Unknown type(%d)\r\n", type);
			break;
		}
	}
}

void fluffy_trans_set_parse_request_callback(parseRequestCallback callback, void* inParam)
{
	g_requestCallback = callback;
	g_requestCallback_inParam = inParam;
}

void fluffy_trans_set_parse_push_callback(parsePushCallback callback, void* inParam)
{
	set_socket_info(-1, FLUFFY_SOCK_TYPE_PUSH, callback, inParam);
}
