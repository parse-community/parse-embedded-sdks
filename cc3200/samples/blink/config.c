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

#define CONFIG_FILENAME "blink.cfg"

char g_ApplicationID[APPLICATION_ID_MAX_LEN+1];
char g_ClientKey[CLIENT_KEY_MAX_LEN+1];
char g_InstallationID[INSTALLATION_ID_MAX_LEN+1];
char g_SessionToken[SESSION_TOKEN_MAX_LEN+1];
char g_DeviceName[DEVICE_NAME_MAX_LEN+1];

short initConfig() {
    short status = -1;

    memset(g_ApplicationID, 0, sizeof(g_ApplicationID));
    memset(g_ClientKey, 0, sizeof(g_ClientKey));
    memset(g_InstallationID, 0, sizeof(g_InstallationID));
    memset(g_SessionToken, 0, sizeof(g_SessionToken));
    memset(g_DeviceName, 0, sizeof(g_DeviceName));

    status = loadConfig();

    return status;
}

short loadConfig() {
    long status;
    long configFile;

    UART_PRINT("[Blink] Loading configuration\r\n");

    status = sl_FsOpen((unsigned char *)CONFIG_FILENAME, FS_MODE_OPEN_READ, NULL, &configFile);
    if (status < 0) {
        sl_FsClose(configFile, 0, 0, 0);
        return status;
    }

    long readOffset = 0;
    status = sl_FsRead(configFile, readOffset, (unsigned char *)g_ApplicationID, sizeof(g_ApplicationID));
    if (status < 0) {
        ERR_PRINT(status);
        sl_FsClose(configFile, 0, 0, 0);
        return status;
    }
    UART_PRINT("[Blink] Application Id            : %s\r\n", g_ApplicationID);

    readOffset = readOffset + status;
    status = sl_FsRead(configFile, readOffset, (unsigned char *)g_ClientKey, sizeof(g_ClientKey));
    if (status < 0) {
        ERR_PRINT(status);
        sl_FsClose(configFile, 0, 0, 0);
        return status;
    }
    UART_PRINT("[Blink] Client key                : %s\r\n", g_ClientKey);

    readOffset = readOffset + status;
    status = sl_FsRead(configFile, readOffset, (unsigned char *)g_InstallationID, sizeof(g_InstallationID));
    if (status < 0) {
        ERR_PRINT(status);
        sl_FsClose(configFile, 0, 0, 0);
        return status;
    }
    UART_PRINT("[Blink] Installation Id           : %s\r\n", g_InstallationID);

    readOffset = readOffset + status;
    status = sl_FsRead(configFile, readOffset, (unsigned char *)g_SessionToken, sizeof(g_SessionToken));
    if (status < 0) {
        ERR_PRINT(status);
        sl_FsClose(configFile, 0, 0, 0);
        return status;
    }
    UART_PRINT("[Blink] Session token             : %s\r\n", g_SessionToken);

    readOffset = readOffset + status;
    status = sl_FsRead(configFile, readOffset, (unsigned char *)g_DeviceName, sizeof(g_DeviceName));
    if (status < 0) {
        ERR_PRINT(status);
        sl_FsClose(configFile, 0, 0, 0);
        return status;
    }
    UART_PRINT("[Blink] Device name               : %s\r\n", g_DeviceName);

    sl_FsClose(configFile, 0, 0, 0);

    return status;
}

