/**
 *
 * \file
 *
 * \brief BSD compatible socket interface.
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

#ifndef __SOCKET_H__
#define __SOCKET_H__

#ifdef  __cplusplus
extern "C" {
#endif

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
INCLUDES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

#include "common/include/nm_common.h"


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
MACROS
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/


#define HOSTNAME_MAX_SIZE									64
/*!< 
	Maximum allowed size for a host domain name passed
	to the function [gethostbyname](@ref gethostbyname).
*/


	
#define SOCKET_BUFFER_MAX_LENGTH							1400
/*!< 
	Maximum allowed size for a socket Data buffer.
*/


#define  AF_INET											2
/*!< 
	IPv4 socket family. It is the only supported by the
	current implementation.
*/


#define  SOCK_STREAM										1
/*!< 
	TCP socket type.
*/


#define  SOCK_DGRAM											2
/*!<
	UDP socket type.
*/


#define SOCKET_FLAGS_SSL									0x01
/*!< 
	This flag shall be passed to the socket API for 
	SSL session.
*/


#define TCP_SOCK_MAX										(7)
/*!< 
	Maximum number of simultaneous TCP sockets.
*/


#define UDP_SOCK_MAX										4
/*!<
	Maximum number of simultaneous UDP sockets.
*/


#define MAX_SOCKET											(TCP_SOCK_MAX + UDP_SOCK_MAX)
/*!< 
	Maximum number of Sockets.
*/

#define SOL_SOCKET											1


#define	SO_SET_UDP_SEND_CALLBACK							0x00
/*!<
	Socket option used by the application to enable/disable
	the use of UDP send callbacks. 
*/

#define IP_ADD_MEMBERSHIP									0x01
/*!< 
	Set Socket Option Add Membership command value.
*/

	
#define IP_DROP_MEMBERSHIP									0x02
/*!< 
	Set Socket Option Drop Membership command value.
*/ 

		
/**************
Socket Errors
**************/

#define SOCK_ERR_NO_ERROR									0
/*!< 
	Every thing is OK.
*/


#define SOCK_ERR_INVALID_ADDRESS							-1
/*!<
	Socket address is invalid. The socket operation cannot
	be completed without address is specified. For example,
	Bind is called without specifying a port number.
*/


#define SOCK_ERR_ADDR_ALREADY_IN_USE						-2
/*!< 
	Cannot bind on the given address. It is already bound
	by another opened socket.
*/


#define SOCK_ERR_MAX_TCP_SOCK								-3
/*!< 
	The maximum number of TCP sockets is reached. Socket
	creation failed.
*/


#define SOCK_ERR_MAX_UDP_SOCK								-4
/*!< 
	The maximum number of UDP sockets is reached. Socket
	creation failed.
*/


#define SOCK_ERR_INVALID_ARG								-6
/*!< 
	An invalid arguement is passed to a function.
*/


#define SOCK_ERR_MAX_LISTEN_SOCK							-7
/*!< 
	The maximum number of TCP passive listening sockets is
	reached. Listen function fails.
*/


#define SOCK_ERR_INVALID									-9
/*!< 
	The requested socket operation is not valid in the 
	current socket state. For Example, accept is called on a
	TCP socket before bind or listen.
*/


#define SOCK_ERR_ADDR_IS_REQUIRED							-11
/*!< 
	The socket address is required for the operation to 
	be completed. It is generated from sendto when there is
	no valid address found to send the data to.
*/


#define SOCK_ERR_CONN_ABORTED								-12
/*!< 
	The socket is closed by the peer. The local socket is 
	closed also.
*/


#define SOCK_ERR_TIMEOUT									-13
/*!< 
	The socket pending operation has been timedout. 
*/


#define SOCK_ERR_BUFFER_FULL								-14
/*!< 
	The send operation could not be performed before the 
	transmission buffer corresponding to this socket is busy.
*/

#ifdef _NM_BSP_BIG_END

#define _htonl(m)				(m)	
#define _htons(A)				(A)

