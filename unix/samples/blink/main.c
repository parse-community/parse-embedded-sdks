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
#include <fcntl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <parse.h>

#include "gpio.h"
#include "json.h"

#define MAX_MSG_SIZE 128
#define MAX_TYPE_SIZE 20

#define LED_OFF 0
#define LED_ON 1

const char* ledStates[] = {"off", "on", "blink"};

#define STATE_OFF 0 
#define STATE_ON 1
#define STATE_BLINK 2

#define NSEC_IN_SECOND 1000000000
#define NSEC_IN_MICROSECOND 1000

#define PERIOD 1000000000

static char userObjectId[OBJECT_ID_MAX_LEN+1];
static char installationObjectId[OBJECT_ID_MAX_LEN+1];
static char modelObjectId[OBJECT_ID_MAX_LEN+1];

static int device_state = STATE_BLINK;

static void calculateNextOnAndOff(struct timespec* current,
                                  struct timespec* next_on,
                                  struct timespec* next_off) {
    next_on->tv_sec = current->tv_sec + ((PERIOD + current->tv_nsec) / NSEC_IN_SECOND);
    next_on->tv_nsec = (current->tv_nsec + PERIOD) % NSEC_IN_SECOND;
    next_off->tv_sec = current->tv_sec + ((PERIOD / 2 + current->tv_nsec) / NSEC_IN_SECOND);
    next_off->tv_nsec = (current->tv_nsec + PERIOD / 2) % NSEC_IN_SECOND;
}

static void advanceTimeByPeriod(struct timespec* next) {
    next->tv_sec = next->tv_sec + ((PERIOD + next->tv_nsec) / NSEC_IN_SECOND);
    next->tv_nsec = (next->tv_nsec + PERIOD) % NSEC_IN_SECOND;
}

static void myPushCallback(ParseClient client, int error, const char *buffer) {
    if (error == 0 && buffer != NULL) {
        char msg[MAX_MSG_SIZE];
        char msgType[MAX_TYPE_SIZE];

        simpleJson(buffer, "data", msg, MAX_MSG_SIZE);
        simpleJson(msg, "alert", msgType, MAX_TYPE_SIZE);

        if (strcmp("blink", msgType) == 0) {
            device_state = STATE_BLINK;
        } else if (strcmp("off", msgType) == 0) {
            device_state = STATE_OFF;
        } else if (strcmp("on", msgType) == 0) {
            device_state = STATE_ON;
        } else
        {
            printf("INFO: unknown state '%s'\n", msgType);
        }
    }
}

static void saveCurrentDeviceState(ParseClient client, int device_state) {
    char objectJson[1024];
    sprintf(objectJson, "{\"installationId\": \"%s\", \"value\": {\"state\": \"%s\"}, \"alarm\": true, \"ACL\":{ \"%s\": { \"read\": true, \"write\": true}}}",
            installationObjectId, ledStates[device_state], userObjectId);
    parseSendRequest(client, "POST", "/1/classes/Event", objectJson, NULL);
}

static void blinkGetUserObjectIdCallback(ParseClient client, int error, int httpStatus, const char* httpResponseBody)
{
    userObjectId[0] = '\0';
    if ((error == 0) && (httpStatus >= 200 && httpStatus < 300))
    {
         simpleJson(httpResponseBody, "objectId", userObjectId, OBJECT_ID_MAX_LEN);
    }
}

static void blinkGetInstallationObjectIdByIdCallback(ParseClient client, int error, int httpStatus, const char* httpResponseBody)
{
    installationObjectId[0] = '\0';
    if ((error == 0) && (httpStatus >= 200 && httpStatus < 300))
    {
        char queryResults[1024] = {0};
        short status = simpleJson(httpResponseBody, "results", queryResults, sizeof(queryResults));
        if ((status != 0) && (queryResults[1] == '{'))
        {
            simpleJson(queryResults+1, "objectId", installationObjectId, sizeof(installationObjectId));
        }
    }
}

