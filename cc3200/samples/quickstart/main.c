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

#include "pinmux.h"

// TODO: Add Parse header here

#if defined(ccs) || defined(gcc)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif

#define OOB_TASK_PRIORITY       1
#define OSI_STACK_SIZE          2048

void initBoard() {
#ifndef USE_TIRTOS
#if defined(ccs) || defined(gcc)
    MAP_IntVTableBaseSet((unsigned long) &g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif

    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();

    PinMuxConfig();
    GPIO_IF_LedConfigure(LED1);
    GPIO_IF_LedOff(MCU_RED_LED_GPIO);

    InitTerm();
    ClearTerm();

    UART_PRINT("QuickStart - Parse for IoT sample application\r\n");
    UART_PRINT("----------------------------------------\r\n");
    UART_PRINT("\r\n");
    UART_PRINT("[QuickStart] Board init\r\n");

    // start the spawn task
    VStartSimpleLinkSpawnTask(SPAWN_TASK_PRIORITY);

    // initialize the I2C bus
    I2C_IF_Open(I2C_MASTER_MODE_FST);

    UART_PRINT("[QuickStart] Device                    : TI SimpleLink CC3200\r\n");
#ifdef USE_TIRTOS
    UART_PRINT("[QuickStart] Operating system          : TI-RTOS\r\n");
#endif
#ifdef USE_FREERTOS
    UART_PRINT("[QuickStart] Operating system          : FreeRTOS\r\n");
#endif
#ifndef SL_PLATFORM_MULTI_THREADED
    UART_PRINT("[QuickStart] Operating system          : None\r\n");
#endif
}

#ifdef USE_FREERTOS
void vAssertCalled(const char *file, unsigned long line) {
    UART_PRINT("[FreeRTOS] Assert at %s (line %ul)\r\n", file, line);
    LOOP_FOREVER();
}

void vApplicationIdleHook(void) {
}

void vApplicationMallocFailedHook() {
    UART_PRINT("[FreeRTOS] Malloc fail\r\n");
    LOOP_FOREVER();
}

void vApplicationStackOverflowHook(OsiTaskHandle *task, signed char *taskName) {
    UART_PRINT("[FreeRTOS] Stack Overflow in task %s\r\n", taskName);
    LOOP_FOREVER();
}
#endif //USE_FREERTOS

unsigned char g_Status = 0;
unsigned char g_ConnectionSSID[SSID_LEN_MAX+1];
unsigned char g_ConnectionBSSID[BSSID_LEN_MAX+1];

void printVersionInfo() {
    // get the host driver version information
    SlVersionFull ver;
    unsigned char configOptOpt = SL_DEVICE_GENERAL_VERSION;
    unsigned char configOptLen = sizeof(SlVersionFull);
    memset(&ver, 0, configOptLen);

    short status = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &configOptOpt, &configOptLen, (unsigned char *)(&ver));
    if(status >= 0) {
        UART_PRINT("[QuickStart] Host Driver Version       : %s\r\n", SL_DRIVER_VERSION);
        UART_PRINT("[QuickStart] Rom Version               : %u\r\n", ver.RomVersion);
        UART_PRINT("[QuickStart] Chip ID                   : %u\r\n", ver.ChipFwAndPhyVersion.ChipId);
        UART_PRINT("[QuickStart] Network Processor Version : %lu.%lu.%lu.%lu\r\n",
                ver.NwpVersion[0],
                ver.NwpVersion[1],
                ver.NwpVersion[2],
                ver.NwpVersion[3]);
        UART_PRINT("[QuickStart] MAC Version               : %lu.%lu.%lu.%lu\r\n",
                ver.ChipFwAndPhyVersion.FwVersion[0],
                ver.ChipFwAndPhyVersion.FwVersion[1],
                ver.ChipFwAndPhyVersion.FwVersion[2],
                ver.ChipFwAndPhyVersion.FwVersion[3]);
        UART_PRINT("[QuickStart] PHY Version               : %lu.%lu.%lu.%lu\r\n",
                ver.ChipFwAndPhyVersion.PhyVersion[0],
                ver.ChipFwAndPhyVersion.PhyVersion[1],
                ver.ChipFwAndPhyVersion.PhyVersion[2],
                ver.ChipFwAndPhyVersion.PhyVersion[3]);
    }
}