#else

#define _htonl(m)		\
	(uint32)(((uint32)(m << 24)) | ((uint32)((m & 0x0000FF00) << 8)) | ((uint32)((m & 0x00FF0000) >> 8)) | ((uint32)(m >> 24)))
/*!<
	Convert a 4-byte integer from the host representation to the Network byte order representation.
*/


#define _htons(A)   	(uint16)((((uint16) (A)) << 8) | (((uint16) (A)) >> 8))
/*!<
	Convert a 2-byte integer (short) from the host representation to the Network byte order representation.
*/


#endif


#define _ntohl      		_htonl
/*!<
	Convert a 4-byte integer from the Network byte order representation to the host representation .
*/


#define _ntohs      		_htons
/*!< 
	Convert a 2-byte integer from the Network byte order representation to the host representation .
*/


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
DATA TYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*!
@typedef	\	
	SOCKET
	
@brief
	Definition for socket handler data type.
*/
typedef sint8  SOCKET;


/*!
@struct	\
	in_addr
	
@brief
	IPv4 address representation.
*/
typedef struct{
	uint32		s_addr;   
	/*!< 
		Network Byte Order representation of the IPv4 address. For example,
		the address "192.168.0.10" is represented as 0x0A00A8C0.
	*/
}in_addr;


/*!
@struct	\
	sockaddr
	
@brief
	Generic socket address structure.
*/
struct sockaddr{
    uint16		sa_family;
    uint8		sa_data[14];
};


/*!
@struct	\
	sockaddr_in
	
@brief
	Socket address structure for IPV4 addresses.
*/
struct sockaddr_in{
	uint16			sin_family;            
	/*!< 
		The only supported value for this is AF_INET.
	*/
	uint16   		sin_port;              
	/*!< 
		Port number of the socket address. It must be set in the 
		Network Byte Order format (e.g. _htons(80)).
	*/
	in_addr			sin_addr;       
	/*!< 
		IP Address [in_addr].
	*/
	uint8			sin_zero[8];          
	/*!< 
		Dummy bytes.
	*/
};


/*******************************************
Specific Definitions for Asynchronous implementation
*******************************************/

/*!
@enum	\
	tenuSocketCallbackMsgType
	
@brief
	Socket event message types for socket callback notifications.
*/
typedef enum{
	SOCKET_MSG_BIND	= 1,
	/*!<
		Bind socket event.
	*/
	SOCKET_MSG_LISTEN,
	/*!<
		Listen socket event.
	*/
	SOCKET_MSG_DNS_RESOLVE,
	/*!<
		DNS Resolution event.
	*/
	SOCKET_MSG_ACCEPT,
	/*!<
		Accept socket event.
	*/
	SOCKET_MSG_CONNECT,
	/*!<
		Connect socket event.
	*/
	SOCKET_MSG_RECV,
	/*!<
		Receive socket event.
	*/
	SOCKET_MSG_SEND,
	/*!<
		Send socket event.
	*/
	SOCKET_MSG_SENDTO,
	/*!<
		sendto socket event.
	*/
	SOCKET_MSG_RECVFROM
	/*!<
		Recvfrom socket event.
	*/
}tenuSocketCallbackMsgType;


/*!
@struct	\
	tstrSocketBindMsg
	
@brief	Socket bind status. 

	It is passed to the application socket callback with SOCKET_MSG_BIND event 
	in a response to a user call to bind.
*/
typedef struct{
	sint8		status;
	/*!< 
		The result of the bind operation. It shall be ZERO for successful bind and a negative 
		number corresponding to the error type otherwise.
	*/
}tstrSocketBindMsg;


/*!
@struct	\
	tstrSocketListenMsg
	
@brief	Socket listen status. 

	It is passed to the application socket callback with SOCKET_MSG_LISTEN event 
	in a response to a user call to listen.
*/
typedef struct{
	sint8		status;
	/*!< 
		The result of the listen operation. It shall be ZERO for successful listen and a negative 
		number corresponding to the error type otherwise.
	*/
}tstrSocketListenMsg;