static void blinkGetInstallationObjectId(ParseClient client)
{
    if (installationObjectId[0] == '\0') {
        char query[60];
        sprintf(query, "where={\"installationId\":\"%s\"}", parseGetInstallationId(client));
        parseSendRequest(client, "GET", "/1/installations", query, blinkGetInstallationObjectIdByIdCallback);
    }
}

static void blinkGetModelObjectIdByNameCallback(ParseClient client, int error, int httpStatus, const char* httpResponseBody)
{
    modelObjectId[0] = '\0';
    if ((error == 0) && (httpStatus >= 200 && httpStatus < 300))
    {
        char queryResults[1024] = {0};
        short status = simpleJson(httpResponseBody, "results", queryResults, sizeof(queryResults));
        if ((status != 0) && (queryResults[1] == '{'))
        {
            simpleJson(queryResults+1, "objectId", modelObjectId, sizeof(modelObjectId));
        }
    }
}

static void blinkGetUserObjectId(ParseClient client)
{
    if (userObjectId[0] == '\0') {
        parseSendRequest(client, "GET", "/1/users/me", NULL, blinkGetUserObjectIdCallback);
    }
}

static void blinkGetModelObjectId(ParseClient client)
{
    if(modelObjectId[0] == '\0') {
        parseSendRequest(client, "GET", "/1/classes/Model", "where={\"appName\":\"fbdr000001a\"}", blinkGetModelObjectIdByNameCallback);
    }
}


static void blinkUpdateInstallation(ParseClient client)
{
    printf("Bravo!");
    blinkGetInstallationObjectId(client);

    if (strlen(installationObjectId) > 0)
    {
        blinkGetModelObjectId(client);
        blinkGetUserObjectId(client);

        char path[40] = {0};
        char objectJson[1024] = {0};
        sprintf(path, "/1/installations/%s", installationObjectId);
        sprintf(objectJson, "{\"deviceName\": \"%s\", \"deviceSubtype\": \"fluffy\", \"model\": {\"__type\":\"Pointer\",\"className\":\"Model\",\"objectId\":\"%s\"}, \"owner\": {\"__type\":\"Pointer\",\"className\":\"_User\",\"objectId\":\"%s\"}}",
          "Raspberry PI blink", modelObjectId, userObjectId);
        parseSendRequest(client, "PUT", path, objectJson, NULL);
    }
}


