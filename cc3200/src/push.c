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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parse_impl.h"

static char pushServer[] = "push.parse.com";
unsigned short pushPort = 443;

// 3 seconds timeout for reads
#define PUSH_READ_TIMEOUT 3000

#define RECEIVE_BUFFER_SIZE 1024

char pushDataBuffer[RECEIVE_BUFFER_SIZE];
static int pushDataBufferUsed = 0;

void parseSetPushCallback(ParseClient client, parsePushCallback callback) {
    getInternalClient(client)->callback = callback;
}

int parseStartPushService(ParseClient client) {
    ParseClientInternal *parseClient = getInternalClient(client);

    getInstallation(parseClient);

    // Always assume success initially
    short status = 0;

    short socketHandle = socketSslConnect(pushServer, pushPort);

    if (socketHandle >= 0) {
#ifdef CLIENT_DEBUG
        DEBUG_PRINT("[Parse] Push service started.\r\n");
#endif /* CLIENT_DEBUG */

        int lastPushTimeLen = strlen(parseClient->lastPushTime);
        snprintf(pushDataBuffer,
        		 sizeof(pushDataBuffer)-1,
                 "{\"installation_id\":\"%s\", \"oauth_key\":\"%s\", \"v\": \"e1.0.0\", \"last\": %s%s%s}\r\n",
                 parseClient->installationId,
                 parseClient->applicationId,
                 lastPushTimeLen > 0 ? "\"" : "",
                 lastPushTimeLen > 0 ? parseClient->lastPushTime : "null",
                 lastPushTimeLen > 0 ? "\"" : ""
                );

#ifdef PUSH_DEBUG
        DEBUG_PRINT("[Parse] Sending %s\r\n", pushDataBuffer);
#endif /* PUSH_DEBUG */

        status = socketWrite(socketHandle, pushDataBuffer, 0);

        if (status < 0) {
#ifdef PUSH_DEBUG
            DEBUG_PRINT("[Parse] Hadshake send failed. Push socket closed.\r\n");
#endif /* PUSH_DEBUG */
            socketClose(socketHandle);
#ifdef CLIENT_DEBUG
            DEBUG_PRINT("[Parse] Push service stopped.\r\n");
#endif /* CLIENT_DEBUG */
        } else {
            parseClient->socketHandle = socketHandle;
        }
    } else {
#ifdef CLIENT_DEBUG
        DEBUG_PRINT("[Parse] Failed to open push socket.\r\n");
#endif /* CLIENT_DEBUG */
        status = socketHandle;
    }

    if (status < 0) {
#ifdef PUSH_DEBUG
        DEBUG_PRINT("[Parse] Calling callback with error\r\n");
#endif /* PUSH_DEBUG */
        // Call the callback on error
        if (parseClient->callback) {
            parseClient->callback(client, status, NULL);
        }
    }

    return status;
}

void parseStopPushService(ParseClient client) {
    ParseClientInternal *parseClient = getInternalClient(client);

    socketClose(parseClient->socketHandle);

    parseClient->socketHandle = -1;
    parseClient->nFailedPing = 0;
    parseClient->callback = NULL;

#ifdef CLIENT_DEBUG
    DEBUG_PRINT("[Parse] Push service stopped.\r\n");
#endif /* CLIENT_DEBUG */
}

void sendPing(ParseClientInternal *parseClient) {
    if (isPingTime() != 0) {
        // send a ping to keep connection
        short status = socketWrite(parseClient->socketHandle, "{}\r\n", 0);
        if (status < 0) {
            if (parseClient->nFailedPing > 20) {
#ifdef PUSH_DEBUG
                DEBUG_PRINT("[Parse] Failed to send too many pings\r\n");
#endif /* PUSH_DEBUG */

                if (parseClient->callback != NULL) {
                    parseClient->callback((ParseClient)parseClient, -1, NULL);
                }

                // Close the handle and stop the service
                socketClose(parseClient->socketHandle);
                parseClient->socketHandle = -1;
#ifdef CLIENT_DEBUG
                DEBUG_PRINT("[Parse] Push service stopped.\r\n");
#endif /* CLIENT_DEBUG */
            } else {
                parseClient->nFailedPing++;
            }
        } else {
            parseClient->nFailedPing = 0;
            updateLastPingTime();
        }
    }
}

int parseProcessNextPushNotification(ParseClient client) {
    ParseClientInternal *parseClient = getInternalClient(client);

    if (parseClient->socketHandle > 0) {
        int length = -1;
        int start = 0;
        char* message = NULL;

        while ((length == -1) && (pushDataBufferUsed < sizeof(pushDataBuffer))) {
            int len = socketRead(parseClient->socketHandle, pushDataBuffer + pushDataBufferUsed, sizeof(pushDataBuffer) - pushDataBufferUsed, PUSH_READ_TIMEOUT);
            if (len > 0) {
                pushDataBufferUsed += len;
                message = (char *)getPushJson(pushDataBuffer, pushDataBufferUsed, &start, &length);
            } else {
                break;
            }
        }

        if ((length == -1) && (pushDataBufferUsed == sizeof(pushDataBuffer))) {
            // this message is bigger than the buffer, we need to drop it :-(
            pushDataBufferUsed = 0;
            return 0;
        }

        if (length > 0 && message != NULL) {
            // We assume messages are separated by '\n'.
            message[length] = '\0';

#ifdef PUSH_DEBUG
			DEBUG_PRINT("[Parse] Push notification: %s\r\n", message);
#endif /* PUSH_DEBUG */

			if (simpleJsonProcessor(message, "time", parseClient->lastPushTime, PUSH_TIME_MAX_LEN)) {
#ifdef PUSH_DEBUG
				DEBUG_PRINT("[Parse] Last push time: %s\r\n", parseClient->lastPushTime);
#endif /* PUSH_DEBUG */
				saveClientState(parseClient);
            }

            if (strncmp("{}", message, 2) == 0) {
            	// we got a heartbeat back
            } else if (parseClient->callback != NULL) {
                parseClient->callback(client, 0, message);
            }

            pushDataBufferUsed -= length + 1;
            if (pushDataBufferUsed > 0) {
                memmove(pushDataBuffer, pushDataBuffer + length + 1, pushDataBufferUsed);
            }

            return (pushDataBufferUsed > 0) ? 1 : 0;
        }

        sendPing(parseClient);
    }

    return 0;
}

int parseGetPushSocket(ParseClient client) {
    return getInternalClient(client)->socketHandle;
}

void parseRunPushLoop(ParseClient client) {
    ParseClientInternal *parseClient = getInternalClient(client);

    // Exit if push service is not started, or is closed
    while (parseClient->socketHandle != -1) {
        parseProcessNextPushNotification(client);
    }
}