/*!
@struct	\
	tstrSocketAcceptMsg
	
@brief	Socket accept status. 

	It is passed to the application socket callback with SOCKET_MSG_ACCEPT event 
	in a response to a user call to accept.
*/
typedef struct{
	SOCKET		sock;
	/*!< 
		Socket ID for the accepted connection with a remote peer. If it is a negative value, it refers to 
		an accept error (accept failed).
	*/
	struct		sockaddr_in	strAddr;
	/*!< 
		Socket address structure for the remote peer.
	*/
}tstrSocketAcceptMsg;


/*!
@struct	\
	tstrSocketConnectMsg
	
@brief	Socket connect status. 

	It is passed to the application socket callback with SOCKET_MSG_CONNECT event 
	in a response to a user call to connect.
*/
typedef struct{
	SOCKET	sock;
	/*!< 
		Socket ID referring to the socket passed to the connect function call.
	*/
	sint8		s8Error;
	/*!< 
		Connect error code. It shall be ZERO for successful connect and a negative number otherwise.
	*/
}tstrSocketConnectMsg;


/*!
@struct	\
	tstrSocketRecvMsg
	
@brief	Socket recv status. 

	It is passed to the application socket callback with SOCKET_MSG_RECV or SOCKET_MSG_RECVFROM events 
	in a response to a user call to the recv or recvfrom.
	If the received data from the remote peer is larger than the USER Buffer size (given at recv call), the data is 
	delivered to the user in a number of consecutive chunks according to the USER Buffer size.
*/
typedef struct{
	uint8					*pu8Buffer;
	/*!< 
		Pointer to the USER buffer (passed to recv or recvfrom) containing a received data chunk.
	*/
	sint16					s16BufferSize;
	/*!< 
		The recevied data chunk size. It will be negative value if there is a recv error & equals 0
		If received close socket message.
	*/
	uint16					u16RemainingSize;
	/*!< 
		The number of bytes remaining in the current recv operation.
	*/
	struct sockaddr_in		strRemoteAddr;
	/*!< 
		Socket address structure for the remote peer. It is valid for SOCKET_MSG_RECVFROM event.
	*/
}tstrSocketRecvMsg;


/*!
@typedef \
	tpfAppSocketCb

@brief
				Socket application callback function.

@param [in] sock
				Socket ID for the callback.
				
@param [in] u8Msg
				 Socket event type. Possible values are:
				  - SOCKET_MSG_BIND
				  - SOCKET_MSG_LISTEN
				  - SOCKET_MSG_ACCEPT
				  - SOCKET_MSG_CONNECT
				  - SOCKET_MSG_RECV
				  - SOCKET_MSG_SEND
				  - SOCKET_MSG_SENDTO
				  - SOCKET_MSG_RECVFROM
				
@param [in] pvMsg
				Pointer to message structure. Existing types are:
				  - tstrSocketBindMsg
				  - tstrSocketListenMsg
				  - tstrSocketAcceptMsg
				  - tstrSocketConnectMsg
				  - tstrSocketRecvMsg

@sa
	tenuSocketCallbackMsgType tstrSocketRecvMsg tstrSocketConnectMsg tstrSocketAcceptMsg tstrSocketListenMsg tstrSocketBindMsg 
*/
typedef void (*tpfAppSocketCb) (SOCKET sock, uint8 u8Msg, void * pvMsg);


/*!
@typedef	\
	tpfAppResolveCb

@brief
	Callback for the gethostbyname function (DNS Resolution callback).

@param [in] pu8DomainName
				Domain name of the host.
				
@param [in]	u32ServerIP
				Server IPv4 address encoded in NW byte order format. If it is Zero, then the DNS resolution failed. 
*/
typedef void (*tpfAppResolveCb) (uint8* pu8DomainName, uint32 u32ServerIP);

