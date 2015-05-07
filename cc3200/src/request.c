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
#include <strings.h>

#include "parse_impl.h"

const char g_deviceClientVersion[] = "c-ti-cc3200-rtos-1.0.0";

static char parseServer[] = "api.parse.com";
unsigned short sshPort = 443;

#define REQUEST_HEADERS_SIZE 400
#define REQUEST_BODY_SIZE 500

// We need roughly 400 bytes for headers
// and about 500 bytes for a reasonably big object
// (enough to fit a full installation object)
// Both numbers are padded up, but let's add another
// 124 bytes just in case
#define RECEIVE_BUFFER_SIZE 1024

// Use only one preallocated buffer for now
// Consider allocating per request
char dataBuffer[RECEIVE_BUFFER_SIZE];

int buildRequestHeaders(ParseClientInternal *parseClient, const char *host, const char *httpVerb, const char *httpRequestBody, int addInstallationHeader) {
    int status = 0;

    int currentPosition = 0;
    int currentSize = sizeof(dataBuffer) - currentPosition - 1;

    int isGetRequest = strncasecmp(httpVerb, "GET", 3) == 0;
    int hasBody = (httpRequestBody != NULL) && (strlen(httpRequestBody) > 0);

    if (isGetRequest != FALSE) {
        hasBody = FALSE;
    }

    memset(dataBuffer, 0, sizeof(dataBuffer));

    if (isGetRequest != FALSE) {
        status = beginHttpGetRequest(dataBuffer + currentPosition, currentSize, host, httpVerb, httpRequestBody);
    } else {
        status = beginHttpRequest(dataBuffer + currentPosition, currentSize, host, httpVerb);
    }

    if (status >= 0) {
        currentPosition += status;
        currentSize -= status;
        status = addHttpRequestHeader(dataBuffer + currentPosition, currentSize, "Host", parseServer);
    }

    if (status >= 0) {
        currentPosition += status;
        currentSize -= status;
        status = addHttpRequestHeader(dataBuffer + currentPosition, currentSize, "User-Agent", g_deviceClientVersion);
    }

    if (status >= 0) {
        currentPosition += status;
        currentSize -= status;
        status = addHttpRequestHeader(dataBuffer + currentPosition, currentSize, "X-Parse-OS-Version", g_osVersion);
    }

    if (status >= 0) {
        currentPosition += status;
        currentSize -= status;
        status = addHttpRequestHeader(dataBuffer + currentPosition, currentSize, "X-Parse-Client-Version", g_deviceClientVersion);
    }

    if (status >= 0) {
        currentPosition += status;
        currentSize -= status;
        status = addHttpRequestHeader(dataBuffer + currentPosition, currentSize, "X-Parse-Application-Id", parseClient->applicationId);
    }

    if (status >= 0) {
        currentPosition += status;
        currentSize -= status;
        status = addHttpRequestHeader(dataBuffer + currentPosition, currentSize, "X-Parse-Client-Key", parseClient->clientKey);
    }

    if (addInstallationHeader) {
        if (status >= 0) {
            currentPosition += status;
            currentSize -= status;
            status = addHttpRequestHeader(dataBuffer + currentPosition, currentSize, "X-Parse-Installation-Id",  parseClient->installationId);
        }

        if (status >= 0) {
            currentPosition += status;
            currentSize -= status;
            status = addHttpRequestHeader(dataBuffer + currentPosition, currentSize, "X-Parse-Session-Token", parseClient->sessionToken);
        }
    }

    if (hasBody) {
        if (status >= 0) {
            currentPosition += status;
            currentSize -= status;
            status = addHttpRequestHeader(dataBuffer + currentPosition, currentSize, "Content-Type", "application/json; charset=utf-8");
        }

        if (status >= 0) {
            currentPosition += status;
            currentSize -= status;
            status = addHttpRequestHeaderInt(dataBuffer + currentPosition, currentSize, "Content-Length", strlen(httpRequestBody));
        }

        if (status >= 0) {
            currentPosition += status;
            currentSize -= status;
            status = snprintf(dataBuffer + currentPosition, currentSize, "\r\n");
        }

        if (status >= 0) {
            currentPosition += status;
            currentSize -= status;
            status = snprintf(dataBuffer + currentPosition, currentSize, "%s", httpRequestBody);
        }
    } else {
        if (status >= 0) {
            currentPosition += status;
            currentSize -= status;
            status = snprintf(dataBuffer + currentPosition, currentSize, "\r\n");
        }
    }


    if (status >= 0) {
        currentPosition += status;
        currentSize -= status;
    	dataBuffer[currentPosition] = 0;
    }

    return (status < 0) ? status : currentPosition;
}

