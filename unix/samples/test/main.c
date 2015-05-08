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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <parse.h>
#include <time.h>

#include "json.h"

int totalTests = 0;
int successfullTests = 0;
int failedTests = 0;
unsigned long run;

void logSummary() {
    printf("Tests run:      %4i\n", totalTests);
    printf("Tests succeded: %4i\n", successfullTests);
    printf("Tests failed:   %4i\n", failedTests);
}

void logResults(int success, int abort, const char* test, const char* error) {
    totalTests++;
    if (success) {
        successfullTests++;
        printf("[SUCCESS] %s.\n", test);
    } else {
        failedTests++;
        printf("[FAILURE] %s: %s\n", test, error);
        if (abort) {
            logSummary();
            exit (-1);
        }
    }
}

int isHex(char c) {
    if (c >= 'a' && c <= 'z') return 1;
    if (c >= '0' && c <= '9') return 1;
    return 0;
}

int verifyInstallationId(const char* id) {
    int i;
    if (strlen(id) != 36) return 0;
    for (i = 0 ; i < 36; i++) {
        if (i == 8 || i == 13 || i == 18 || i == 23) {
            if (id[i] != '-') return 0;
        } else {
            if (!isHex(id[i])) return 0;
        }
    }

    return 1;
}

char* cachedRunResult = NULL;

static void clearCachedResults() {
    if (cachedRunResult) free (cachedRunResult);
    cachedRunResult = NULL;
}

static void callback(ParseClient client, int error, int httpStatus, const char* httpResponseBody) {
    if (error == 0) {
        cachedRunResult = strdup(httpResponseBody);
    }
}

int pushCounter = 0;

