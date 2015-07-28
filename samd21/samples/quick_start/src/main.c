/**
 *
 * \file
 *
 * \brief WINC1500 Chip Info Example.
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

/** \mainpage
 * \section intro Introduction
 * This example demonstrates the use of the WINC1500 with the SAMD21 Xplained Pro
 * board to retrieve the chip information of the Wi-Fi module.<br>
 * It uses the following hardware:
 * - the SAMD21 Xplained Pro.
 * - the WINC1500 on EXT1.
 *
 * \section files Main Files
 * - main.c : Initialize the WINC1500 and retrieve information.
 *
 * \section usage Usage
 * -# Build the program and download it into the board.
 * -# On the computer, open and configure a terminal application as the follows.
 * \code
 *    Baud Rate : 115200
 *    Data : 8bit
 *    Parity bit : none
 *    Stop bit : 1bit
 *    Flow control : none
 * \endcode
 * -# Start the application.
 * -# In the terminal window, the following text should appear:
 * \code
 *    -- WINC1500 chip information example --
 *    -- SAMD21_XPLAINED_PRO --
 *    -- Compiled: xxx xx xxxx xx:xx:xx --
 *    Chip ID :             xxxxxx
 *    RF Revision ID :      x
 *    Done.
 * \endcode
 *
 * \section compinfo Compilation Information
 * This software was written for the GNU GCC compiler using Atmel Studio 6.2
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com">Atmel</A>.\n
 */

#include "asf.h"
#include "conf_uart_serial.h"
#include "parse.h"
#include "parse/parse_impl.h"
#include "parse/nm_fluffy.h"
#include "common/include/nm_common.h"
#include "conf_nvm.h"


#define PUSH_TEST    // define for push test
#define REQUEST_TEST // define for request test

#define YOUR_SSID                    "TEST_AP"          // should be modified with your AP information
#define YOUR_PW                      "TEST_PASSWORD"    // should be modified with your AP information
#define YOUR_SECTYPE                 2     // should be modified with your AP information and refer 'tenuM2mSecType' in 'm2m_types.h'
#define YOUR_PARSE_APP_ID            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"    // should be modified with PARSE APP ID
#define YOUR_PARSE_CLIENT_KEY        "YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY"    // should be modified with PARSE CLIENT KEY
#define YOUR_PARSE_INSTALLATION_ID   "ZZZZZZZZ-ZZZZ-ZZZZ-ZZZZ-ZZZZZZZZZZZZ"        // should be modified with PARSE INSTALLATION ID

#define STRING_EOL    "\r\n"
#define STRING_HEADER "-- Atmel WINC1500 : Fluffy --"STRING_EOL \
"-- "BOARD_NAME " --"STRING_EOL	\
"-- Compiled: "__DATE__ " "__TIME__ " --"STRING_EOL

/* Parse instance */
ParseClient parseClient = NULL;

/** UART module for debug. */
static struct usart_module cdc_uart_module;


static void requestCallback(ParseClient client, int error, int httpStatus, const char* httpResponseBody)
{
	if(0 != error) {
		printf("requestCallback / error code %d / Error !!!!!\r\n", error);
		return;
	}

	printf("requestCallback : 0x%x, %d, %d, %s\r\n\r\n", client, error, httpStatus, httpResponseBody);
}

void pushCallback(ParseClient client, int error, const char* data)
{
	if(0 != error) {
		printf("pushCallback / error code %d / Error !!!!!\r\n", error);
		return;
	}

	printf("pushCallback : 0x%x, %s\r\n", client, data);
}

/**
 * \brief Configure UART console.
 */
static void configure_console(void)
{
	struct usart_config usart_conf;

	usart_get_config_defaults(&usart_conf);
	usart_conf.mux_setting = CONF_STDIO_MUX_SETTING;
	usart_conf.pinmux_pad0 = CONF_STDIO_PINMUX_PAD0;
	usart_conf.pinmux_pad1 = CONF_STDIO_PINMUX_PAD1;
	usart_conf.pinmux_pad2 = CONF_STDIO_PINMUX_PAD2;
	usart_conf.pinmux_pad3 = CONF_STDIO_PINMUX_PAD3;
	usart_conf.baudrate    = CONF_STDIO_BAUDRATE;

	stdio_serial_init(&cdc_uart_module, CONF_STDIO_USART_MODULE, &usart_conf);
	usart_enable(&cdc_uart_module);
}


/**
 * \brief Main application function.
 *
 * Application entry point.
 * Initialize board and WINC1500 Wi-Fi module.
 * Display WINC1500 chip id and rf revision id.
 *
 * \return program return value.
 */
int main(void)
{
	char ssid[] = YOUR_SSID;
	char pw[] = YOUR_PW;
	int secType = YOUR_SECTYPE;

	/* Initialize the board. */
	system_init();

	/* Initialize the UART console. */
	configure_console();
	printf(STRING_HEADER);

	/* Configure Non-Volatile Memory */
	configure_nvm();

	if( LOCAL_PARSE_SUCCESS != initWifiModule() )
	{
		printf("main / initWifiModule Error !!!!!\r\n");
		return -1;
	}

	loadSettingsOrProvisionBoard(ssid, pw, secType);
	while(1)
	{
		if( M2M_WIFI_MODE_STA == localParseEventLoop(NULL) )
		{
			printf("main / WiFi connection is done\r\n");
			break;
		}
	}

	parseClient = parseInitialize(YOUR_PARSE_APP_ID, YOUR_PARSE_CLIENT_KEY);
	if (parseClient)
	{
		printf("main / parseClient 0x%x\r\n", parseClient );

		parseSetInstallationId(parseClient, YOUR_PARSE_INSTALLATION_ID);

#ifdef PUSH_TEST
		parseSetPushCallback(parseClient, pushCallback);
		parseStartPushService(parseClient);
#endif

#ifdef REQUEST_TEST
		parseSendRequest(parseClient,
						"POST",
						"/1/classes/Event",
						"{\"value\":{\"temperature\":77}}",
						requestCallback );
#endif

		while(1)
		{
			parseRunPushLoop(parseClient);
		}
	}

    return 0;
}
