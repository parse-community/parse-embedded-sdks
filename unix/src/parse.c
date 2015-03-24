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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <config.h>
#include <curl/curl.h>
#include <parse.h>
#include <parse_log.h>
#include <parse_os.h>
#include <parse_time.h>
#include <parse_uuid.h>

#include <simplejson.h>

#define PARSE_HEARTBIT_INTERVAL_SECONDS (14 * 60) // issue heartbeat every 14 minutes

#define PARSE_INSTALLATION_ID "installationID"
#define PARSE_SESSION_TOKEN "sessionToken"

typedef struct _ParseClientInternal {
    const char *applicationId;
    const char *clientKey;
    char *installationId;
    char *objectId;
    char *sessionToken;
    char *osVersion;
    parsePushCallback pushCallback;
    CURL *pushCurlHandle;
    unsigned long long lastHearbeat;
} ParseClientInternal;

typedef struct _ParseRequestDataInternal {
    ParseClient client;
    parseRequestCallback requestCallback;
    CURL *curl;
} ParseRequestDataInternal;

static void parseSendRequestInternal(
        ParseClient client,
        const char *httpVerb,
        const char* httpPath,
        const char* requestBody,
        parseRequestCallback callback,
        int useInstallationIdHeader);


ParseClientInternal *getClient(ParseClient client) {
    return (ParseClientInternal *)(client);
}

static void parseCreateInstallationIdIfNeeded(ParseClient client) {
    ParseClientInternal *clientInternal = getClient(client);

    if (clientInternal->installationId == NULL) {
        char id[37] = {0};
        parseOsLoadKey(clientInternal->applicationId, PARSE_INSTALLATION_ID, id, sizeof(id));
        parseLog(PARSE_LOG_INFO, "loaded installation id = '%s'\n", id);
        if (id[0] == '\0') {
            parseGetUUID(id, sizeof(id));
        }
        parseSetInstallationId(client, id);
    }
}

static size_t curlDataCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ParseRequestDataInternal *data = (ParseRequestDataInternal*)userp;
    long code = 0;
    char* temp = calloc(1, size * nmemb + 1);
    if (temp == NULL) {
        if (data->requestCallback != NULL) {
            data->requestCallback(data->client, ENOMEM, code, NULL);
        }
        parseLog(PARSE_LOG_ERROR, "%s:%s generated out of memory.\n", __FUNCTION__, __LINE__);
        return 0;
    }
    memcpy(temp, contents, size * nmemb);


    if (data->curl != NULL) {
        curl_easy_getinfo(data->curl, CURLINFO_RESPONSE_CODE, &code);
    }

    if (data->requestCallback != NULL) {
        data->requestCallback(data->client, 0, code, temp);
    }

    free(temp);

    return size * nmemb;
}

ParseClient parseInitialize(
        const char *applicationId,
        const char *clientKey)
{
    parseSetLogLevel(PARSE_LOG_NONE);

    ParseClientInternal *client = calloc(1, sizeof(ParseClientInternal));
    if (client == NULL) {
        parseLog(PARSE_LOG_ERROR, "%s:%s generated out of memory.\n", __FUNCTION__, __LINE__);
        return NULL;
    }
    if (applicationId != NULL)
        client->applicationId= strdup(applicationId);
    if (clientKey != NULL)
        client->clientKey = strdup(clientKey);

    char version[256];
    parseOsGetVersion(version, sizeof(version));
    client->osVersion = strdup(version);

    char temp[40];
    parseOsLoadKey(client->applicationId, PARSE_INSTALLATION_ID, temp, sizeof(temp));
    if (temp[0] != '\0') {
        parseSetInstallationId((ParseClient)client, temp);
    }

    parseOsLoadKey(client->applicationId, PARSE_SESSION_TOKEN, temp, sizeof(temp));
    if (temp[0] != '\0') {
        parseSetSessionToken((ParseClient)client, temp);
    }

    curl_global_init(CURL_GLOBAL_ALL);

    return (ParseClient)client;
}