void initNetwork(signed char *ssid, SlSecParams_t *keyParams) {
    memset(g_ConnectionSSID, 0, sizeof(g_ConnectionSSID));
    memset(g_ConnectionBSSID, 0, sizeof(g_ConnectionBSSID));

    short status = sl_Start(0, 0, 0);
    if (status >= 0) {
        printVersionInfo();

        // disable scan
        unsigned char configOpt = SL_SCAN_POLICY(0);
        sl_WlanPolicySet(SL_POLICY_SCAN, configOpt, NULL, 0);

        // set tx power to maximum
        unsigned char txPower = 0;
        sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *)&txPower);

        // set power policy to normal
        sl_WlanPolicySet(SL_POLICY_PM, SL_NORMAL_POLICY, NULL, 0);

        // remove all rx filters
        _WlanRxFilterOperationCommandBuff_t rxFilterIdMask;
        memset(rxFilterIdMask.FilterIdMask, 0xFF, 8);
        sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (unsigned char *)&rxFilterIdMask, sizeof(_WlanRxFilterOperationCommandBuff_t));

        status = sl_WlanPolicySet(SL_POLICY_CONNECTION, SL_CONNECTION_POLICY(1, 0, 0, 0, 0), NULL, 0);
    }

    if (status < 0) {
        sl_Stop(SL_STOP_TIMEOUT);
        ERR_PRINT(status);
        LOOP_FOREVER();
    }

    if (status < 0) {
        sl_Stop(SL_STOP_TIMEOUT);
        ERR_PRINT(status);
        LOOP_FOREVER();
    }

    sl_WlanDisconnect();

    status = sl_WlanSetMode(ROLE_STA);
    if (status < 0) {
        sl_Stop(SL_STOP_TIMEOUT);
        ERR_PRINT(status);
        LOOP_FOREVER();
    }

    UART_PRINT("\r\n");
    UART_PRINT("[QuickStart] Network init\r\n");

    status = sl_WlanConnect(ssid, strlen((char *)ssid), NULL, keyParams, NULL);
    if (status < 0) {
        sl_Stop(SL_STOP_TIMEOUT);
        ERR_PRINT(status);
        LOOP_FOREVER();
    }

    while (!IS_IP_ACQUIRED(g_Status)) {
#ifndef SL_PLATFORM_MULTI_THREADED
        _SlNonOsMainLoopTask();
#else
        osi_Sleep(100);
#endif
    }

    sl_NetAppStop(SL_NET_APP_HTTP_SERVER_ID);
}

void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *httpServerEvent, SlHttpServerResponse_t *httpServerResponse) {
}

void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent) {
    switch (pNetAppEvent->Event) {
        case SL_NETAPP_IPV4_IPACQUIRED_EVENT:
        {
            UART_PRINT("[QuickStart] IP address                : %d.%d.%d.%d\r\n",
                    SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip, 3),
                    SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip, 2),
                    SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip, 1),
                    SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip, 0));
            UART_PRINT("[QuickStart] Gateway                   : %d.%d.%d.%d\r\n",
                    SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway, 3),
                    SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway, 2),
                    SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway, 1),
                    SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway, 0));
            UART_PRINT("[QuickStart] DNS server                : %d.%d.%d.%d\r\n",
                    SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.dns, 3),
                    SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.dns, 2),
                    SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.dns, 1),
                    SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.dns, 0));

            SET_STATUS_BIT(g_Status, STATUS_BIT_IP_AQUIRED);
        }
            break;

        default:
            break;
    }
}

