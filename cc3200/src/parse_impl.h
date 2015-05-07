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

#ifndef __PARSE_IMPL_H__
#define __PARSE_IMPL_H__

#ifdef __cplusplus
extern "C"
{
#endif

// Uncomment for console output
#define CONSOLE_DEBUG

// Uncomment for client state output on console
#define CLIENT_DEBUG

// Uncomment for detailed SSL output on console
#define SSL_DEBUG

// Uncomment for detailed request output on console
#define REQUEST_DEBUG

// Uncomment for trace request output on console
// #define REQUEST_TRACE

// Uncomment for detailed push output on console
#define PUSH_DEBUG

#ifdef CONSOLE_DEBUG
#define DEBUG_PRINT debugPrintf
#else
#define DEBUG_PRINT(...)
#endif /* CONSOLE_DEBUG */

#ifndef FALSE
#define FALSE       (0)
#endif

#ifndef TRUE
#define TRUE        (!FALSE)
#endif

#include <parse.h>
#include <simplejson.h>

#define OS_VERSION_MAX_LEN 19

extern char g_osVersion[OS_VERSION_MAX_LEN+1];

#define PUSH_TIME_MAX_LEN 40

// 10 minutes keep alive
#define PUSH_KEEP_ALIVE 10 * 60 * 1000

// Root CA file name
#define SL_SSL_CA_CERT_FILE_NAME "/parse/DigiCertHighAssuranceEVRootCA.der"

// Client state file, one file per app id used
#define CLIENT_STATE_FILENAME "/parse/%s"
#define CLIENT_STATE_FILENAME_LEN APPLICATION_ID_MAX_LEN+10

/* Client-related info - installation id, session token, etc. */
typedef struct _ParseClientInternal {
    char applicationId[APPLICATION_ID_MAX_LEN+1];
    char clientKey[CLIENT_KEY_MAX_LEN+1];
    char installationObjectId[OBJECT_ID_MAX_LEN+1];
    char installationId[INSTALLATION_ID_MAX_LEN+1];
    char sessionToken[SESSION_TOKEN_MAX_LEN+1];
    parsePushCallback callback;
    int socketHandle;
    int nFailedPing;
    char lastPushTime[PUSH_TIME_MAX_LEN + 1];
} ParseClientInternal;

/* cc3200_certificate.c */
void ensureCertificateFile();

/* cc3200_client_state.c */
short loadClientState(ParseClientInternal *parseClient);
short saveClientState(ParseClientInternal *parseClient);

/* cc3200_debug.c */
void debugPrint(const char *buffer);
int debugPrintf(const char *format, ...);

/* cc3200_utils.c */
int isPingTime();
void updateLastPingTime();
void fetchDeviceOSVersion();
void createNewInstallationId(ParseClientInternal *parseClient);

/* cc3200_socket.c */
short socketSslConnect(const char *host, unsigned short port);
short socketRead(short socketHandle, char *buffer, unsigned int receiveBufferSize, unsigned int timeoutMilliseconds);
short socketWrite(short socketHandle, const char *sendBuffer, unsigned int sendBufferSize);
short socketClose(short socketHandle);

/* http.c */
int beginHttpRequest(char *httpRequest, unsigned int httpRequestSize, const char *host, const char *httpVerb);
int beginHttpGetRequest(char *httpRequest, unsigned int httpRequestSize, const char *host, const char *httpVerb, const char *httpQuery);
int addHttpRequestHeader(char *httpRequest, unsigned int httpRequestSize, const char *httpHeader, const char *httpRequestHeaderValue);
int addHttpRequestHeaderInt(char *httpRequest, unsigned int httpRequestSize, const char *httpHeader, int httpRequestHeaderValue);
int getHttpResponseStatus(const char *httpResponse);
const char *getHttpResponseBody(const char *httpResponse);

/* installation.c */
void createInstallation(ParseClientInternal *parseClient);
void getInstallation(ParseClientInternal *parseClient);

/* parse_impl.c */
ParseClientInternal *getInternalClient(ParseClient client);

/* request.c */
void parseSendRequestInternal(ParseClient client, const char *httpVerb, const char *httpPath, const char *httpRequestBody, parseRequestCallback callback, int addInstallationHeader);

#ifdef __cplusplus
}
#endif

#endif /* __PARSE_IMPL_H__ */