static void setInstallationCallback(ParseClient client, int error, int httpStatus, const char* httpResponseBody)
{
    if (error != 0) {
        return;
    }

    ParseClientInternal *clientInternal = getClient(client);
    char value[64];
    if (simpleJsonProcessor(httpResponseBody, "objectId", value, sizeof(value))) {
        clientInternal->objectId = strdup(value);
    }
    parseLog(PARSE_LOG_INFO, "Got: %s\n", httpResponseBody);
}

void parseSetInstallationId(ParseClient client, const char *installationId)
{
    ParseClientInternal *clientInternal = getClient(client);

    if (installationId != NULL) {
        parseOsStoreKey(clientInternal->applicationId, PARSE_INSTALLATION_ID, installationId);
        if (clientInternal->installationId != NULL) {
            free(clientInternal->installationId);
        }
        clientInternal->installationId = strdup(installationId);

        int payloadLength = strlen("{\"installationId\":\"");
        payloadLength += strlen(clientInternal->installationId);
        payloadLength += strlen("\",\"deviceType\":\"embedded\"}") + 1;

        char *payload = calloc(sizeof(char), payloadLength);
        if (payload == NULL) {
            parseLog(PARSE_LOG_ERROR, "%s:%s generated out of memory.\n", __FUNCTION__, __LINE__);
            return;
        }
        strncat(payload, "{\"installationId\":\"", payloadLength - strlen(payload));
        strncat(payload, clientInternal->installationId, payloadLength - strlen(payload));
        strncat(payload, "\",\"deviceType\":\"embedded\"}", payloadLength - strlen(payload));

        parseSendRequestInternal(client, "POST", "/1/installations/", payload, setInstallationCallback, 0);
        free(payload);
    } else {
        parseOsClearKey(clientInternal->applicationId, PARSE_INSTALLATION_ID);
        if (clientInternal->installationId != NULL) {
            free(clientInternal->installationId);
            clientInternal->installationId = NULL;
        }
    }
}

const char *parseGetInstallationId(ParseClient client) 
{
    ParseClientInternal *clientInternal = getClient(client);
    return clientInternal->installationId;
}

static void getSessionObjectCallback(ParseClient client, int error, int httpStatus, const char* httpResponseBody) {
    ParseClientInternal *clientInternal = getClient(client);

    if (error == 0 && httpResponseBody != NULL) {
        // extract objectId for the session if no object id, this is invalid token
        char objectId[16];
        if (simpleJsonProcessor(httpResponseBody, "objectId", objectId, sizeof(objectId))) {
            char installationId[40];
            if (simpleJsonProcessor(httpResponseBody, "installationId", installationId, sizeof(installationId))) {
                if(strncmp(clientInternal->installationId, installationId, sizeof(installationId))) {
                    // else if installation id does not match we cannot use this ession
                    // let it fall but disregard it. 
                    parseLog(PARSE_LOG_WARN, "InstallationId does not match.\n");
                    return;
                }
                // if it has installation id and it matches we are done
                parseLog(PARSE_LOG_INFO, "Session and installationId matched.\n");
            } else {
                // if no installation id this is new session and need to 
                // associate it with simple PUT /1/sessions/me with empty body {}
                // and header with installation id.
                parseSendRequest(client, "PUT", "/1/sessions/me", "{}", NULL);
            }
        } else {
            // no session found, do nothing
            parseLog(PARSE_LOG_WARN, "My session is not found\n");
        }
    } else {
        parseLog(PARSE_LOG_ERROR, "Did not manage to talk to the server.\n");
    }
}

void parseSetSessionToken(ParseClient client, const char *sessionToken)
{
    ParseClientInternal *clientInternal = getClient(client);

    if (sessionToken != NULL) {
        parseCreateInstallationIdIfNeeded(client);
        parseOsStoreKey(clientInternal->applicationId, PARSE_SESSION_TOKEN, sessionToken);
        if (clientInternal->sessionToken != NULL) {
            free(clientInternal->sessionToken);
        }
        clientInternal->sessionToken= strdup(sessionToken);

        // query the session object by doing GET on /1/sessions/me
        parseSendRequest(client, "GET", "/1/sessions/me", NULL, getSessionObjectCallback);
    } else {
        parseOsClearKey(clientInternal->applicationId, PARSE_SESSION_TOKEN);
        if (clientInternal->sessionToken != NULL) {
            free(clientInternal->sessionToken);
            clientInternal->sessionToken = NULL;
        }
    }
}

