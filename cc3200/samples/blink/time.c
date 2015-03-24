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

#include "blink.h"

#define SERVER_RESPONSE_TIMEOUT 10

#define NTP_UNIX_EPOCH_DIFF 2208988800UL

static const char ntpServer[] = "time-a.nist.gov";

short setBoardTimeFromNtp() {
    UART_PRINT("[Blink] Set board time from NTP\r\n");
    short socketHandle = -1;

    unsigned long destinationIp = 0;
    short status = gethostbyname((signed char *)ntpServer, strlen(ntpServer), &destinationIp, AF_INET);

    if (status >= 0) {
        status = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    }

    if (status >= 0) {
        socketHandle = status;

        struct timeval timeVal;
        timeVal.tv_sec = SERVER_RESPONSE_TIMEOUT;
        timeVal.tv_usec = 0;
        setsockopt(socketHandle, SOL_SOCKET, SO_RCVTIMEO, (unsigned char*)&timeVal, sizeof(timeVal));

        int sockAddrInLen = sizeof(sockaddr_in);

        // Send NTP query to get the time
        sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(123);
        server.sin_addr.s_addr = htonl(destinationIp);

        char ntpData[48];
        memset(ntpData, 0, sizeof(ntpData));
        ntpData[0] = 0x1b;
        ntpData[1] = 0;
        ntpData[2] = 4;
        ntpData[3] = 0xfa;

        status = sendto(socketHandle, ntpData, sizeof(ntpData), 0, (sockaddr *)&server, sockAddrInLen);
        if (status >= 0) {
            memset(ntpData, 0, sizeof(ntpData));

            // Wait to receive the NTP time from the server
            sockaddr_in client;
            client.sin_family = AF_INET;
            client.sin_port = 0;
            client.sin_addr.s_addr = 0;

            status = bind(socketHandle, (sockaddr *)&client, sockAddrInLen);
            if (status < 0) {
                UART_PRINT("[Blink] Bind: %s\r\n", status);
            }

            while ((status == EAGAIN) || (status == 0)) {
                status = recvfrom(socketHandle, (void *)ntpData, sizeof(ntpData), 0, (sockaddr *)&client, (socklen_t *)&sockAddrInLen);
            }

            if (status > 0) {
                // Getting the data from the Transmit Timestamp (seconds) field
                // This is the time at which the reply departed the server for the client
                // We could calculate the network latency by sending another request
                // and using the diff to approximate the time of flight there and back
                unsigned long ntpTime = 0;
                memcpy(&ntpTime, ntpData+40, sizeof(ntpTime));

                time_t unixTime = (time_t)(ntohl(ntpTime) - NTP_UNIX_EPOCH_DIFF);
                UART_PRINT("[Blink] New time                  : %s\r\n", ctime((const time_t *)&unixTime));

                struct tm *calendarTime = localtime((const time_t *)&unixTime);
                SlDateTime_t dateTime;
                memcpy(&dateTime, calendarTime, sizeof(dateTime));
                dateTime.sl_tm_year += 1970;
                dateTime.sl_tm_mon += 1;
                setBoardTimeDate(&dateTime);
            }
        }
    }

    if (status < 0) {
        UART_PRINT("[Blink] NTP error                 : %d\r\n", status);
    }

    close(socketHandle);

    return status;
}
