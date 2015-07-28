/*
 * parse_api_impl.c
 *
 * Created: 1/8/2015 4:55:06 PM
 *  Author: yakovlev
 */

#include "asf.h"
#include "parse.h"
#include "nm_fluffy.h"
#include "parse_impl.h"
#include "simplejson.h"

// For PUSH, temp, must be changed(removed) after NVRAM will be implemented.
char g_applicationId[APPLICATION_ID_MAX_LEN+1];
char g_installationId[INSTALLATION_ID_MAX_LEN+1];

#define CLIENT_DEBUG

static uint32_t s_randomSeed = 0;

/*
 * !!! IMPORTANT !!!
 * This function does not conform to RFC 4122, even though it returns
 * a string formatted as an UUID. Do not use this to generate proper UUID!
 * Currently this function generetes the same id every time do not use for other than initial test
 */
void createNewInstallationId(ParseClientInternal *parseClient) {
  static int randInitialized = 0;

  if (!randInitialized) {
    // Somewhat prone to collision, use for testing purposes only.
    srand(s_randomSeed);
    randInitialized = 1;
  }

  sprintf(parseClient->installationId, "%x%x%x%x%x%x%x%x-%x%x%x%x-%x%x%x%x-%x%x%x%x-%x%x%x%x%x%x%x%x%x%x%x%x",
    rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16,
    rand()%16, rand()%16, rand()%16, rand()%16,
    rand()%16, rand()%16, rand()%16, rand()%16,
    rand()%16, rand()%16, rand()%16, rand()%16,
    rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16
  );
  deviceConfigSetValue(0, "installationId", parseClient->installationId);
  strncpy(g_installationId, parseClient->installationId, INSTALLATION_ID_MAX_LEN);
}

ParseClientInternal *getInternalClient(ParseClient client);
static void parseSendRequestInternal(ParseClient client, const char *httpVerb, const char *httpPath, const char *httpRequestBody, parseRequestCallback callback, int addInstallHeader);

static void createInstallationCallback(ParseClient client, int error, int httpStatus, const char* httpResponseBody) {
    ParseClientInternal *parseClient = getInternalClient(client);

    if ((error == 0) && (httpStatus >= 200 && httpStatus < 300)) {
        simpleJsonProcessor(httpResponseBody, "objectId", parseClient->installationObjectId, sizeof(parseClient->installationObjectId));
        deviceConfigSetValue(0, "installationObjectId", parseClient->installationObjectId);
#ifdef CLIENT_DEBUG
        printf("[Parse] Installation object id: %s.\r\n", parseClient->installationObjectId);
#endif /* CLIENT_DEBUG */
        deviceConfigCommitValues(0);
    } else {
#ifdef CLIENT_DEBUG
        printf("[Parse] Failed to create installation. Error: %d, HTTP status: %d\r\n", error, httpStatus);
#endif /* CLIENT_DEBUG */
        memset(parseClient->installationObjectId, 0, sizeof(parseClient->installationObjectId));
    }
}

static void getInstallationCallback(ParseClient client, int error, int httpStatus, const char* httpResponseBody) {
    ParseClientInternal *parseClient = getInternalClient(client);

    if ((error == 0) && (httpStatus >= 200 && httpStatus < 300)) {
        simpleJsonProcessor(httpResponseBody, "objectId", parseClient->installationObjectId, sizeof(parseClient->installationObjectId));
        simpleJsonProcessor(httpResponseBody, "installationId", parseClient->installationId, sizeof(parseClient->installationId));
        deviceConfigSetValue(0, "installationId", parseClient->installationId);
        deviceConfigSetValue(0, "installationObjectId", parseClient->installationObjectId);
        strncpy(g_installationId, parseClient->installationId, INSTALLATION_ID_MAX_LEN);
#ifdef CLIENT_DEBUG
        printf("[Parse] Installation object id: %s.\r\n", parseClient->installationObjectId);
        printf("[Parse] Installation id: %s.\r\n", parseClient->installationId);
#endif /* CLIENT_DEBUG */
        deviceConfigCommitValues(0);
    } else {
#ifdef CLIENT_DEBUG
        printf("[Parse] Failed to get installation. Error: %d, HTTP status: %d\r\n", error, httpStatus);
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
            deviceConfigSetValue(0, "installationId", parseClient->installationId);
            deviceConfigSetValue(0, "installationObjectId", parseClient->installationObjectId);
            strncpy(g_installationId, parseClient->installationId, INSTALLATION_ID_MAX_LEN);
#ifdef CLIENT_DEBUG
            printf("[Parse] Installation object id: %s.\r\n", parseClient->installationObjectId);
            printf("[Parse] Installation id: %s.\r\n", parseClient->installationId);
#endif /* CLIENT_DEBUG */
            deviceConfigCommitValues(0);
        }
    } else {
#ifdef CLIENT_DEBUG
        printf("[Parse] Failed to get installation. Error: %d, HTTP status: %d\r\n", error, httpStatus);
#endif /* CLIENT_DEBUG */
        memset(parseClient->installationObjectId, 0, sizeof(parseClient->installationObjectId));
    }
}