void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent) {
    switch (pWlanEvent->Event) {
        case SL_WLAN_CONNECT_EVENT:
        {
            slWlanConnectAsyncResponse_t* pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;

            memset(g_ConnectionSSID, 0, sizeof(g_ConnectionSSID));
            memset(g_ConnectionBSSID, 0, sizeof(g_ConnectionBSSID));
            memcpy(g_ConnectionSSID, pEventData->ssid_name, pEventData->ssid_len);
            memcpy(g_ConnectionBSSID, pEventData->bssid, SL_BSSID_LENGTH);
            UART_PRINT("[QuickStart] Connected to              : %s (%x:%x:%x:%x:%x:%x)\r\n",
                    g_ConnectionSSID,
                    g_ConnectionBSSID[0], g_ConnectionBSSID[1],
                    g_ConnectionBSSID[2], g_ConnectionBSSID[3],
                    g_ConnectionBSSID[4], g_ConnectionBSSID[5]);

            SET_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);
        }
            break;

        case SL_WLAN_DISCONNECT_EVENT:
        {
            CLR_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(g_Status, STATUS_BIT_IP_AQUIRED);

            slWlanConnectAsyncResponse_t* pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

            UART_PRINT("[QuickStart] Disconnected from         : %s (%x:%x:%x:%x:%x:%x)\r\n",
                    g_ConnectionSSID,
                    g_ConnectionBSSID[0], g_ConnectionBSSID[1],
                    g_ConnectionBSSID[2], g_ConnectionBSSID[3],
                    g_ConnectionBSSID[4], g_ConnectionBSSID[5]);
            if (SL_USER_INITIATED_DISCONNECTION == pEventData->reason_code) {
                UART_PRINT("        Reason                    : user request\r\n");
            } else {
                UART_PRINT("        Error                     : %d\r\n", pEventData->reason_code);
            }
            memset(g_ConnectionSSID, 0, sizeof(g_ConnectionSSID));
            memset(g_ConnectionBSSID, 0, sizeof(g_ConnectionBSSID));
        }
            break;

        default:
            break;
    }
}

void SimpleLinkSockEventHandler(SlSockEvent_t *socket) {
}

typedef struct {
    P_OSI_SPAWN_ENTRY handler;
    void *params;
} ApplicationMessage;

OsiMsgQ_t g_ApplicationMessageQueue;

void startApplication(void *pvParameters) {
    // TODO: Change to your SSID and password
    signed char ssid[] = "<wifi ssid>";
    signed char key[] = "<wifi password>";
    SlSecParams_t keyParams;
    keyParams.Type = SL_SEC_TYPE_WPA;
    keyParams.Key = key;
    keyParams.KeyLen = strlen((char *)key);

    initNetwork(ssid, &keyParams);

    // TODO: Set to current date/time (within an hour precision)
    SlDateTime_t dateTime;
    memset(&dateTime, 0, sizeof(dateTime));
    dateTime.sl_tm_year = 2015;
    dateTime.sl_tm_mon = 3;
    dateTime.sl_tm_day = 20;
    dateTime.sl_tm_hour = 12;
    sl_DevSet(SL_DEVICE_GENERAL_CONFIGURATION, SL_DEVICE_GENERAL_CONFIGURATION_DATE_TIME, sizeof(SlDateTime_t), (unsigned char *)&dateTime);

    UART_PRINT("\r\n");
    UART_PRINT("[QuickStart] Start application\r\n");
    UART_PRINT("\r\n");

    // TODO: Add Parse code here
}

int main(void) {
    initBoard();

    // create the main application message queue
    // this call properly enables the OSI scheduler to function
    short status = osi_MsgQCreate(&g_ApplicationMessageQueue, "ApplicationMessageQueue", sizeof(ApplicationMessage), 1);
    if (status < 0) {
        ERR_PRINT(status);
        LOOP_FOREVER();
    }

    // start the main application task
    // this is necessary because SimpleLink host driver is started by sl_Start(),
    // which cannot be called on before the OSI scheduler is started
    status = osi_TaskCreate(startApplication, (signed char *)"QuickStart", OSI_STACK_SIZE,  NULL, OOB_TASK_PRIORITY, NULL);
    if (status < 0) {
        ERR_PRINT(status);
        LOOP_FOREVER();
    }

    // start the OSI scheduler
    osi_start();

    return 0;
}