/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
FUNCTION PROTOTYPES
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*!
@fn	\			
	NMI_API void socketInit(void);

@brief	Socket Layer Initialization

	The function performs the necessary initializations for the socket library. 
	It must be invoked before any socket operation is performed.
*/
NMI_API void socketInit(void);


/*!
@fn	\			
	NMI_API void registerSocketCallback(tpfAppSocketCb socket_cb, tpfAppResolveCb resolve_cb);

@brief	Socket Layer Callback registration.

	The function performs the necessary app callback registration for sockets and DNS.
*/
NMI_API void registerSocketCallback(tpfAppSocketCb socket_cb, tpfAppResolveCb resolve_cb);


/*!
@fn	\			
	NMI_API SOCKET socket(uint16 u16Domain, uint8 u8Type, uint8 u8Flags);

@brief	
	Creates a socket with a given type.

@param [in]	u16Domain
				Socket family. The only allowed value is AF_INET (IPv4.0) for TCP/UDP sockets.

@param [in] u8Type
				Socket type. Allowed values are:
				- [SOCK_STREAM](@ref SOCK_STREAM)
				- [SOCK_DGRAM](@ref SOCK_DGRAM)

@param [in] u8Flags
				Used to specify the socket creation flags. It shall be set to zero for normal TCP/UDP sockets.
				If could be SOCKET_FLAGS_SSL if the socket is used for SSL session. The use of the flag
				[SOCKET_FLAGS_SSL](@ref SOCKET_FLAGS_SSL) has no meaning in case of UDP sockets.

@return		
	The function shall return a negative value for socket creation failed and a nonnegative value
	representing the socket ID otherwise.
*/
NMI_API SOCKET socket(uint16 u16Domain, uint8 u8Type, uint8 u8Flags);


/*!
@fn	\			
	NMI_API sint8 bind(SOCKET sock, struct sockaddr *pstrAddr, uint8 u8AddrLen);

@brief	
	Binds a socket on a local port.

@param [in]	sock
				Socket ID.

@param [in] pstrAddr
				Socket address for the address to be bound.

@param [in] u8AddrLen
				Size of the given address in bytes.

@return		
	The function shall return ZERO for successful operation and a negative value otherwise. 
*/
NMI_API sint8 bind(SOCKET sock, struct sockaddr *pstrAddr, uint8 u8AddrLen);


/*!
@fn	\			
	NMI_API sint8 listen(SOCKET sock, uint8 backlog);

@brief	
	Start listening on a passive socket for incoming connections. The socket must be bound on a local port
	or the listen fails. The listen function must be called at receiving [SOCKET_MSG_BIND](@ref SOCKET_MSG_BIND)
	in the socket callback.
	A successful listen means the TCP server operation is active. There is no need to call accept after calling
	listen. If a connection is accepted, then the application socket callback functions is notified with the new 
	connected socket through the event SOCKET_MSG_ACCEPT. 

@param [in]	sock
				Socket ID.

@param [in] backlog
				It is not used by the current implementation.

@return		
	The function shall return ZERO for successful operation and a negative value otherwise. 
*/
NMI_API sint8 listen(SOCKET sock, uint8 backlog);


/*!
@fn	\			
	NMI_API sint8 accept(SOCKET sock, struct sockaddr *addr, uint8 *addrlen);

@brief	
	The function is left in order not to break legacy implementations using it. It has no effect
	and could be safely removed from any application using it.

@param [in]	sock
				Socket ID.

@param [in] addr
				It is not used in the current implementation.

@param [in] addrlen
				It is not used in the current implementation.

@return		
	The function shall return ZERO for successful operation and a negative value otherwise. 
*/
NMI_API sint8 accept(SOCKET sock, struct sockaddr *addr, uint8 *addrlen);