short saveConfig() {
    long status = 0;
    long configFile = -1;

    UART_PRINT("[Blink] Saving configuration\r\n");

    status = sl_FsOpen((unsigned char *) CONFIG_FILENAME,
            FS_MODE_OPEN_CREATE(65536, _FS_FILE_OPEN_FLAG_COMMIT | _FS_FILE_PUBLIC_WRITE),
            NULL, &configFile);
    if (status < 0) {
        ERR_PRINT(status);
        if (status == SL_FS_ERR_NO_AVAILABLE_BLOCKS) {
             UART_PRINT("[Blink] No available blocks. Need to flash your device file system...\r\n");
        }

        return status;
    }

    sl_FsClose(configFile, 0, 0, 0);

    configFile = -1;
    status = sl_FsOpen((unsigned char *) CONFIG_FILENAME, FS_MODE_OPEN_WRITE, NULL, &configFile);
    if (status < 0) {
        ERR_PRINT(status);
        if (status == SL_FS_ERR_NO_AVAILABLE_BLOCKS) {
             UART_PRINT("[Blink] No available blocks. Need to flash your device file system...\r\n");
        }
        sl_FsClose(configFile, 0, 0, 0);
        return status;
    }

    UART_PRINT("[Blink] Application Id            : %s\r\n", g_ApplicationID);
    long readOffset = 0;
    status = sl_FsWrite(configFile, readOffset, (unsigned char *)g_ApplicationID, sizeof(g_ApplicationID));
    if (status < 0) {
        ERR_PRINT(status);
        if (status == SL_FS_ERR_NO_AVAILABLE_BLOCKS) {
             UART_PRINT("[Blink] No available blocks. Need to flash your device file system...\r\n");
        }
        sl_FsClose(configFile, 0, 0, 0);
        return status;
    }

    UART_PRINT("[Blink] Client key                : %s\r\n", g_ClientKey);
    readOffset = readOffset + status;
    status = sl_FsWrite(configFile, readOffset, (unsigned char *)g_ClientKey, sizeof(g_ClientKey));
    if (status < 0) {
        ERR_PRINT(status);
        if (status == SL_FS_ERR_NO_AVAILABLE_BLOCKS) {
             UART_PRINT("[Blink] No available blocks. Need to flash your device file system...\r\n");
        }
        sl_FsClose(configFile, 0, 0, 0);
        return status;
    }

    UART_PRINT("[Blink] Installation Id           : %s\r\n", g_InstallationID);
    readOffset = readOffset + status;
    status = sl_FsWrite(configFile, readOffset, (unsigned char *)g_InstallationID, sizeof(g_InstallationID));
    if (status < 0) {
        ERR_PRINT(status);
        if (status == SL_FS_ERR_NO_AVAILABLE_BLOCKS) {
             UART_PRINT("[Blink] No available blocks. Need to flash your device file system...\r\n");
        }
        sl_FsClose(configFile, 0, 0, 0);
        return status;
    }

    UART_PRINT("[Blink] Session token             : %s\r\n", g_SessionToken);
    readOffset = readOffset + status;
    status = sl_FsWrite(configFile, readOffset, (unsigned char *)g_SessionToken, sizeof(g_SessionToken));
    if (status < 0) {
        ERR_PRINT(status);
        if (status == SL_FS_ERR_NO_AVAILABLE_BLOCKS) {
             UART_PRINT("[Blink] No available blocks. Need to flash your device file system...\r\n");
        }
        sl_FsClose(configFile, 0, 0, 0);
        return status;
    }

    UART_PRINT("[Blink] Device name               : %s\r\n", g_DeviceName);
    readOffset = readOffset + status;
    status = sl_FsWrite(configFile, readOffset, (unsigned char *)g_DeviceName, sizeof(g_DeviceName));
    if (status < 0) {
        ERR_PRINT(status);
        if (status == SL_FS_ERR_NO_AVAILABLE_BLOCKS) {
             UART_PRINT("[Blink] No available blocks. Need to flash your device file system...\r\n");
        }
        sl_FsClose(configFile, 0, 0, 0);
        return status;
    }

    sl_FsClose(configFile, 0, 0, 0);

    return status;
}

void resetConfig() {
    sl_FsDel((unsigned char *)CONFIG_FILENAME, 0);

    memset(g_ApplicationID, 0, sizeof(g_ApplicationID));
    memset(g_ClientKey, 0, sizeof(g_ClientKey));
    memset(g_InstallationID, 0, sizeof(g_InstallationID));
    memset(g_SessionToken, 0, sizeof(g_SessionToken));
    memset(g_DeviceName, 0, sizeof(g_DeviceName));
}
