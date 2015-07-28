/*
 * nm_fluffy_http.c
 *
 * Created: 2014-11-14
 *  Author: jini
 */ 

#include "asf.h"
#include "nm_fluffy.h"
#include "nm_fluffy_http.h"
#include "http_entity.h"
#include "common/include/nm_common.h"
#include "nm_fluffy_trans.h"
#include "socket/include/socket.h"
#include "parse_impl.h"
#include "provisioning.h"



char responseDataBuffer[ RESP_BUFFER_MAX_SIZE ] = { 0, };

int gSentLength = 0;
char* gHttpProvisionPacket = NULL;
int gHttpProvisionPacketIndex = 0;
int gHttpProvisionPacketCompletion = false;

char* gHttpDataBuffer = NULL;
int gHttpDataBufferLength = 0;


void http_process_ap_recv_data(uint8_t socket, uint8_t* payload, uint16_t payloadSize, uint16_t remainingSize)
{	
	printf( "http_process_ap_recv_data / socket %d, payloadSize %d, remainingSize %d\r\n",
		socket, payloadSize, remainingSize );

	if( 0 == strncmp( (char*)payload, LocalControl_HttpProvision, strlen( LocalControl_HttpProvision ) ) )
	{
		printf( "http_process_ap_recv_data / Http Provisioning\r\n" );

		gSentLength = 0;

		int OriginalProvisionFileSize = 0;
		int addKeysSize = 0;

		for( int i = 0; i < fluffy_device_config_get_count(); i++ )
		{
			printf("http_process_ap_recv_data / %s, %s\r\n", fluffy_device_config_get_key( i ), fluffy_device_config_get_label( i ));
	
			char eachKeyBuffer[ 512 ] = {0,};
			sprintf( eachKeyBuffer, "<tr><td align=\"right\">%s</td><td align=\"left\"><input type=\"text\" name=\"%s\" size=\"64\"></td></tr>",
				fluffy_device_config_get_label( i ), fluffy_device_config_get_key( i ) );
			addKeysSize += strlen( eachKeyBuffer );
		}

		for(int i=0; i<HTML_DATA_LINES; i++) {
			OriginalProvisionFileSize += strlen(provisiongHtmlData[i]);
		}

		printf( "http_process_ap_recv_data / Original Provisioning file size %d, addKeysSize %d\r\n",
			OriginalProvisionFileSize, addKeysSize );

		gHttpDataBuffer = malloc( OriginalProvisionFileSize + addKeysSize + 1 );
		memset( gHttpDataBuffer, 0, OriginalProvisionFileSize + addKeysSize + 1 );
			
		for(int i=0; i<HTML_DATA_LINES; i++) {
			int tmp = strlen(gHttpDataBuffer);
			memcpy( gHttpDataBuffer + tmp, provisiongHtmlData[i], strlen(provisiongHtmlData[i]) );
		}
			
		if( addKeysSize )
		{
			char* pTemp = NULL;

			pTemp = (char*)m2m_strstr( (uint8*)gHttpDataBuffer, (uint8*)"<!-- additional keys go here -->" );
			if( pTemp )
			{
				int rearDataLength = OriginalProvisionFileSize - ( pTemp - gHttpDataBuffer );						
				char* rearData = malloc( rearDataLength + 1 );
				memcpy( rearData, pTemp, rearDataLength );
				rearData[ rearDataLength ] = 0;

				printf( "http_process_ap_recv_data / rearData size %d\r\n", rearDataLength );

				for( int i = 0; i < fluffy_device_config_get_count(); i++ )
				{				
					char eachKeyBuffer[ 512 ] = {0,};
					sprintf( eachKeyBuffer, "<tr><td align=\"right\">%s</td><td align=\"left\"><input type=\"text\" name=\"%s\" size=\"64\"></td></tr>",
						fluffy_device_config_get_label( i ), fluffy_device_config_get_key( i ) );
					memcpy( pTemp, eachKeyBuffer, strlen( eachKeyBuffer ) );
					pTemp += strlen( eachKeyBuffer );
				}

				memcpy( pTemp, rearData, rearDataLength );

				free( rearData );
				rearData = NULL;
			}
			else
			{
				printf( "http_process_ap_recv_data / <!-- additional keys go here --> not found Error !!!!!\r\n" );
			}
		}

		gHttpDataBufferLength = OriginalProvisionFileSize + addKeysSize;

		memset( responseDataBuffer, 0, RESP_BUFFER_MAX_SIZE );
		sprintf( responseDataBuffer, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: text/html\r\n\r\n",
			OriginalProvisionFileSize + addKeysSize );			
		fluffy_trans_socket_send_data_for_response(socket, responseDataBuffer);

		return;
	}
	else if( NULL == payload && 0 == payloadSize && 0 == remainingSize && gHttpDataBufferLength )
	{
		printf( "http_process_ap_recv_data / Http Provisioning entity / fileSize : %d / gSentLength : %d\r\n",
			gHttpDataBufferLength, gSentLength );

		if( gSentLength < gHttpDataBufferLength )
		{
			memset( responseDataBuffer, 0, RESP_BUFFER_MAX_SIZE );
			
			if( RESP_BUFFER_MAX_SIZE > gHttpDataBufferLength - gSentLength )
			{
				memcpy( responseDataBuffer, &gHttpDataBuffer[ gSentLength ], gHttpDataBufferLength - gSentLength );
				gSentLength += gHttpDataBufferLength - gSentLength;
			}
			else
			{
				memcpy( responseDataBuffer, &gHttpDataBuffer[ gSentLength ], RESP_BUFFER_MAX_SIZE - 1 );
				gSentLength += RESP_BUFFER_MAX_SIZE - 1;
			}
			
			fluffy_trans_socket_send_data_for_response(socket, responseDataBuffer);

			printf( "http_process_ap_recv_data / Http Provisioning entity / gSentLength : %d\r\n", gSentLength );
		}
		else
		{
			printf( "http_process_ap_recv_data / Http Provisioning entity / gSentLength : %d, gHttpDataBufferLength %d\r\n", gSentLength, gHttpDataBufferLength );

			free( gHttpDataBuffer );
			gHttpDataBuffer = NULL;
		}

		return;
	}
	else if( 0 == strncmp( (char*)payload, LocalControl_FaviconProvision, strlen( LocalControl_FaviconProvision ) ) )
	{
		memset( responseDataBuffer, 0, RESP_BUFFER_MAX_SIZE );
		sprintf( responseDataBuffer, "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: Keep-Alive\r\n\r\n" );
		fluffy_trans_socket_send_data_for_response(socket, responseDataBuffer);
		
		return;
	}


	if( 0 == strncmp( (char*)payload, LocalControl_HttpProvisionData, strlen( LocalControl_HttpProvisionData ) ) )
	{
		if( 0 < remainingSize )
		{
			gHttpProvisionPacket = malloc( payloadSize + remainingSize + 1 );
			memset( gHttpProvisionPacket, 0, payloadSize + remainingSize + 1 );
			memcpy( gHttpProvisionPacket, payload, payloadSize );
			gHttpProvisionPacketIndex = payloadSize;
			gHttpProvisionPacketCompletion = false;
			return;
		}
		else
		{
			gHttpProvisionPacket = malloc( payloadSize + 1 );
			memset( gHttpProvisionPacket, 0, payloadSize + 1 );
			memcpy( gHttpProvisionPacket, payload, payloadSize );
			gHttpProvisionPacketIndex = payloadSize;
			gHttpProvisionPacketCompletion = true;
		}
	}
	else if( gHttpProvisionPacket )
	{
		if( 0 < remainingSize )
		{
			memcpy( gHttpProvisionPacket + gHttpProvisionPacketIndex, payload, payloadSize );
			gHttpProvisionPacketIndex += payloadSize;
			gHttpProvisionPacketCompletion = false;
			return;
		}
		else
		{
			memcpy( gHttpProvisionPacket + gHttpProvisionPacketIndex, payload, payloadSize );
			gHttpProvisionPacketIndex += payloadSize;
			gHttpProvisionPacketCompletion = true;
		}
	}

	if( gHttpProvisionPacket && true == gHttpProvisionPacketCompletion )
	{
		uint8* pTemp = NULL;
		int nContentLength = 0;
		
		pTemp = m2m_strstr( (uint8*)gHttpProvisionPacket, (uint8*)"Content-Length: " );
		if( !pTemp ) 
		{
			printf( "http_process_ap_recv_data / Content-Length is 0 Error !!!!!\r\n" );
			memset( responseDataBuffer, 0, RESP_BUFFER_MAX_SIZE );
			sprintf( responseDataBuffer, "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: Keep-Alive\r\n\r\n" );
			fluffy_trans_socket_send_data_for_response(socket, responseDataBuffer);

			free( gHttpProvisionPacket );
			gHttpProvisionPacket = NULL;
			gHttpProvisionPacketIndex = 0;
			gHttpProvisionPacketCompletion = false;
			return;
		}
		
		pTemp += 16;
		nContentLength = atoi( (char*)pTemp );
		printf( "http_process_ap_recv_data / Content-Length : %d\r\n", nContentLength );


		pTemp = m2m_strstr( (uint8*)gHttpProvisionPacket, (uint8*)"\r\n\r\n" );
		if( pTemp == NULL )
		{
			gHttpProvisionPacketCompletion = false;
			return;
		}
		else
		{
			if( nContentLength != ( gHttpProvisionPacketIndex - ( pTemp + strlen( "\r\n\r\n" ) - (uint8*)gHttpProvisionPacket ) ) )
			{
				gHttpProvisionPacketCompletion = false;
				return;
			}
		}

		char SSID[ 64 ] = { 0, };
		char PASSWORD[ 64 ] = { 0, };
		char SECURITY[ 64 ] = { 0, };
		int nSECURITY = 0;

		pTemp = m2m_strstr( (uint8*)gHttpProvisionPacket, (uint8*)"SSID" );
		if( pTemp ) 
		{
			int tempIndex = 0;
			pTemp += strlen( "SSID" ) + 1;
			while( *pTemp != '&' )
			{
				SSID[ tempIndex ] = *pTemp;
				tempIndex++;
				pTemp++;
			}
		}

		pTemp = m2m_strstr( (uint8*)gHttpProvisionPacket, (uint8*)"PASSWORD" );
		if( pTemp ) 
		{
			int tempIndex = 0;
			pTemp += strlen( "PASSWORD" ) + 1;
			while( *pTemp != '&' )
			{
				PASSWORD[ tempIndex ] = *pTemp;
				tempIndex++;
				pTemp++;
			}
		}

		pTemp = m2m_strstr( (uint8*)gHttpProvisionPacket, (uint8*)"SECURITY" );
		if( pTemp ) 
		{
			int tempIndex = 0;
			pTemp += strlen( "SECURITY" ) + 1;
			while( *pTemp != '&' )
			{
				SECURITY[ tempIndex ] = *pTemp;
				tempIndex++;
				pTemp++;
			}

			//nSECURITY = atoi( SECURITY );
			if( 0 == strcmp( SECURITY, "none" ) )
				nSECURITY = 1; 	// M2M_WIFI_SEC_OPEN
			else if( 0 == strcmp( SECURITY, "wpa_personal" ) )
				nSECURITY = 2;	// M2M_WIFI_SEC_WPA_PSK
			else if( 0 == strcmp( SECURITY, "wep" ) )
				nSECURITY = 3;	// M2M_WIFI_SEC_WEP
			else if( 0 == strcmp( SECURITY, "wpa_enterprise" ) )
				nSECURITY = 4;	// M2M_WIFI_SEC_802_1X
				
		}

		// Must
		///////////////////////////////////////////////////////////////////////
		// Optional

		for( int i = 0; i < fluffy_device_config_get_count(); i++ )
		{			
			uint8* pTemp = NULL;

			pTemp = m2m_strstr( (uint8*)gHttpProvisionPacket, (uint8*)fluffy_device_config_get_key( i ) );
			if( pTemp ) 
			{
				char tempBuffer[ 128 ] = { 0, };
				int tempIndex = 0;
				pTemp += strlen( fluffy_device_config_get_key( i ) ) + 1;

				while( *pTemp != '&' )
				{
					if( pTemp >= &gHttpProvisionPacket[ gHttpProvisionPacketIndex ] )
					{
						break;
					}
					
					tempBuffer[ tempIndex ] = *pTemp;
					tempIndex++;
					pTemp++;
				}

				if( tempIndex )
				{
					fluffy_device_config_set_value( i, tempBuffer );
				}
			}
		}

		memset( responseDataBuffer, 0, RESP_BUFFER_MAX_SIZE );
		char tempBuffer[] = "HTTP/1.1 200 OK\r\nContent-Length: 29\r\nConnection: close\r\n\r\nProvision has been completed.\r\n";
		memcpy( responseDataBuffer, tempBuffer, sizeof( tempBuffer ) );
		fluffy_trans_socket_send_data_for_response(socket, responseDataBuffer);		

		printf( "http_process_ap_recv_data / %s, %s, %d\r\n", SSID, PASSWORD, nSECURITY );

		for( int i = 0; i < fluffy_device_config_get_count(); i++ )
		{
			printf( "http_process_ap_recv_data / %s, %s\r\n",
				fluffy_device_config_get_key( i ), fluffy_device_config_get_value_by_index( i ) );
		}

#if 1		
		fluffy_set_home_ap_info(SSID, PASSWORD, nSECURITY, true);
		fluffy_set_fluffy_info( true );		
#else
		fluffy_set_home_ap_info((char*)"NW11", (char*)"nmisemi2*", 2, false);
		fluffy_set_fluffy_info( (char*)"api.parse.com", 
								(char*)"G5DeUoiZMLxMPELna3FM2G9Lqa3ihzSIcLsZOuTr",
								(char*)"mXPdL4xqPuHgOEHKh22JhlqU2V2Pkzwy65Lulz9A",
								(char*)"52534c71-7977-49a0-84d3-7c9595e03ed6",
								(char*)"mkDeviceId",
								(char*)"mkName",
								false );
#endif								

		free( gHttpProvisionPacket );
		gHttpProvisionPacket = NULL;
		gHttpProvisionPacketIndex = 0;
		gHttpProvisionPacketCompletion = false;

		return;
	}
}

void http_process_sta_recv_data(uint8_t socket, uint8_t* payload, uint16_t payloadSize, uint16_t remainingSize)
{
	printf( "http_process_sta_recv_data / Error !!!!!\r\n" );

	memset( responseDataBuffer, 0, RESP_BUFFER_MAX_SIZE );
	sprintf( responseDataBuffer, "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: Keep-Alive\r\n\r\n" );
}