/*!
@fn	\	
	NMI_API sint8 connect(SOCKET sock, struct sockaddr *pstrAddr, uint8 u8AddrLen);

@brief	
	Establishes a TCP connection with a remote server.

@param [in]	sock
				Socket ID.

@param [in]	pstrAddr
				Address of the remote server.

@param [in]	u8AddrLen
				Address length in  bytes.

@return		
	The function shall return ZERO for successful operation and a negative value otherwise. 
*/
NMI_API sint8 connect(SOCKET sock, struct sockaddr *pstrAddr, uint8 u8AddrLen);


/*!
@fn	\		
	NMI_API sint16 recv(SOCKET sock, void *pvRecvBuf, uint16 u16BufLen, uint32 u32Timeoutmsec);

@brief		
	Recieves data from a TCP Scoket.

@param [in]	sock
				Socket handler.

@param [in]	pvRecvBuf
				Pointer to a buffer that will hold the received data. The buffer shall be used 
				in the recv callback to deliver the received data to the caller. The buffer must
				be resident in memory (heap or global buffer). 

@param [in]	u16BufLen
				The buffer size in bytes.

@param [in]	u32Timeoutmsec
				Timeout for the recv function in milli-seconds. If the value is set to ZERO, the timeout
				will be set to infinite (the recv function waits forever). If the timeout period is
				elapsed with no data received, the socket will get a timeout error.

@return		
	- [SOCK_ERR_NO_ERROR](@ref SOCK_ERR_NO_ERROR)
	- [SOCK_ERR_INVALID_ARG](@ref SOCK_ERR_INVALID_ARG)
*/
NMI_API sint16 recv(SOCKET sock, void *pvRecvBuf, uint16 u16BufLen, uint32 u32Timeoutmsec);


/*!
@fn	\	
	NMI_API sint16 recvfrom(SOCKET sock, void *pvRecvBuf, uint16 u16BufLen, uint32 u32TimeoutSeconds);

@brief		
	Recieves data from a UDP Scoket.

@param [in]	sock
				Socket handler.

@param [in]	pvRecvBuf
				Pointer to a buffer that will hold the received data. The buffer shall be used 
				in the recv callback to deliver the received data to the caller. The buffer must
				be resident in memory (heap or global buffer). 

@param [in]	u16BufLen
				The buffer size in bytes.

@param [in]	u32TimeoutSeconds
				Timeout for the recv function in milli-seconds. If the value is set to ZERO, the timeout
				will be set to infinite (the recv function waits forever).

@return		
	- [SOCK_ERR_NO_ERROR](@ref SOCK_ERR_NO_ERROR)		
	- [SOCK_ERR_INVALID_ARG](@ref SOCK_ERR_INVALID_ARG)
*/
NMI_API sint16 recvfrom(SOCKET sock, void *pvRecvBuf, uint16 u16BufLen, uint32 u32Timeoutmsec);


/*!
@fn	\		
	NMI_API sint16 send(SOCKET sock, void *pvSendBuffer, uint16 u16SendLength, uint16 u16Flags);

@brief		
	Sends data on a TCP Scoket.

@param [in]	sock
				Socket handler.

@param [in]	pvSendBuffer
				Pointer to a buffer that holding data to be transmitted. 

@param [in]	u16SendLength
				The buffer size in bytes. It must not exceed [SOCKET_BUFFER_MAX_LENGTH](@ref SOCKET_BUFFER_MAX_LENGTH).

@param [in]	u16Flags
				It is not used in the current implementation

@return		
	The function shall return ZERO for successful operation and a negative value otherwise. 
*/
NMI_API sint16 send(SOCKET sock, void *pvSendBuffer, uint16 u16SendLength, uint16 u16Flags);


