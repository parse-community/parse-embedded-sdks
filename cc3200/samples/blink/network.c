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

#include "blink.h"

unsigned char g_Status = 0;
unsigned char g_ConnectionSSID[SSID_LEN_MAX+1];
unsigned char g_ConnectionBSSID[BSSID_LEN_MAX+1];
unsigned char g_ProvisioningDone = 0;
signed char g_WlanSSID[MAXIMAL_SSID_LENGTH+1];
signed char g_WlanSecurityKey[50];
SlSecParams_t g_SecParams;

#define CONNECTION_TIMEOUT_COUNT 5000

void printVersionInfo() {
    // get the host driver version information
    SlVersionFull ver;
    unsigned char configOptOpt = SL_DEVICE_GENERAL_VERSION;
    unsigned char configOptLen = sizeof(SlVersionFull);
    memset(&ver, 0, configOptLen);

    short status = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &configOptOpt, &configOptLen, (unsigned char *)(&ver));
    if (status >= 0) {
        UART_PRINT("[Blink] Host Driver Version       : %s\r\n", SL_DRIVER_VERSION);
        UART_PRINT("[Blink] Rom Version               : %u\r\n", ver.RomVersion);
        UART_PRINT("[Blink] Chip ID                   : %u\r\n", ver.ChipFwAndPhyVersion.ChipId);
        UART_PRINT("[Blink] Network Processor Version : %lu.%lu.%lu.%lu\r\n",
                ver.NwpVersion[0],
                ver.NwpVersion[1],
                ver.NwpVersion[2],
                ver.NwpVersion[3]);
        UART_PRINT("[Blink] MAC Version               : %lu.%lu.%lu.%lu\r\n",
                ver.ChipFwAndPhyVersion.FwVersion[0],
                ver.ChipFwAndPhyVersion.FwVersion[1],
                ver.ChipFwAndPhyVersion.FwVersion[2],
                ver.ChipFwAndPhyVersion.FwVersion[3]);
        UART_PRINT("[Blink] PHY Version               : %lu.%lu.%lu.%lu\r\n",
                ver.ChipFwAndPhyVersion.PhyVersion[0],
                ver.ChipFwAndPhyVersion.PhyVersion[1],
                ver.ChipFwAndPhyVersion.PhyVersion[2],
                ver.ChipFwAndPhyVersion.PhyVersion[3]);
    }
}

void initNetwork() {
    UART_PRINT("[Blink] Network init\r\n");

    memset(g_ConnectionSSID, 0, sizeof(g_ConnectionSSID));
    memset(g_ConnectionBSSID, 0, sizeof(g_ConnectionBSSID));
    memset(g_WlanSSID, 0, sizeof(g_WlanSSID));
    memset(g_WlanSecurityKey, 0, sizeof(g_WlanSecurityKey));

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

        sl_NetAppStop(SL_NET_APP_HTTP_SERVER_ID);
    }

    if (status < 0) {
        sl_Stop(SL_STOP_TIMEOUT);
        UART_PRINT("[Blink] Network init error\r\n");
        ERR_PRINT(status);
        LOOP_FOREVER();
    }
}

