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

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <parse.h>

#include "config.h"
#include "utils.h"
#include "yun.h"

#define PARSE_BUFSIZE 2048

typedef struct PushNode {
    char* push_data;
    struct PushNode* next;
} ParsePush;

typedef ParsePush* ParsePushPtr;

int commandLine = 0;

ParsePushPtr head = NULL;
ParsePushPtr tail = NULL;
char pushBuf[PARSE_BUFSIZE];

void enqueuePush(const char* push) {
    const char* next = push;
    int nextSize = strlen(push);
  
    // create new ParsePush
    tail->next = malloc(sizeof(ParsePush));
    tail = tail->next;

    // fill in push data
    tail->push_data = malloc(nextSize+1);
    memset(tail->push_data, 0, nextSize+1);
    strncpy(tail->push_data, next, nextSize);
    tail->next = NULL;
}

void newPushHandler(ParseClient client, int osError, const char* buffer) {
    // extract push data
    memset(pushBuf, 0, PARSE_BUFSIZE);
    getValueFromJSON(buffer, "data", pushBuf, PARSE_BUFSIZE);

    if (commandLine) {
        if (buffer != NULL) {
            fprintf(stdout, "%s\n", pushBuf);
            fflush(stdout);
        }
    } else {
        enqueuePush(pushBuf);
    }
}

void arduinoPushFetchHandler() {
    if (commandLine) return;

    char c[10] = {0};
    int l = read(STDIN_FILENO, c, sizeof(c));
    if (c[0] == 'n') { // nextPush() called in Arduino
        if(head->next) {
            fprintf(stdout, "%s", head->next->push_data);
            fflush(stdout);

            ParsePushPtr tmp = head->next;
            head->next = head->next->next;
            if (!head->next) {
                tail = head;
            }

            free(tmp->push_data);
            free(tmp);
        }
    }
}

void displayHelp() {
    fprintf(stderr,
            "parse_push, version %s\n"
            "Usage: parse_push [option] ...\n"
            "\n"
            "  -a - set application id\n"
            "  -k - set client key\n"
            "  -x - set installation id\n"
            "  -y - set session token\n"
            "  -c - deliver messeges as they come in\n"
            "\n", VERSION);
}

int main(int argc , char **argv) {

    char c;

    yunReadProvisioningInfo();

    while((c=getopt(argc,argv,":a:k:x:y:hc"))!=-1){
        switch(c) {
            case 'h':
                displayHelp();
                exit(0);
            case 'a':
                strncpy(g_cAppID, optarg, sizeof(g_cAppID));
                break;
            case 'k':
                strncpy(g_cClientKey, optarg, sizeof(g_cClientKey));
                break;
            case 'x':
                strncpy(g_cInstallationID, optarg, sizeof(g_cInstallationID));
                break;
            case 'y':
                strncpy(g_cSessionToken, optarg, sizeof(g_cSessionToken));
                break;
            case 'c':
                commandLine = 1;
                break;
            case ':':
                fprintf(stderr, "%s: option '-%c' requires an argument\n", argv[0], optopt);
                exit(0);
            case '?':
                fprintf(stderr, "%s: option '-%c' is not recognized \n", argv[0], optopt);
                exit(0);
            default:
                fprintf(stderr, "%s: option '-%c' is not implemented\n", argv[0], optopt);
                exit(0);
        }
    }

    if (g_cAppID[0] == '\0') {
        fprintf(stdout, "{\"error\":\"missing application id\"}\n");
        exit(0);
    }
    if (g_cClientKey[0] == '\0') {
        fprintf(stdout, "{\"error\":\"missing client key\"}\n");
        exit(0);
    }

    ParseClient client = parseInitialize(g_cAppID, g_cClientKey);

    if (client) {
        // initialize push buffer queue
        ParsePush queue;
        queue.push_data = NULL;
        queue.next = NULL;
        head = &queue; // head's next will be first push
        tail = &queue; // tail is last push
    
        parseSetPushCallback(client, newPushHandler);
        if (commandLine) {
            if (parseStartPushService(client) == 0) {
                fprintf(stderr, "connected to parse\n");
            } else {
                fprintf(stderr, "failed to connect to parse\n");
                exit(0);
            }
        } else {
            if (parseStartPushService(client) == 0) {
                fprintf(stdout, "s\n"); // notify Arduino success
            } else {
                fprintf(stdout, "f\n"); // notify Arduino failure
            }
        }
        fflush(stdout);

        while(1) {
            int socket = parseGetPushSocket(client);
            struct timeval tv;
            fd_set receive, send, error;

            tv.tv_sec = 1;
            tv.tv_usec= 0;

            FD_ZERO(&receive);
            FD_ZERO(&send);
            FD_ZERO(&error);

            FD_SET(socket, &error);
            FD_SET(socket, &receive);
            FD_SET(STDIN_FILENO, &receive);

            if (select(socket + 1, &receive, &send, &error, &tv) > 0) {
                if (FD_ISSET(socket, &error)) {
                    parseStopPushService(client);
                    fprintf(stderr, "error on the socket\n");
                    break;
                }
                if(FD_ISSET(STDIN_FILENO, &receive)) {
                    arduinoPushFetchHandler();
                }
            }
            parseProcessNextPushNotification(client);
        }
    }
    exit(0);
}

