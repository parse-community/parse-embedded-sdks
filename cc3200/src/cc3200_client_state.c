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

#include <simplelink.h>

#include <stdio.h>

#include "parse_impl.h"

char clientStateFileName[CLIENT_STATE_FILENAME_LEN+1];

short loadClientState(ParseClientInternal *parseClient) {
    snprintf(clientStateFileName, CLIENT_STATE_FILENAME_LEN, CLIENT_STATE_FILENAME, parseClient->applicationId);

    long clientStateFile;

#ifdef CLIENT_DEBUG
    DEBUG_PRINT("[Parse] Loading client state\r\n");
#endif /* CLIENT_DEBUG */

    short status = sl_FsOpen((unsigned char *)clientStateFileName, FS_MODE_OPEN_READ, NULL, &clientStateFile);
    if (status < 0) {
        sl_FsClose(clientStateFile, 0, 0, 0);
        return status;
    }

    long readOffset = 0;
    status = sl_FsRead(clientStateFile, readOffset, (unsigned char *)parseClient->installationId, sizeof(parseClient->installationId));
    if (status < 0) {
        sl_FsClose(clientStateFile, 0, 0, 0);
        return status;
    }
#ifdef CLIENT_DEBUG
    DEBUG_PRINT("[Parse] Installation Id: %s\r\n", parseClient->installationId);
#endif /* CLIENT_DEBUG */

    readOffset = readOffset + status;
    status = sl_FsRead(clientStateFile, readOffset, (unsigned char *)parseClient->sessionToken, sizeof(parseClient->sessionToken));
    if (status < 0) {
        sl_FsClose(clientStateFile, 0, 0, 0);
        return status;
    }
#ifdef CLIENT_DEBUG
    DEBUG_PRINT("[Parse] Session token: %s\r\n", parseClient->sessionToken);
#endif /* CLIENT_DEBUG */

    readOffset = readOffset + status;
    status = sl_FsRead(clientStateFile, readOffset, (unsigned char *)parseClient->lastPushTime, sizeof(parseClient->lastPushTime));
    if (status < 0) {
        sl_FsClose(clientStateFile, 0, 0, 0);
        return status;
    }
#ifdef CLIENT_DEBUG
    DEBUG_PRINT("[Parse] Last push time: %s\r\n", parseClient->lastPushTime);
#endif /* CLIENT_DEBUG */

    sl_FsClose(clientStateFile, 0, 0, 0);

    return status;
}

short saveClientState(ParseClientInternal *parseClient) {
#ifdef CLIENT_DEBUG
    DEBUG_PRINT("[Parse] Saving client state\r\n");
#endif /* CLIENT_DEBUG */

    snprintf(clientStateFileName, CLIENT_STATE_FILENAME_LEN, CLIENT_STATE_FILENAME, parseClient->applicationId);

    long clientStateFile = -1;

    sl_FsDel((unsigned char *)clientStateFileName, 0);

    short status = sl_FsOpen((unsigned char *)clientStateFileName,
            FS_MODE_OPEN_CREATE(65536, _FS_FILE_OPEN_FLAG_COMMIT | _FS_FILE_PUBLIC_WRITE),
            NULL, &clientStateFile);
    if (status < 0) {
#ifdef CLIENT_DEBUG
        if (status == SL_FS_ERR_NO_AVAILABLE_BLOCKS) {
             DEBUG_PRINT("[Parse] No available blocks. Need to flash your device file system...\r\n");
        }
#endif /* CLIENT_DEBUG */
        return status;
    }

    sl_FsClose(clientStateFile, 0, 0, 0);

    clientStateFile = -1;
    status = sl_FsOpen((unsigned char *)clientStateFileName, FS_MODE_OPEN_WRITE, NULL, &clientStateFile);
    if (status < 0) {
#ifdef CLIENT_DEBUG
        if (status == SL_FS_ERR_NO_AVAILABLE_BLOCKS) {
             DEBUG_PRINT("[Parse] No available blocks. Need to flash your device file system...\r\n");
        }
#endif /* CLIENT_DEBUG */
        sl_FsClose(clientStateFile, 0, 0, 0);
        return status;
    }

#ifdef CLIENT_DEBUG
    DEBUG_PRINT("[Parse] Installation Id: %s\r\n", parseClient->installationId);
#endif /* CLIENT_DEBUG */
    long writeOffset = 0;
    status = sl_FsWrite(clientStateFile, writeOffset, (unsigned char *)parseClient->installationId, sizeof(parseClient->installationId));
    if (status < 0) {
#ifdef CLIENT_DEBUG
        if (status == SL_FS_ERR_NO_AVAILABLE_BLOCKS) {
             DEBUG_PRINT("[Parse] No available blocks. Need to flash your device file system...\r\n");
        }
#endif /* CLIENT_DEBUG */
        sl_FsClose(clientStateFile, 0, 0, 0);
        return status;
    }

#ifdef CLIENT_DEBUG
    DEBUG_PRINT("[Parse] Session token: %s\r\n", parseClient->sessionToken);
#endif /* CLIENT_DEBUG */
    writeOffset = writeOffset + status;
    status = sl_FsWrite(clientStateFile, writeOffset, (unsigned char *)parseClient->sessionToken, sizeof(parseClient->sessionToken));
    if (status < 0) {
#ifdef CLIENT_DEBUG
        if (status == SL_FS_ERR_NO_AVAILABLE_BLOCKS) {
             DEBUG_PRINT("[Parse] No available blocks. Need to flash your device file system...\r\n");
        }
#endif /* CLIENT_DEBUG */
        sl_FsClose(clientStateFile, 0, 0, 0);
        return status;
    }

#ifdef CLIENT_DEBUG
    DEBUG_PRINT("[Parse] Last push time: %s\r\n", parseClient->lastPushTime);
#endif /* CLIENT_DEBUG */
    writeOffset = writeOffset + status;
    status = sl_FsWrite(clientStateFile, writeOffset, (unsigned char *)parseClient->lastPushTime, sizeof(parseClient->lastPushTime));
    if (status < 0) {
#ifdef CLIENT_DEBUG
        if (status == SL_FS_ERR_NO_AVAILABLE_BLOCKS) {
             DEBUG_PRINT("[Parse] No available blocks. Need to flash your device file system...\r\n");
        }
#endif /* CLIENT_DEBUG */
        sl_FsClose(clientStateFile, 0, 0, 0);
        return status;
    }

    sl_FsClose(clientStateFile, 0, 0, 0);

    return status;
}