int main(int argc, char *argv[]) {

    ParseClient client = parseInitialize(YOUR_PARSE_APP_ID, YOUR_PARSE_CLIENT_KEY);

    if (parseGetInstallationId(client) == NULL) {
        // we need installation id and session token. If we never set them,
        // call to parseSetSessionToken will also generate installation id.
        parseSetSessionToken(client, YOUR_RESTRICTED_SESSION_TOKEN);
    }

    blinkUpdateInstallation(client);

    parseSetPushCallback(client, myPushCallback);
    int e = parseStartPushService(client);

    int socket = parseGetPushSocket(client);
    if (socket < 0) {
       printf("socket is: %i\n", socket);
       return socket;
    }

    int led_fd = gpioEnable(24, "out");
    int irq_fd = gpioEnable(25, "in");

    if (irq_fd > -1) {
        gpioSetEdge(25, "both");
    }

    struct timespec next_time_on;
    struct timespec next_time_off;
    struct timespec current;

    clock_gettime(CLOCK_MONOTONIC, &current);
    calculateNextOnAndOff(&current, &next_time_on, & next_time_off);

    int led_state = 0;
    int last_value = 0;

    saveCurrentDeviceState(client, device_state);

    while(1) {

        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        clock_gettime(CLOCK_MONOTONIC, &current);
        printf("=========================================\n");
        printf("current = (%2d, %09d)\n", current.tv_sec, current.tv_nsec);

        if (device_state == STATE_BLINK) {

            int next_state = 0;
            struct timespec next;

            if (next_time_off.tv_sec > next_time_on.tv_sec) {
                // on is next
                next_state = 1;
                next.tv_sec = next_time_on.tv_sec;
                next.tv_nsec = next_time_on.tv_nsec;
            } else if (next_time_off.tv_sec < next_time_on.tv_sec) {
                // off is next
                next_state = 0;
                next.tv_sec = next_time_off.tv_sec;
                next.tv_nsec = next_time_off.tv_nsec;
            } else {
                next.tv_sec = next_time_on.tv_sec;
                if (next_time_off.tv_nsec > next_time_on.tv_nsec) {
                    // on is next
                    next_state = 1;
                    next.tv_nsec = next_time_on.tv_nsec;
                } else {
                    // off is next
                    next_state = 0;
                    next.tv_nsec = next_time_off.tv_nsec;
                }
            }

            if (current.tv_sec > next.tv_sec || current.tv_sec == next.tv_sec && current.tv_nsec > next.tv_nsec) {
                if (next_state == 1) {
                    led_state = 1;
                    advanceTimeByPeriod(&next_time_on);
                } else {
                    led_state = 0;
                    advanceTimeByPeriod(&next_time_off);
                }
                tv.tv_sec = 0;
                tv.tv_usec = 0;
            } else {
                // next change in

                struct timespec diff;
                diff.tv_nsec = next.tv_nsec - current.tv_nsec;
                diff.tv_sec = next.tv_sec - current.tv_sec;
                if (diff.tv_nsec < 0 ) {
                    diff.tv_nsec += NSEC_IN_SECOND;
                    diff.tv_sec -= 1;
                }
                if (diff.tv_sec < 0) {
                    diff.tv_sec = 0;
                }

                tv.tv_sec = diff.tv_sec;
                tv.tv_usec = diff.tv_nsec / NSEC_IN_MICROSECOND;
            }
        } else if (device_state == STATE_OFF) {
            led_state = LED_OFF;
        } else if (device_state == STATE_ON) {
            led_state = LED_ON;
        }

        if (led_state == LED_OFF) {
            write(led_fd, "0\n", 2);
        } else {
            write(led_fd, "1\n", 2);
        }

        if (tv.tv_sec > 0 || tv.tv_usec > 0) {
            fd_set receive, send, error;

            FD_ZERO(&receive);
            FD_ZERO(&send);
            FD_ZERO(&error);

            FD_SET(socket, &error);
            FD_SET(socket, &receive);
            FD_SET(irq_fd, &error);

            int max = socket > irq_fd ? socket : irq_fd;

            int ret = select(max + 1, &receive, &send, &error, &tv);
            if (ret < 0) {
                printf("ERROR: select returned '%s'\n", strerror(errno));
            }

            if (FD_ISSET(socket, &error)) {
                printf("ERROR: on push socket\n");
                break;
            }

            int old_device_state = device_state;
            int maxMessagesProcessed = 3;

            while(parseProcessNextPushNotification(client) && maxMessagesProcessed--) ;

            if (FD_ISSET(irq_fd, &error)) {
                char value[16] = {};
                int r = gpioReadValue(irq_fd, value, sizeof(value));
                if (r > 0) {
                    if (last_value == 0 && value[0] == '1') {
                        device_state = (device_state + 1) % 3;
                    }
                    if (device_state == STATE_BLINK) {
                        clock_gettime(CLOCK_MONOTONIC, &current);
                        calculateNextOnAndOff(&current, &next_time_on, & next_time_off);
                        led_state = LED_ON;
                    }
                    if (value[0] == '0') last_value = 0;
                    if (value[0] == '1') last_value = 1;
                } else {
                    printf("ERROR: failed to read gpio\n");
                    break;
                }
            }

            if (device_state == STATE_BLINK && old_device_state != STATE_BLINK) {
                clock_gettime(CLOCK_MONOTONIC, &current);
                calculateNextOnAndOff(&current, &next_time_on, & next_time_off);
                led_state = LED_ON;
            }
            if (device_state != old_device_state) {
                saveCurrentDeviceState(client, device_state);
            }
        }
    }

    return 0;
}