static void createInstallation(ParseClientInternal *parseClient) {
    // If we have an installation object id, we don't need to create it again
    if (strlen(parseClient->installationObjectId) == 0 && strlen(parseClient->installationId) == 0) {
        // Create new id if necessary
        if (strlen(parseClient->installationId) == 0) {
            createNewInstallationId(parseClient);
        }

        // Send installation create request and get the object id from the response.
        // If the response is a failure, set the instalaltionObjectId back to empty
        char content[120];
        sprintf(content, "{\"installationId\": \"%s\", \"deviceType\": \"embedded\", \"parseVersion\": \"1.0.0\"}", parseClient->installationId);

        parseSendRequestInternal(parseClient, "POST", "/1/installations", content, createInstallationCallback, 0);
    }
}

static void getInstallation(ParseClientInternal *parseClient) {
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

        parseSendRequestInternal(parseClient, "GET", content, NULL, getInstallationCallback, 0);
    } else if (strlen(parseClient->installationId) > 0) {
        snprintf(content, sizeof(content)-1, "/1/installations?where=%%7b%%22installationId%%22%%3a+%%22%s%%22%%7d", parseClient->installationId);

        parseSendRequestInternal(parseClient, "GET", content, NULL, getInstallationByIdCallback, 0);
    }

    // Go through create new installation, to catch the case we still don't have
    // an installation object id. This will be noop if we managed to get the installation
    // already
    createInstallation(parseClient);

    localWaitForRequestToFinish(parseClient);
}


ParseClient parseInitialize(const char *applicationId, const char *clientKey)
{
    ParseClientInternal *parseClient = (ParseClientInternal *)malloc(sizeof(ParseClientInternal));
    // This seeds number of processor ticks elapsed since last delay() call
    // It is semi-random, but still better than to count the seconds.
    s_randomSeed = SysTick->VAL;

    memset(parseClient->applicationId, 0, sizeof(parseClient->applicationId));
    memset(parseClient->clientKey, 0, sizeof(parseClient->clientKey));
    memset(parseClient->installationId, 0, sizeof(parseClient->installationId));
    memset(parseClient->installationObjectId, 0, sizeof(parseClient->installationObjectId));
    memset(parseClient->sessionToken, 0, sizeof(parseClient->sessionToken));

    fluffy_load_fluffy_info();

    // Copy the application id and the client key to the client
    if (applicationId != NULL) {
      strncpy(parseClient->applicationId, applicationId, APPLICATION_ID_MAX_LEN);
      deviceConfigSetValue(0, "applicationId", applicationId);
    } else {
      const char *tmp = deviceConfigGetValue(0, "applicationId");
      if (tmp != NULL)
        strncpy(parseClient->applicationId, tmp, APPLICATION_ID_MAX_LEN);
    }

    if (clientKey != NULL) {
      strncpy(parseClient->clientKey, clientKey, CLIENT_KEY_MAX_LEN);
      deviceConfigSetValue(0, "clientKey", clientKey);
    } else {
      const char *tmp = deviceConfigGetValue(0, "clientKey");
      if (tmp != NULL)
        strncpy(parseClient->clientKey, tmp, CLIENT_KEY_MAX_LEN);
    }

    const char *tmp = deviceConfigGetValue(0, "sessionToken");
    if (tmp != NULL)
      strncpy(parseClient->sessionToken, tmp, SESSION_TOKEN_MAX_LEN);

    tmp = deviceConfigGetValue(0, "installationObjectId");
    if (tmp != NULL)
      ; //strncpy(parseClient->installationObjectId, tmp, INSTALLATION_ID_MAX_LEN);

    parseClient->parsePushCallback = NULL;
    parseClient->parseRequestCallback = NULL;
    parseClient->isStartPushService = 0;

    memset(g_applicationId, 0, sizeof(g_applicationId));
    memset(g_installationId, 0, sizeof(g_installationId));
    strncpy(g_applicationId, applicationId, APPLICATION_ID_MAX_LEN);

    tmp = deviceConfigGetValue(0, "installationId");
    if (tmp != NULL) {
        strncpy(parseClient->installationId, tmp, INSTALLATION_ID_MAX_LEN);
        strncpy(g_installationId, parseClient->installationId, INSTALLATION_ID_MAX_LEN);
    }

    return (ParseClient)parseClient;
}