void parseClearSessionToken(ParseClient client)
{
    parseSetSessionToken(client, NULL);
}

const char *parseGetSessionToken(ParseClient client)
{
    ParseClientInternal *clientInternal = getClient(client);
    return clientInternal->sessionToken;
}

void parseSetPushCallback(ParseClient client, parsePushCallback callback)
{
    ParseClientInternal *clientInternal = getClient(client);
    clientInternal->pushCallback = callback;
}

int parseStartPushService(ParseClient client)
{
    ParseClientInternal *clientInternal = getClient(client);
    CURLcode result = CURLE_OK;
    CURL *curl = NULL;

    if (clientInternal->pushCurlHandle != NULL) {
        return 0;
    }

    parseCreateInstallationIdIfNeeded(client);
    parseLog(PARSE_LOG_INFO, "Installation id: %s\n", clientInternal->installationId);

    curl = curl_easy_init();
    if (curl == NULL) {
        return CURLE_FAILED_INIT;
    }

    result = curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1L);
    if (result != CURLE_OK) {
        return result;
    }
    result = curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    if (result != CURLE_OK) {
        return result;
    }
    result = curl_easy_setopt(curl, CURLOPT_URL, "https://push.parse.com");
    if (result != CURLE_OK) {
        return result;
    }
    result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        return result;
    }

    clientInternal->pushCurlHandle = curl;

    size_t sent;

    char push[256];
    snprintf(push,
             sizeof(push),
             "{\"installation_id\":\"%s\", \"oauth_key\":\"%s\", \"v\":\"e1.0.0\", \"last\":null, \"ack_keep_alive\":true}\n",
             clientInternal->installationId,
             clientInternal->applicationId);

    curl_easy_send(curl, push, strlen(push), &sent);

    parseLog(PARSE_LOG_INFO, "Sending %i chars in request '%s'\n", sent, push); 

    curl_easy_send(curl, "{}\n", 3, &sent);
    clientInternal->lastHearbeat = secondsSinceBoot();

    return 0;
}

void parseStopPushService(ParseClient client)
{
    ParseClientInternal *clientInternal = getClient(client);
    if ( clientInternal->pushCurlHandle != NULL) {
        curl_easy_cleanup(clientInternal->pushCurlHandle);
        clientInternal->pushCurlHandle = NULL;
    }
}

static char parse_push_message_buffer[2048];
static int parse_push_message_size = 0;

