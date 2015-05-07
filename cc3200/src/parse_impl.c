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

ParseClient parseInitialize(const char *applicationId, const char *clientKey) {
#ifdef CLIENT_DEBUG
    DEBUG_PRINT("[Parse] Initializing new client.\r\n");
#endif /* CLIENT_DEBUG */

    // Do any one-time initialization of the SDK
    fetchDeviceOSVersion();

    // Do any one-time intialization of the new client
    ParseClientInternal *parseClient = (ParseClientInternal *)malloc(sizeof(ParseClientInternal));

    if (parseClient != NULL) {
		memset(parseClient->applicationId, 0, sizeof(parseClient->applicationId));
		memset(parseClient->clientKey, 0, sizeof(parseClient->clientKey));
		memset(parseClient->installationObjectId, 0, sizeof(parseClient->installationObjectId));
		memset(parseClient->installationId, 0, sizeof(parseClient->installationId));
		memset(parseClient->sessionToken, 0, sizeof(parseClient->sessionToken));

		// Copy the application id and the client key to the client
		strncpy(parseClient->applicationId, applicationId, APPLICATION_ID_MAX_LEN);
		strncpy(parseClient->clientKey, clientKey, CLIENT_KEY_MAX_LEN);

#ifdef CLIENT_DEBUG
		DEBUG_PRINT("[Parse] Application id: %s.\r\n", parseClient->applicationId);
		DEBUG_PRINT("[Parse] Client key: %s.\r\n", parseClient->clientKey);
#endif /* CLIENT_DEBUG */

		// initialize the push service data
		parseClient->socketHandle = -1;
		parseClient->callback = NULL;
		parseClient->nFailedPing = 0;
		memset(parseClient->lastPushTime, 0, sizeof(parseClient->lastPushTime));

		loadClientState(parseClient);
#ifdef CLIENT_DEBUG
    } else {
		DEBUG_PRINT("[Parse] Failed to allocate client handle.\r\n");
#endif /* CLIENT_DEBUG */
    }

    return (ParseClient)parseClient;
}

ParseClientInternal *getInternalClient(ParseClient client) {
    return (ParseClientInternal *)(client);
}
