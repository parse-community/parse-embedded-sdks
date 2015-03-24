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

void associateSessionWithInstallation(ParseClientInternal *parseClient) {
    // We can associate the session only if we have a valid installation
    if (strlen(parseClient->installationObjectId) > 0) {
        // No callback as there isn't anything we can do if this fails
        parseSendRequestInternal((ParseClient)parseClient, "PUT", "/1/sessions/me", "{}\r\n", NULL, TRUE);
    }
}

static void getSessionCallback(ParseClient client, int error, int httpStatus, const char* httpResponseBody) {
    ParseClientInternal *parseClient = getInternalClient(client);

    if ((error == 0) && (httpStatus >= 200 && httpStatus < 300)) {
        char installationId[INSTALLATION_ID_MAX_LEN+1];
        short status = simpleJsonProcessor(httpResponseBody, "installationId", installationId, sizeof(installationId));
        if (status == FALSE) {
            associateSessionWithInstallation(parseClient);
#ifdef CLIENT_DEBUG
        } else {
            if (strncasecmp(installationId, parseClient->installationId, sizeof(parseClient->installationId)) != 0) {
                // The session token is for another installation, so all client calls should fail
                // We can't do anything sane here (restricted session can't create another restricted session)
                // but we still want to let the developer know
                DEBUG_PRINT("[Parse] The session token is for different installation.\r\n");
            } else {
                DEBUG_PRINT("[Parse] The session token is for our installation.\r\n");
            }
#endif /* CLIENT_DEBUG */
        }
#ifdef CLIENT_DEBUG
    } else {
        DEBUG_PRINT("[Parse] Failed to get session. Error: %d, HTTP status: %d\r\n", error, httpStatus);
#endif /* CLIENT_DEBUG */
    }
}

void getSession(ParseClientInternal *parseClient) {
    getInstallation(parseClient);

    parseSendRequestInternal((ParseClient)parseClient, "GET", "/1/sessions/me", NULL, getSessionCallback, TRUE);
}

void parseSetSessionToken(ParseClient client, const char *sessionToken) {
#ifdef CLIENT_DEBUG
    DEBUG_PRINT("[Parse] Setting new session token.\r\n");
#endif /* CLIENT_DEBUG */

    ParseClientInternal *parseClient = getInternalClient(client);
    if ((sessionToken != NULL) && (strlen(sessionToken) > 0)) {
        if (strncasecmp(parseClient->sessionToken, sessionToken, SESSION_TOKEN_MAX_LEN) == 0) {
            return;
        }

        strncpy(parseClient->sessionToken, sessionToken, SESSION_TOKEN_MAX_LEN);

        getSession(parseClient);
    } else {
        if (strlen(parseClient->sessionToken) == 0) {
            return;
        }

        memset(parseClient->sessionToken, 0, sizeof(parseClient->sessionToken));
    }

    saveClientState(parseClient);
}

void parseClearSessionToken(ParseClient client) {
    parseSetSessionToken(client, NULL);
}

const char *parseGetSessionToken(ParseClient client) {
    ParseClientInternal *parseClient = getInternalClient(client);

    if (strlen(parseClient->sessionToken) == 0) {
        return NULL;
    }

    return parseClient->sessionToken;
}