int parseProcessNextPushNotification(ParseClient client)
{
    ParseClientInternal *clientInternal = getClient(client);
    if (clientInternal->pushCurlHandle != NULL) {
        size_t read = 0;;
        int length = -1;
        int start = 0;
        char* message = NULL;

        while (length == -1 && parse_push_message_size < sizeof(parse_push_message_buffer)) {
            CURLcode result = curl_easy_recv(clientInternal->pushCurlHandle,
                                             parse_push_message_buffer + parse_push_message_size,
                                             sizeof(parse_push_message_buffer) - parse_push_message_size,
                                             &read);
            if (result == CURLE_OK) {
                parseLog(PARSE_LOG_INFO, "got ok!\n");
                parse_push_message_size += read;
                message = (char *)getPushJson(parse_push_message_buffer,
                                              parse_push_message_size,
                                              &start,
                                              &length);
            } else if (result == CURLE_AGAIN) {
                break;
            } else {
                parseLog(PARSE_LOG_ERROR, "curl_easy_recv read %i an error %s\n", length, curl_easy_strerror(result));
                if (clientInternal->pushCallback != NULL) {
                    clientInternal->pushCallback(client, ECONNRESET, NULL);
                }
                return 0;
            }
        }
        if (length == -1 && parse_push_message_size == sizeof(parse_push_message_buffer)) {
            // this message is bigger than the buffer, we need to drop it :-(
            parse_push_message_size = 0;
            return 0;
        }

        parseLog(PARSE_LOG_INFO, "message = %p, start = %i, length = %i\n", message, start, length);

        if (length > 0 && message != NULL) {

            message[length] = '\0'; // We assume messages are separated by '\n'.
            if (strncmp("{}", message, 2) == 0) {
                // we got a hearbeat back
                parseLog(PARSE_LOG_DEBUG, "got heartbeat\n");
            } else if (clientInternal->pushCallback != NULL) {
                clientInternal->pushCallback(client, 0, message);
            }
            parse_push_message_size -= length + 1;
            if (parse_push_message_size > 0) {
                memmove(parse_push_message_buffer, parse_push_message_buffer + length + 1, parse_push_message_size);
            }

            return (parse_push_message_size > 0) ? 1 : 0;
        }

        unsigned int seconds = secondsSinceBoot();
        if (seconds > clientInternal->lastHearbeat + PARSE_HEARTBIT_INTERVAL_SECONDS) {
            clientInternal->lastHearbeat = seconds;
            size_t sent;
            curl_easy_send(clientInternal->pushCurlHandle, "{}\n", 3, &sent);
        }
    }
    return 0;
}

void parseRunPushLoop(ParseClient client) {
    while(1) {
        int socket = parseGetPushSocket(client);
        struct timeval tv;
        fd_set receive, send, error;
 
        tv.tv_sec = 10;
        tv.tv_usec= 0;

        FD_ZERO(&receive);
        FD_ZERO(&send);
        FD_ZERO(&error);

        FD_SET(socket, &error);
        FD_SET(socket, &receive);

        if(select(socket + 1, &receive, &send, &error, &tv) > 0) {
            if(FD_ISSET(socket, &error)) {
                parseLog(PARSE_LOG_WARN, "Push socket has problems.\n");
                parseStopPushService(client);
                ParseClientInternal *clientInternal = getClient(client);
                // we need to notify about the problem because push will not be happening
                // any more
                if (clientInternal->pushCallback != NULL) {
                    clientInternal->pushCallback(client, ECONNRESET, NULL);
                }
            }
        }
        parseProcessNextPushNotification(client);
    }
}

void parseSendRequest(
        ParseClient client,
        const char *httpVerb,
        const char* httpPath,
        const char* requestBody,
        parseRequestCallback callback)
{
    ParseClientInternal *clientInternal = getClient(client);
    parseCreateInstallationIdIfNeeded(client);
    parseSendRequestInternal(client, httpVerb, httpPath, requestBody, callback, 1);
}

