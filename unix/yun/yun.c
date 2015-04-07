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
#include <parse.h>

#include "yun.h"

char g_cAppID[APPLICATION_ID_MAX_LEN + 1];
char g_cClientKey[CLIENT_KEY_MAX_LEN + 1];
char g_cInstallationID[INSTALLATION_ID_MAX_LEN + 1];
char g_cSessionToken[SESSION_TOKEN_MAX_LEN + 1];

void yunReadProvisioningInfo() {
  int socketHandle = -1;
  char buf[128];
  char value[64];
  memset(value, 0, 64);
  if ((socketHandle = tcp_connect("127.0.0.1", 5700)) > 0) {
    // read appId
    memset(buf, 0, 128);
    tcp_write(socketHandle, "{\"command\": \"get\", \"key\": \"appId\"}\n");
    if (tcp_read(socketHandle, buf, 128, 2)) {
      getValueFromJSON(buf, "value", value, 64);
      if(strcmp("null", value) != 0) {
        strncpy(g_cAppID, value, sizeof(g_cAppID));
      }
    }
    
    // read clientKey
    memset(buf, 0, 128);
    tcp_write(socketHandle, "{\"command\": \"get\", \"key\": \"clientKey\"}\n");
    if (tcp_read(socketHandle, buf, 128, 2)) {
      getValueFromJSON(buf, "value", value, 64);
      if(strcmp("null", value) != 0) {
        strncpy(g_cClientKey, value, sizeof(g_cClientKey));
      }
    }
    
    // read installationId
    memset(buf, 0, 128);
    tcp_write(socketHandle, "{\"command\": \"get\", \"key\": \"installationId\"}\n");
    if (tcp_read(socketHandle, buf, 128, 2)) {
      getValueFromJSON(buf, "value", value, 64);
      if(strcmp("null", value) != 0) {
        strncpy(g_cInstallationID, value, sizeof(g_cInstallationID));
      }
    }
    
    // read sessionToken
    memset(buf, 0, 128);
    tcp_write(socketHandle, "{\"command\": \"get\", \"key\": \"sessionToken\"}\n");
    if (tcp_read(socketHandle, buf, 128, 2)) {
      getValueFromJSON(buf, "value", value, 64);
      if(strcmp("null", value) != 0) {
        strncpy(g_cSessionToken, value, sizeof(g_cSessionToken));
      }
    }
		
    tcp_close(socketHandle);
  }
}