ParseClientInternal *getInternalClient(ParseClient client) {
    return (ParseClientInternal *)(client);
}

void parseSetInstallationId(ParseClient client, const char *installationId)
{
    ParseClientInternal *parseClient = getInternalClient(client);

    if ((installationId != NULL) && (strlen(installationId) > 0)) {
        strncpy(parseClient->installationId, installationId, INSTALLATION_ID_MAX_LEN);
        deviceConfigSetValue(0, "installationId", parseClient->installationId);

        strncpy(g_installationId, installationId, INSTALLATION_ID_MAX_LEN);
        getInstallation(parseClient);
    } else {
        // Error
    }
}

const char *parseGetInstallationId(ParseClient client)
{
    ParseClientInternal *parseClient = getInternalClient(client);
    getInstallation(parseClient);

    if (strlen(parseClient->installationId) == 0) {
        return NULL;
    }

    return parseClient->installationId;
}

void parseSetSessionToken(ParseClient client, const char *sessionToken)
{
    ParseClientInternal *parseClient = getInternalClient(client);

    if ((sessionToken != NULL) && (strlen(sessionToken) > 0)) {
        strncpy(parseClient->sessionToken, sessionToken, SESSION_TOKEN_MAX_LEN);
        deviceConfigSetValue(0, "sessionToken", parseClient->sessionToken);
    } else {
        // Error
    }
}

void parseClearSessionToken(ParseClient client)
{
    ParseClientInternal *parseClient = getInternalClient(client);

    if (strlen(parseClient->sessionToken) == 0) {
        return NULL;
    }

    memset(parseClient->sessionToken, 0, SESSION_TOKEN_MAX_LEN);
    deviceConfigSetValue(0, "sessionToken", parseClient->sessionToken);
}

const char *parseGetSessionToken(ParseClient client)
{
    ParseClientInternal *parseClient = getInternalClient(client);

    if (strlen(parseClient->sessionToken) == 0) {
        return NULL;
    }

    return parseClient->sessionToken;
}

void parseSetPushCallback(ParseClient client, parsePushCallback callback)
{
    ParseClientInternal *parseClient = getInternalClient(client);

    parseClient->parsePushCallback = callback;

    localParseSetPushCallback(callback, (void *)parseClient);
}

int parseStartPushService(ParseClient client)
{
    ParseClientInternal *parseClient = getInternalClient(client);

    parseClient->isStartPushService = 1;

    return 0;
}

void parseStopPushService(ParseClient client)
{

}

int parseProcessNextPushNotification(ParseClient client)
{

    return 0;
}

void parseRunPushLoop(ParseClient client)
{
    ParseClientInternal *parseClient = getInternalClient(client);

    localParseEventLoop(parseClient);
}

void parseSendRequest(ParseClient client, const char *httpVerb, const char *httpPath, const char *httpRequestBody, parseRequestCallback callback) {
  getInstallation(getInternalClient(client));
  parseSendRequestInternal(client, httpVerb, httpPath, httpRequestBody, callback, 1);
  localWaitForRequestToFinish(client);
}

void parseSendRequestInternal(ParseClient client, const char *httpVerb, const char *httpPath, const char *httpRequestBody, parseRequestCallback callback, int addInstallHeader)
{
    ParseClientInternal *parseClient = getInternalClient(client);
    if( NULL == parseClient->applicationId[ 0 ] || NULL == parseClient->clientKey[ 0 ] )
    {
        // Error
        return;
    }

    parseClient->parseRequestCallback = callback;

    localParseSetRequestCallback(parseClient, callback, (void *)parseClient);

#ifdef CLIENT_DEBUG
    printf("[Parse] parseSendRequest: %s %s \"%s\".\r\n", httpVerb, httpPath, httpRequestBody ? httpRequestBody : "");
#endif /* CLIENT_DEBUG */

    localParseSendRequest(parseClient, httpVerb, httpPath, httpRequestBody, addInstallHeader);
}

int parseGetErrorCode(const char *httpResponseBody)
{
    // Simple implementation, just extract the 'code' value
    char code[10] = {0};
    if (simpleJsonProcessor(httpResponseBody, "code", code, 10) != 0) {
        return atoi(code);
    }

    return 2;
}