/*!
@fn	\		
	NMI_API sint16 sendto(SOCKET sock, void *pvSendBuffer, uint16 u16SendLength, uint16 flags, struct sockaddr *pstrDestAddr, uint8 u8AddrLen);

@brief		
	Sends data on a UDP Scoket.

@param [in]	sock
				Socket handler.

@param [in]	pvSendBuffer
				Pointer to a buffer that holding data to be transmitted. 

@param [in]	u16SendLength
				The buffer size in bytes. It must not exceed [SOCKET_BUFFER_MAX_LENGTH](@ref SOCKET_BUFFER_MAX_LENGTH).

@param [in]	flags
				It is not used in the current implementation

@param [in]	pstrDestAddr
				The destination address.

@param [in]	u8AddrLen
				Destination address length in bytes.

@return		
	The function shall return ZERO for successful operation and a negative value otherwise. 
*/
NMI_API sint16 sendto(SOCKET sock, void *pvSendBuffer, uint16 u16SendLength, uint16 flags, struct sockaddr *pstrDestAddr, uint8 u8AddrLen);


/*!
@fn	\		
	NMI_API sint8 close(SOCKET sock);

@brief		
	Closes a socket.

@param [in]	sock
				Socket handler.

@return		
	The function shall return ZERO for successful operation and a negative value otherwise. 
*/
NMI_API sint8 close(SOCKET sock);


/*!
@fn	\		
	NMI_API uint32 nmi_inet_addr(char *pcIpAddr);

@brief		
	Converts a string containing an IPv4 dotted-decimal address into a proper address for the IN_ADDR structure.

@param [in]	pcIpAddr
			A null terminated string containing the IP address in IPv4 dotted-decimal address.

@return		
	The IP address in IN_ADDR structure.
*/
NMI_API uint32 nmi_inet_addr(char *pcIpAddr);


/*!
@fn	\		
	NMI_API sint8 gethostbyname(uint8 * pcHostName);

@brief		
	Use DNS to resolve a domain name into the corresponding IP Address.

@param [in]	pcHostName
				NULL terminated string containing the domain name for the remote host.
				Its size must not exceed [HOSTNAME_MAX_SIZE](@ref HOSTNAME_MAX_SIZE).

@return		
	- [SOCK_ERR_NO_ERROR](@ref SOCK_ERR_NO_ERROR)
	- [SOCK_ERR_INVALID_ARG](@ref SOCK_ERR_INVALID_ARG)
*/
NMI_API sint8 gethostbyname(uint8 * pcHostName);


/*!
@fn	\		
	NMI_API sint8 setsockopt(SOCKET socket, sint32 s32Level, sint32 option_name,
      	 const void *option_value, uint16 u16OptionLen)

@brief		
	The setsockopt() function shall set the option specified by the option_name
	argument, at the protocol level specified by the level argument, to the value
	pointed to by the option_value argument for the socket associated with the file
	descriptor specified by the socket argument.

@param [in]	sock
				Socket handler.

@param [in]	level
				protocol level. always SOL_SOCKET for now.

@param [in]	option_name
				option to be set.

@param [in]	option_value
				pointer to user provided value.

@param [in]	option_len
				 length of the option value.

@return		
	- [SOCK_ERR_NO_ERROR](@ref SOCK_ERR_NO_ERROR)
	- [SOCK_ERR_INVALID_ARG](@ref SOCK_ERR_INVALID_ARG)
*/

NMI_API sint8 setsockopt(SOCKET socket, uint8 u8Level, uint8 option_name,
       const void *option_value, uint16 u16OptionLen);


/*!
@fn	\			
	sint8 getsockopt(SOCKET sock, uint8 u8Level, uint8 u8OptName, const void *pvOptValue, uint8 * pu8OptLen);

@brief	
	Get socket options.

@param [in]	sock
				Socket ID.
@param [in] u8Level
				The protocol level of the option.
@param [in] u8OptName
				The u8OptName argument specifies a single option to get.
@param [out] pvOptValue
				The pvOptValue argument contains pointer to a buffer containing the option value. 
@param [out] pu8OptLen
				Option value buffer length.
@return		
	The function shall return ZERO for successful operation and a negative value otherwise. 
*/
NMI_API sint8 getsockopt(SOCKET sock, uint8 u8Level, uint8 u8OptName, const void *pvOptValue, uint8* pu8OptLen);

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* __SOCKET_H__ */
