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
#include <ctype.h>
#include <strings.h>

#include "parse_impl.h"

static void createInstallationCallback(ParseClient client, int error, int httpStatus, const char* httpResponseBody) {
    ParseClientInternal *parseClient = getInternalClient(client);

    if ((error == 0) && (httpStatus >= 200 && httpStatus < 300)) {
        simpleJsonProcessor(httpResponseBody, "objectId", parseClient->installationObjectId, sizeof(parseClient->installationObjectId));
#ifdef CLIENT_DEBUG
        DEBUG_PRINT("[Parse] Installation object id: %s.\r\n", parseClient->installationObjectId);
#endif /* CLIENT_DEBUG */
    } else {
#ifdef CLIENT_DEBUG
        DEBUG_PRINT("[Parse] Failed to create installation. Error: %d, HTTP status: %d\r\n", error, httpStatus);
#endif /* CLIENT_DEBUG */
        memset(parseClient->installationObjectId, 0, sizeof(parseClient->installationObjectId));
    }
}

static void getInstallationCallback(ParseClient client, int error, int httpStatus, const char* httpResponseBody) {
    ParseClientInternal *parseClient = getInternalClient(client);

    if ((error == 0) && (httpStatus >= 200 && httpStatus < 300)) {
        simpleJsonProcessor(httpResponseBody, "objectId", parseClient->installationObjectId, sizeof(parseClient->installationObjectId));
        simpleJsonProcessor(httpResponseBody, "installationId", parseClient->installationId, sizeof(parseClient->installationId));
#ifdef CLIENT_DEBUG
        DEBUG_PRINT("[Parse] Installation object id: %s.\r\n", parseClient->installationObjectId);
        DEBUG_PRINT("[Parse] Installation id: %s.\r\n", parseClient->installationId);
#endif /* CLIENT_DEBUG */
    } else {
#ifdef CLIENT_DEBUG
        DEBUG_PRINT("[Parse] Failed to get installation. Error: %d, HTTP status: %d\r\n", error, httpStatus);
#endif /* CLIENT_DEBUG */
        memset(parseClient->installationObjectId, 0, sizeof(parseClient->installationObjectId));
    }
}

static void getInstallationByIdCallback(ParseClient client, int error, int httpStatus, const char* httpResponseBody) {
    ParseClientInternal *parseClient = getInternalClient(client);

    if ((error == 0) && (httpStatus >= 200 && httpStatus < 300)) {
        char queryResults[400];
        memset(queryResults, 0, sizeof(queryResults));
        short status = simpleJsonProcessor(httpResponseBody, "results", queryResults, sizeof(queryResults));
        if ((status != 0) && (queryResults[1] == '{')) {
            simpleJsonProcessor(queryResults+1, "objectId", parseClient->installationObjectId, sizeof(parseClient->installationObjectId));
            simpleJsonProcessor(queryResults+1, "installationId", parseClient->installationId, sizeof(parseClient->installationId));
#ifdef CLIENT_DEBUG
            DEBUG_PRINT("[Parse] Installation object id: %s.\r\n", parseClient->installationObjectId);
            DEBUG_PRINT("[Parse] Installation id: %s.\r\n", parseClient->installationId);
#endif /* CLIENT_DEBUG */
        }
    } else {
#ifdef CLIENT_DEBUG
        DEBUG_PRINT("[Parse] Failed to get installation. Error: %d, HTTP status: %d\r\n", error, httpStatus);
#endif /* CLIENT_DEBUG */
        memset(parseClient->installationObjectId, 0, sizeof(parseClient->installationObjectId));
    }
}

void createInstallation(ParseClientInternal *parseClient) {
    // If we have an installation object id, we don't need to create it again
    if (strlen(parseClient->installationObjectId) == 0) {
#ifdef CLIENT_DEBUG
        DEBUG_PRINT("[Parse] Creating new installation.\r\n");
#endif /* CLIENT_DEBUG */

        // Create new id if necessary
        if (strlen(parseClient->installationId) == 0) {
            createNewInstallationId(parseClient);
        }

        // Send installation create request and get the object id from the response.
        // If the response is a failure, set the instalaltionObjectId back to empty
        char content[120];
        snprintf(content, sizeof(content)-1, "{\"installationId\": \"%s\", \"deviceType\": \"embedded\", \"parseVersion\": \"1.0.0\"}", parseClient->installationId);

        parseSendRequestInternal((ParseClient)parseClient, "POST", "/1/installations", content, createInstallationCallback, FALSE);
    }
}

void getInstallation(ParseClientInternal *parseClient) {
    // If we have both installation id and installation object id, don't do anything
    if ((strlen(parseClient->installationObjectId) > 0) && (strlen(parseClient->installationId) > 0)) {
        return;
    }

    char content[150];

    // We have only installation object id or installation id
    if (strlen(parseClient->installationObjectId) > 0) {
        // This is just in case, we should never get in this branch in normal scenarios
        // as the device app will always give us installation id, and never installation object id
        snprintf(content, sizeof(content)-1, "/1/installations/%s", parseClient->installationObjectId);

        parseSendRequestInternal((ParseClient)parseClient, "GET", content, NULL, getInstallationCallback, FALSE);
    } else if (strlen(parseClient->installationId) > 0) {
        snprintf(content, sizeof(content)-1, "where=%%7b%%22installationId%%22%%3a+%%22%s%%22%%7d", parseClient->installationId);

        parseSendRequestInternal((ParseClient)parseClient, "GET", "/1/installations", content, getInstallationByIdCallback, FALSE);
    }

    // Go through create new installation, to catch the case we still don't have
    // an installation object id. This will be noop if we managed to get the installation
    // already
    createInstallation(parseClient);
}

void parseSetInstallationId(ParseClient client, const char *installationId) {
#ifdef CLIENT_DEBUG
    DEBUG_PRINT("[Parse] Setting new installation id: %s\r\n", installationId);
#endif /* CLIENT_DEBUG */

    ParseClientInternal *parseClient = getInternalClient(client);
    if ((installationId != NULL) && (strlen(installationId) > 0)) {
        if (strncasecmp(parseClient->installationId, installationId, INSTALLATION_ID_MAX_LEN) == 0) {
            return;
        }

        strncpy(parseClient->installationId, installationId, INSTALLATION_ID_MAX_LEN);

        int i = 0;
        for (i = 0; i < sizeof(parseClient->installationId); i++) {
            parseClient->installationId[i] = (char)tolower((int)(parseClient->installationId[i]));
        }

        getInstallation(parseClient);
    } else {
        if (strlen(parseClient->installationId) == 0) {
            return;
        }

        memset(parseClient->installationId, 0, sizeof(parseClient->installationId));
    }

    saveClientState(parseClient);
}

const char *parseGetInstallationId(ParseClient client) {
    ParseClientInternal *parseClient = getInternalClient(client);

    if (strlen(parseClient->installationId) == 0) {
        return NULL;
    }

    return parseClient->installationId;
}
