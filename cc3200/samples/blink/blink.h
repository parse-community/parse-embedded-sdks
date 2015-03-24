/*
 *  Copyright (c) 2015, Parse, LLC. All rights reserved.
 *
 *  You are hereby granted a non-exclusive, worldwide, royalty-free license to use,
 *  copy, modify, and distribute this software in source code or binary form for use
 *  in connection with the web services and APIs provided by Parse.
 *
 *  As with any software that integrates with the Parse platform, your use of
 *  this software is subject to the Parse Terms of Service
 *  [https://www.parse.com/about/terms]. This copyright notice shall be
 *  included in all copies or substantial portions of the software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 *  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 *  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 *  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#pragma once

#ifndef __BLINK_H__
#define __BLINK_H__

#ifdef __cplusplus
extern "C"
{
#endif

// SimpleLink host driver
#include <simplelink.h>
#include <hw_types.h>
#include <hw_ints.h>
#include <prcm.h>
#include <rom_map.h>
#include <interrupt.h>
#include <utils.h>

// C runtime
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

// OSI model
#include <osi.h>

// CC3200 samples common headers
#include <common.h>
#include <uart_if.h>
#include <button_if.h>
#include <gpio_if.h>
#include <i2c_if.h>

// Parse for IoT SDK
#include <parse.h>

#if defined(ccs) || defined(gcc)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif

#define OOB_TASK_PRIORITY       1
#define OSI_STACK_SIZE          2048

#define DEVICE_NAME_MAX_LEN     64

typedef struct {
    P_OSI_SPAWN_ENTRY handler;
    void *params;
} ApplicationMessage;

/* blink.c */
void initParseClient();
void resetParseClient();
short startPushServiceTask();
void saveLedState();

/* board.c */
void initBoard();
void resetBoard();
void factoryReset();
void getBoardTimeDate(SlDateTime_t *dateTime);
void setBoardTimeDate(SlDateTime_t *dateTime);

/* button.c */
void enableButtons();

/* config.c */
extern char g_ApplicationID[APPLICATION_ID_MAX_LEN+1];
extern char g_ClientKey[CLIENT_KEY_MAX_LEN+1];
extern char g_InstallationID[INSTALLATION_ID_MAX_LEN+1];
extern char g_SessionToken[SESSION_TOKEN_MAX_LEN+1];
extern char g_DeviceName[DEVICE_NAME_MAX_LEN+1];

short initConfig();
short loadConfig();
short saveConfig();
void resetConfig();

/* json.c */
int simpleJson(const char *data, const char *key, char* value, int size);

/* led.c */
int setLedState(const char* state);
const char *getLedState();
void cycleLedState();

/* main.c */
extern OsiMsgQ_t g_ApplicationMessageQueue;

/* network.c */
extern unsigned char g_Status;
void initNetwork();
void startConfigurationMode();
void startApplicationMode();
void resetNetwork();

/* time.c */
short setBoardTimeFromNtp();

/* urlencode.c */
/**
 * A very lightweight urlencoding
 * @params
 *   str - [in] string to url-encode
 *   buf - [out] buffer to receive the url-encoded string
 *               the out buffer should be threee times the size of the in buffer + 1
 */
void urlEncode(const char *str, char *buf);

/**
 * A very lightweight urldecoding
 * @params
 *   str - [in] string to url-decode
 *   buf - [out] buffer to receive the url-dewcoded string
 *               the out buffer should be the size of the in buffer + 1
 */
void urlDecode(const char *str, char *buf);

#ifdef __cplusplus
}
#endif

#endif /* __BLINK_H__ */