static void pushCallback(ParseClient client, int error, const char *buffer) {
    if (error == 0 && buffer != NULL) {
        char data[64] = {0};
        char id[16] = {0};
        char orig[16] = {0};
        snprintf(orig, sizeof(orig), "%ld", run);
        if (simpleJson(buffer, "data", data, sizeof(data))) {
            if (simpleJson(data, "id", id, sizeof(id))) {
                if (strncmp(id, orig, sizeof(id)) == 0) {
                    pushCounter++;
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {

    // CREATE UNIQUE IDENTIFIER FOR THE RUN

    run = time(NULL);
    char classPathOne[128] = {0};
    char classPathTwo[128] = {0};
    char classPathThree[128] = {0};
    char objectId[11] = {0};
    char path[256]  = {0};

    snprintf(classPathOne, sizeof(classPathOne), "/1/classes/TestObjectOne%ld", run);


    // TEST INITIALIZATION

    ParseClient client = parseInitialize("OVwxCTeK2dukeQ2UiO6N7D0LtqvxO5db7fKittB6",
                                         "zNFZ1lA8Kgv8qDl35RiLkNtMIKC9F2YRbOB1yxyj");
    logResults(client != NULL, 1, "parseInitialize call", "failed to start parse");


    // TEST GENERATION OF INSTALLATION ID

    const char* id = parseGetInstallationId(client);
    logResults(id == NULL, 0, "parseGetInstallationId call", "remove .parse-embedded from home directory");

    parseSendRequest(client, "GET", "/1/classes/testObjectFake/1111111", NULL, NULL);

    id = parseGetInstallationId(client);
    logResults(id != NULL, 1, "parseGetInstallationId call", "did not create the installation id properly");

    logResults(verifyInstallationId(id), 0, "installation id generated correctly", "installation id is not correct");

    // TEST CREATING AND FETCHING OBJECTS ON/FROM THE SERVER

    clearCachedResults(); 

    parseSendRequest(client, "POST", classPathOne, "{\"test\":\"object1\", \"value\":1}", callback);   
    logResults(cachedRunResult != NULL, 0, "create test object 1", "creating object failed");
    memset(objectId, 0, sizeof(objectId));
    logResults(simpleJson(cachedRunResult, "objectId", objectId, sizeof(objectId)), 0, "object 1 created", "could not create an object");

    clearCachedResults();

    memset(path, 0, sizeof(path));
    snprintf(path, sizeof(path), "%s/%s", classPathOne, objectId);
    parseSendRequest(client, "GET", classPathOne, NULL, callback);   
    logResults(cachedRunResult != NULL, 0, "fetch test object 1", "fetching object failed");

    clearCachedResults(); 

    parseSendRequest(client, "POST", classPathOne, "{\"test\":\"object1\", \"value\":2}", callback);   
    logResults(cachedRunResult != NULL, 0, "create test object 2", "creating object failed");
    memset(objectId, 0, sizeof(objectId));
    logResults(simpleJson(cachedRunResult, "objectId", objectId, sizeof(objectId)), 0, "object 2 created", "could not create an object");
    
    clearCachedResults();

    memset(path, 0, sizeof(path));
    snprintf(path, sizeof(path), "%s/%s", classPathOne, objectId);
    parseSendRequest(client, "GET", classPathOne, NULL, callback);   
    logResults(cachedRunResult != NULL, 0, "fetch test object 2", "fetching object failed");

    clearCachedResults(); 

    parseSendRequest(client, "POST", classPathOne, "{\"test\":\"object1\", \"value\":3}", callback);   
    logResults(cachedRunResult != NULL, 0, "create test object 3", "creating object failed");
    char objectIdKeepAround[11] = {0};
    logResults(simpleJson(cachedRunResult, "objectId", objectIdKeepAround, sizeof(objectIdKeepAround)), 0, "object 3 created", "could not create an object");

    clearCachedResults();

    memset(path, 0, sizeof(path));
    snprintf(path, sizeof(path), "%s/%s", classPathOne, objectIdKeepAround);
    parseSendRequest(client, "GET", classPathOne, NULL, callback);   
    logResults(cachedRunResult != NULL, 0, "fetch test object 3", "fetching object failed");

    clearCachedResults(); 

    parseSendRequest(client, "POST", classPathOne, "{\"test\":\"object1\", \"value\":2}", callback);   
    logResults(cachedRunResult != NULL, 0, "create test object 4", "creating object failed");
    memset(objectId, 0, sizeof(objectId));
    logResults(simpleJson(cachedRunResult, "objectId", objectId, sizeof(objectId)), 0, "object 4 created", "could not create an object");
    
    clearCachedResults();

    memset(path, 0, sizeof(path));
    snprintf(path, sizeof(path), "%s/%s", classPathOne, objectId);
    parseSendRequest(client, "GET", classPathOne, NULL, callback);   
    logResults(cachedRunResult != NULL, 0, "fetch test object 4", "fetching object failed");

    // TEST QUERIES

    clearCachedResults();

    parseSendRequest(client, "GET", classPathOne, "where={\"value\":2}", callback);   
    logResults(cachedRunResult != NULL, 0, "query objects", "querying objects failed");

    const char* results = "{\"results\":[{\"";
    int cmp = !strncmp(cachedRunResult, results, strlen(results));
    char* location1 = strstr(cachedRunResult, "objectId");
    char* location2 = strstr(location1 + 1, "objectId");
    char* location3 = strstr(location2 + 1, "objectId");
    logResults(cmp && location1 && location2 && !location3, 0, "query value", "query results not valid");


    // TEST OBJECT MODIFICATION

    clearCachedResults(); 

    memset(path, 0, sizeof(path));
    snprintf(path, sizeof(path), "%s/%s", classPathOne, objectIdKeepAround);

    parseSendRequest(client, "PUT", path, "{\"test\":\"object1\", \"value\":2}", callback);   
    logResults(cachedRunResult != NULL, 0, "modify test object 3", "modifying object failed");

    clearCachedResults();

    parseSendRequest(client, "GET", classPathOne, "where={\"value\":2}", callback);   
    logResults(cachedRunResult != NULL, 0, "query objects after modification", "querying objects failed");

    results = "{\"results\":[{\"";
    cmp = !strncmp(cachedRunResult, results, strlen(results));
    location1 = strstr(cachedRunResult, "objectId");
    location2 = strstr(location1 + 1, "objectId");
    location3 = strstr(location2 + 1, "objectId");
    logResults(cmp && location1 && location2 && location3, 0, "query value after modifying an object", "query results not valid");

    // TEST DELETING AN OBJECT

    clearCachedResults(); 

    memset(path, 0, sizeof(path));
    snprintf(path, sizeof(path), "%s/%s", classPathOne, objectIdKeepAround);

    parseSendRequest(client, "DELETE", path, NULL, callback);   
    logResults(cachedRunResult != NULL, 0, "delete test object 3", "deleting object failed");

    clearCachedResults();

    parseSendRequest(client, "GET", classPathOne, "where={\"value\":2}", callback);   
    logResults(cachedRunResult != NULL, 0, "query objects after delete", "querying objects failed");

    results = "{\"results\":[{\"";
    cmp = !strncmp(cachedRunResult, results, strlen(results));
    location1 = strstr(cachedRunResult, "objectId");
    location2 = strstr(location1 + 1, "objectId");
    location3 = strstr(location2 + 1, "objectId");
    logResults(cmp && location1 && location2 && !location3, 0, "query value after deleting an object", "query results not valid");

    // TEST PUSH

    parseSetPushCallback(client, pushCallback);
    parseStartPushService(client);
    int socket = parseGetPushSocket(client);

    int loopCount = 20;


    printf("[!!!!!!!] Run ./push.sh %ld\n", run);

    while(loopCount--) {
        struct timeval tv;
        fd_set receive, send, error;

        tv.tv_sec = 10;
        tv.tv_usec= 0;
        FD_ZERO(&receive);
        FD_ZERO(&send);
        FD_ZERO(&error);
        FD_SET(socket, &error);
        FD_SET(socket, &receive);
        select(socket + 1, &receive, &send, &error, &tv);

        parseProcessNextPushNotification(client);

        if (pushCounter == 10) loopCount = loopCount > 2 ? 2 : loopCount;
    }

    logResults(pushCounter == 10, 0, "receive push notifications", "did not receive the push notifications correctly");

    logSummary();
    return 0;
}
