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

#include <simplelink.h>
#include <device.h>

#include <stdlib.h>
#include <stdio.h>

#include "parse_impl.h"

char g_osVersion[OS_VERSION_MAX_LEN+1] = {0};

static SlDateTime_t lastDateTime = {0};

static void getDeviceTimeDate(SlDateTime_t *dateTime) {
    unsigned char configOpt = SL_DEVICE_GENERAL_CONFIGURATION_DATE_TIME;
    unsigned char configLen = sizeof(SlDateTime_t);
    memset(dateTime, 0, configLen);
    sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &configOpt, &configLen, (unsigned char *)dateTime);
}

static int timeDiffInSec(SlDateTime_t dt1, SlDateTime_t dt2) {
    unsigned long result = 0;

    long temp = dt2.sl_tm_sec - dt1.sl_tm_sec;
    result += (temp < 0) ? (60 + temp) :  (temp);
    temp = dt2.sl_tm_min - dt1.sl_tm_min - (temp < 0) ? 1 : 0;
    result += (temp < 0) ? (60 + temp) :  (temp);
    temp = dt2.sl_tm_hour - dt1.sl_tm_hour - (temp < 0) ? 1 : 0;
    result += (temp < 0) ? (24 + temp) :  (temp);

    return result;
}

int isPingTime() {
    SlDateTime_t dateTime = {0};
    getDeviceTimeDate(&dateTime);

    return (timeDiffInSec(dateTime, lastDateTime) > PUSH_KEEP_ALIVE) ? TRUE : FALSE;
}

void updateLastPingTime() {
    getDeviceTimeDate(&lastDateTime);
}

void fetchDeviceOSVersion() {
    int status = -1;

    unsigned char configOpt = SL_DEVICE_GENERAL_VERSION;
    unsigned char configLen = sizeof(SlVersionFull);
    SlVersionFull ver;
    memset(&ver, 0, configLen);

    status = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &configOpt, &configLen, (unsigned char *)(&ver));

    if(status >= 0) {
        snprintf(g_osVersion, sizeof(g_osVersion)-1, "%lu.%lu.%lu.%lu", ver.NwpVersion[0], ver.NwpVersion[1], ver.NwpVersion[2], ver.NwpVersion[3]);
    }
}

/*
 * !!! IMPORTANT !!!
 * This function does not conform to RFC 4122, even though it returns
 * a string formatted as an UUID. Do not use this to generate proper UUID!
 */
void createNewInstallationId(ParseClientInternal *parseClient) {
    static int randInitialized = FALSE;

    if (!randInitialized) {
        SlDateTime_t dateTime;
        getDeviceTimeDate(&dateTime);
        // Terrible choice for initialization, prone to collision
        srand(dateTime.sl_tm_sec);
        randInitialized = TRUE;
    }

    snprintf(parseClient->installationId,
    		 sizeof(parseClient->installationId)-1,
    		 "%x%x%x%x%x%x%x%x-%x%x%x%x-%x%x%x%x-%x%x%x%x-%x%x%x%x%x%x%x%x%x%x%x%x",
             rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16,
             rand()%16, rand()%16, rand()%16, rand()%16,
             rand()%16, rand()%16, rand()%16, rand()%16,
             rand()%16, rand()%16, rand()%16, rand()%16,
             rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16
            );
}