void startConfigurationMode() {
    UART_PRINT("[Blink] Start provisioning\r\n");

    CLR_STATUS_BIT_ALL(g_Status);

    short status = sl_WlanSetMode(ROLE_AP);
    if (status < 0) {
        sl_Stop(SL_STOP_TIMEOUT);
        ERR_PRINT(status);
        LOOP_FOREVER();
    }

    _u8 macAddressVal[SL_MAC_ADDR_LEN];
    _u8 macAddressLen = SL_MAC_ADDR_LEN;
    sl_NetCfgGet(SL_MAC_ADDRESS_GET, NULL, &macAddressLen, (_u8 *)macAddressVal);

    char ssid[MAXIMAL_SSID_LENGTH + 1];
    sprintf(ssid, "%s-%s-%02X%02X%02X", "TL04", "fbdr000001a",
            macAddressVal[SL_MAC_ADDR_LEN-3],
            macAddressVal[SL_MAC_ADDR_LEN-2],
            macAddressVal[SL_MAC_ADDR_LEN-1]);
    int ssidLen = strlen(ssid);

    status = sl_WlanSet(SL_WLAN_CFG_AP_ID, WLAN_AP_OPT_SSID, ssidLen, (unsigned char*)ssid);
    if (status < 0) {
        sl_Stop(SL_STOP_TIMEOUT);
        ERR_PRINT(status);
        LOOP_FOREVER();
    }

    UART_PRINT("[Blink] Access point SSID         : %s\r\n", ssid);

    status = sl_Stop(SL_STOP_TIMEOUT);
    if (status < 0) {
        ERR_PRINT(status);
        LOOP_FOREVER();
    }

    CLR_STATUS_BIT_ALL(g_Status);

    status = sl_Start(0, 0, 0);
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

    status = sl_NetAppStop(SL_NET_APP_HTTP_SERVER_ID);
    if (status < 0) {
        sl_Stop(SL_STOP_TIMEOUT);
        ERR_PRINT(status);
        LOOP_FOREVER();
    }

    unsigned short port = 8080;
    status = sl_NetAppSet(SL_NET_APP_HTTP_SERVER_ID, NETAPP_SET_GET_HTTP_OPT_PORT_NUMBER, sizeof(port), (unsigned char*)&port);
    if (status < 0) {
        sl_Stop(SL_STOP_TIMEOUT);
        ERR_PRINT(status);
        LOOP_FOREVER();
    }

    status = sl_NetAppStart(SL_NET_APP_HTTP_SERVER_ID);
    if (status < 0) {
        sl_Stop(SL_STOP_TIMEOUT);
        ERR_PRINT(status);
        LOOP_FOREVER();
    }

    UART_PRINT("[Blink] Listening on              : 192.168.1.1:8080\r\n");

    while (!g_ProvisioningDone) {
#ifndef SL_PLATFORM_MULTI_THREADED
        _SlNonOsMainLoopTask();
#else
        osi_Sleep(100);
#endif
    }

    UART_PRINT("[Blink] Configuration received\r\n");

    status = sl_WlanProfileAdd(g_WlanSSID, strlen((char *)g_WlanSSID), 0, &g_SecParams, 0, 0, 0);
    if (status < 0) {
        sl_Stop(SL_STOP_TIMEOUT);
        ERR_PRINT(status);
        LOOP_FOREVER();
    }

    startApplicationMode();

    status = saveConfig();
    if (status < 0) {
        sl_Stop(SL_STOP_TIMEOUT);
        ERR_PRINT(status);
        LOOP_FOREVER();
    }

    resetBoard();
}