static void parseSendRequestInternal(
        ParseClient client,
        const char *httpVerb,
        const char* httpPath,
        const char* requestBody,
        parseRequestCallback callback,
        int useInstallationIdHeader)
{
    ParseClientInternal *clientInternal = getClient(client);

    int getRequestBody = 0;
    CURLcode result = CURLE_OK;
    CURL *curl = NULL;
    curl = curl_easy_init();
    if (curl == NULL) {
        if (callback != NULL) callback(client, CURLE_FAILED_INIT, 0, NULL);
        return;
    }

    result = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, httpVerb);
    if (result != CURLE_OK) {
        if (callback != NULL) callback(client, result, 0, NULL);
        return;
    }

    if (requestBody != NULL) {
        if (strncmp(httpVerb, "GET", sizeof("GET"))) {
            result = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestBody);
            if (result != CURLE_OK) {
                if (callback != NULL) callback(client, result, 0, NULL);
                return;
            }
            result = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(requestBody));
            if (result != CURLE_OK) {
                if (callback != NULL) callback(client, result, 0, NULL);
                return;
            }
        } else {
            getRequestBody = 1;
        }
    }

    int urlSize = strlen("https://api.parse.com");
    urlSize += strlen(httpPath) + 1;
    char* getEncodedBody = NULL;
    if (getRequestBody) {
        urlSize += strlen(requestBody) + 1;
    }
    char *fullUrl = calloc(1, urlSize + 1);
    if (fullUrl == NULL) {
        parseLog(PARSE_LOG_ERROR, "%s:%s generated out of memory.\n", __FUNCTION__, __LINE__);
        if (callback != NULL) callback(client, ENOMEM, 0, NULL);
        return;
    }
    strncat(fullUrl, "https://api.parse.com", urlSize);
    strncat(fullUrl, httpPath, urlSize - strlen(fullUrl));
    if (getRequestBody) {
        strncat(fullUrl, "?", urlSize - strlen(fullUrl));
        strncat(fullUrl, requestBody, urlSize - strlen(fullUrl));
    }

    result = curl_easy_setopt(curl, CURLOPT_URL, fullUrl);
    free(fullUrl);

    if (result != CURLE_OK) {
        if (callback != NULL) callback(client, result, 0, NULL);
        return;
    }

    struct curl_slist *headers = NULL;

    char header[128] = {};
    snprintf(header, sizeof(header), "X-Parse-Application-Id: %s", clientInternal->applicationId);
    headers = curl_slist_append(headers, header);

    snprintf(header, sizeof(header), "X-Parse-Client-Version: c-embedded-%s", VERSION);
    headers = curl_slist_append(headers, header);

    snprintf(header, sizeof(header), "X-Parse-OS-Version: %s", clientInternal->osVersion);
    headers = curl_slist_append(headers, header);

    snprintf(header, sizeof(header), "X-Parse-Client-Key: %s", clientInternal->clientKey);
    headers = curl_slist_append(headers, header);

    if (useInstallationIdHeader && clientInternal->installationId) {
        snprintf(header, sizeof(header), "X-Parse-Installation-Id: %s", clientInternal->installationId);
        headers = curl_slist_append(headers, header);
    }

    if (clientInternal->sessionToken) {
        snprintf(header, sizeof(header), "X-Parse-Session-Token: %s", clientInternal->sessionToken);
        headers = curl_slist_append(headers, header);
    }

    headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");
    headers = curl_slist_append(headers, "Connection: close");
    headers = curl_slist_append(headers, "Accept:"); // This line removes 'Accpet' header.

    result = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    if (result != CURLE_OK) {
        if (callback != NULL) callback(client, result, 0, NULL);
        return;
    }
         
    result = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlDataCallback);
    if (result != CURLE_OK) {
        if (callback != NULL) callback(client, result, 0, NULL);
        return;
    }
        
    ParseRequestDataInternal data;
    data.client = client;
    data.requestCallback = callback;
    data.curl = curl;

    result = curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&data);
    if (result != CURLE_OK) {
        if (callback != NULL) callback(client, result, 0, NULL);
        return;
    }

    result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        if (callback != NULL) callback(client, result, 0, NULL);
        return;
    }
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

}

int parseGetErrorCode(const char *httpResponseBody)
{
    char value[16];
    if (simpleJsonProcessor(httpResponseBody, "code", value, sizeof(value))) {
        return strtoumax(value, NULL, 10);
    }
    return 0;
}

/* 
 *  OS specific functions 
 */
int parseGetPushSocket(ParseClient client)
{
    ParseClientInternal *clientInternal = getClient(client);
    long socket;
    if (clientInternal->pushCurlHandle == NULL) {
        return -2;
    }
    CURLcode result = curl_easy_getinfo(clientInternal->pushCurlHandle, CURLINFO_LASTSOCKET, &socket);
    if (result != CURLE_OK && result != CURLE_UNSUPPORTED_PROTOCOL) {
        parseLog(PARSE_LOG_INFO, "error: %i, %s\n", result, curl_easy_strerror(result));
        parseLog(PARSE_LOG_INFO, "socket: %i\n", (int)socket);
        return -3;
    }
    return (int)socket;
}