int sendRequest(ParseClientInternal *parseClient, const char *host, const char *httpVerb, const char *httpRequestBody, int addInstallationHeader) {
    short socketHandle = -1;
    short status = socketSslConnect(parseServer, sshPort);

    if (status >= 0) {
        socketHandle = status;

#ifdef REQUEST_DEBUG
        DEBUG_PRINT("\r\n");

        DEBUG_PRINT("[Parse] ---------\r\n");
        DEBUG_PRINT("[Parse] Request :\r\n");
        DEBUG_PRINT("[Parse] --------- Start -\r\n");
#endif /* REQUEST_DEBUG */

        status = buildRequestHeaders(parseClient, host, httpVerb, httpRequestBody, addInstallationHeader);

#ifdef REQUEST_DEBUG
        if (status >= 0) {
            DEBUG_PRINT("%s\r\n", dataBuffer);
        } else {
            DEBUG_PRINT("[Parse] Build request error: %d\r\n", status);
        }

        DEBUG_PRINT("[Parse] ---------  End  -\r\n");
        DEBUG_PRINT("\r\n");
#endif /* REQUEST_DEBUG */
    }

    if (status >= 0) {
        status = socketWrite(socketHandle, dataBuffer, status);
    }

    if (status >= 0) {
#ifdef REQUEST_DEBUG
        DEBUG_PRINT("[Parse] ---------\r\n");
        DEBUG_PRINT("[Parse] Response:\r\n");
        DEBUG_PRINT("[Parse] --------- Start -\r\n");
#endif /* REQUEST_DEBUG */

        memset(dataBuffer, 0, sizeof(dataBuffer));
        status = socketRead(socketHandle, dataBuffer, sizeof(dataBuffer), 5000);

#ifdef REQUEST_DEBUG
        if (status >= 0) {
            DEBUG_PRINT("%s\r\n", dataBuffer);
        } else {
            DEBUG_PRINT("[Parse] Response read error: %d\r\n", status);
        }

        DEBUG_PRINT("[Parse] ---------  End  -\r\n");
        DEBUG_PRINT("\r\n");
#endif /* REQUEST_DEBUG */

        socketClose(socketHandle);
#ifdef REQUEST_DEBUG
    } else {
        DEBUG_PRINT("[Parse] Request write error: %d\r\n", status);
#endif /* REQUEST_DEBUG */
    }

    return status;
}

void parseSendRequestInternal(ParseClient client, const char *httpVerb, const char *httpPath, const char *httpRequestBody, parseRequestCallback callback, int addInstallationHeader) {
    short status = sendRequest(getInternalClient(client), httpPath, httpVerb, httpRequestBody, addInstallationHeader);

    if (callback) {
        if (status >= 0) {
            callback(client, 0, getHttpResponseStatus(dataBuffer), getHttpResponseBody(dataBuffer));
        } else {
            callback(client, status, -1, NULL);
        }
    }
}

void parseSendRequest(ParseClient client, const char *httpVerb, const char *httpPath, const char *httpRequestBody, parseRequestCallback callback) {
    getInstallation(getInternalClient(client));

#ifdef REQUEST_TRACE
    DEBUG_PRINT("[Parse] Send %s request to %s \r\n", httpVerb, httpPath);
#endif /* REQUEST_TRACE */

    parseSendRequestInternal(client, httpVerb, httpPath, httpRequestBody, callback, TRUE);
}

int parseGetErrorCode(const char *httpResponseBody) {
    // Simple implementation, just extract the 'code' value
    char code[10] = {0};
    if (simpleJsonProcessor(httpResponseBody, "code", code, 10) != 0) {
        return atoi(code);
    }

    return 2;
}