void startApplicationMode() {
    UART_PRINT("[Blink] Start application mode.\r\n");

    CLR_STATUS_BIT_ALL(g_Status);

    short status = sl_WlanPolicySet(SL_POLICY_CONNECTION, SL_CONNECTION_POLICY(1, 0, 0, 0, 0), NULL, 0);
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

    status = sl_Stop(SL_STOP_TIMEOUT);
    if (status < 0) {
        ERR_PRINT(status);
        LOOP_FOREVER();
    }

    status = sl_Start(0, 0, 0);
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

void resetNetwork() {
    // delete all stored profiles
    short status = sl_WlanProfileDel(0xFF);
    if (status < 0) {
        ERR_PRINT(status);
    }

    // Force disconnect
    sl_WlanDisconnect();

    // Stop the driver
    sl_Stop(SL_STOP_TIMEOUT);
}

/*********************************************************************
 * Network event handlers
 */

void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *httpServerEvent, SlHttpServerResponse_t *httpServerResponse)
{
    switch (httpServerEvent->Event) {
        case SL_NETAPP_HTTPGETTOKENVALUE_EVENT:
            break;

        case SL_NETAPP_HTTPPOSTTOKENVALUE_EVENT:
        {
            int tokenNameLen = httpServerEvent->EventData.httpPostData.token_name.len;
            char tokenName[256];
            memcpy(tokenName, httpServerEvent->EventData.httpPostData.token_name.data, tokenNameLen);
            tokenName[tokenNameLen] = 0;

            int tokenValueLen = httpServerEvent->EventData.httpPostData.token_value.len;
            char tokenValue[256];
            memcpy(tokenValue, httpServerEvent->EventData.httpPostData.token_value.data, tokenValueLen);
            tokenValue[tokenValueLen] = 0;

            UART_PRINT("[Blink] Parameter name            : %s\r\n", tokenName);
            UART_PRINT("        Parameter value           : %s\r\n", tokenValue);

            if ((0 == memcmp(tokenName, "__SL_P_USZ", tokenNameLen))
                    && (0 == memcmp( tokenValue, "Add", tokenValueLen))) {
                g_ProvisioningDone = 1;
            } else if (0 == memcmp(tokenName, "__SL_P_USA", tokenNameLen)) {
                memcpy(g_WlanSSID, tokenValue, tokenValueLen);
                g_WlanSSID[tokenValueLen] = 0;
            } else if (0 == memcmp(tokenName, "__SL_P_USB", tokenNameLen)) {
                if (tokenValue[0] == '0') {
                    g_SecParams.Type = SL_SEC_TYPE_OPEN;
                } else if (tokenValue[0] == '1') {
                    g_SecParams.Type = SL_SEC_TYPE_WEP;
                } else if (tokenValue[0] == '2') {
                    g_SecParams.Type = SL_SEC_TYPE_WPA;
                } else {
                    g_SecParams.Type = SL_SEC_TYPE_OPEN;
                }
            } else if (0 == memcmp(tokenName, "__SL_P_USC", tokenNameLen)) {
                memcpy(g_WlanSecurityKey, tokenValue, tokenValueLen);
                g_WlanSecurityKey[tokenValueLen] = 0;
                g_SecParams.Key = g_WlanSecurityKey;
                g_SecParams.KeyLen = tokenValueLen;
            } else if (0 == memcmp(tokenName, "__SL_P_USD", tokenNameLen)) {
                memcpy(g_ApplicationID, tokenValue, tokenValueLen);
                g_ApplicationID[tokenValueLen] = 0;
            } else if (0 == memcmp(tokenName, "__SL_P_USE", tokenNameLen)) {
                memcpy(g_ClientKey, tokenValue, tokenValueLen);
                g_ClientKey[tokenValueLen] = 0;
            } else if (0 == memcmp(tokenName, "__SL_P_USF", tokenNameLen)) {
                memcpy(g_InstallationID, tokenValue, tokenValueLen);
                g_InstallationID[tokenValueLen] = 0;
            } else if (0 == memcmp(tokenName, "__SL_P_USG", tokenNameLen)) {
                memcpy(g_SessionToken, tokenValue, tokenValueLen);
                g_SessionToken[tokenValueLen] = 0;
            } else if (0 == memcmp(tokenName, "__SL_P_USH", tokenNameLen)) {
                memcpy(g_DeviceName, tokenValue, tokenValueLen);
                g_DeviceName[tokenValueLen] = 0;
            }
        }
            break;

        default:
            break;
    }
}

void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent) {
    switch (pNetAppEvent->Event) {
        case SL_NETAPP_IPV4_IPACQUIRED_EVENT:
        {
            UART_PRINT("[Blink] IP address                : %d.%d.%d.%d\r\n",
                    SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip, 3),
                    SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip, 2),
                    SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip, 1),
                    SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip, 0));
            UART_PRINT("[Blink] Gateway                   : %d.%d.%d.%d\r\n",
                    SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway, 3),
                    SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway, 2),
                    SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway, 1),
                    SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway, 0));
            UART_PRINT("[Blink] DNS server                : %d.%d.%d.%d\r\n",
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
            UART_PRINT("[Blink] Connected to              : %s (%x:%x:%x:%x:%x:%x)\r\n",
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

            UART_PRINT("[Blink] Disconnected from         : %s (%x:%x:%x:%x:%x:%x)\r\n",
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
    switch (socket->Event) {
        case SL_SOCKET_TX_FAILED_EVENT:
        {
            switch (socket->socketAsyncEvent.SockTxFailData.status) {
                case SL_ECLOSE:
                    UART_PRINT("[Blink] Socket %d close operation failed to transmit all queued packets\r\n", socket->socketAsyncEvent.SockTxFailData.sd);
                    break;
                default:
                    UART_PRINT("[Blink] Socket %d error           : %d\r\n", socket->socketAsyncEvent.SockTxFailData.sd, socket->socketAsyncEvent.SockTxFailData.status);
            }
        }
            break;

        default:
            break;
    }
}

void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent) {
    UART_PRINT("[Blink] Event ID                  : %d\r\n", pDevEvent->EventData.deviceEvent.status);
    UART_PRINT("        Sender                    : %d\r\n", pDevEvent->EventData.deviceEvent.sender);
}
