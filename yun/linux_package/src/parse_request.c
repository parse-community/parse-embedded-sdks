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

#include "yun.h"

#define PARSE_BUFSIZE 2048

char* verb = "GET";
char* endpoint = "/";
char* data = "";
char* params = NULL;

void arduinoCallHandler(ParseClient client, int error, int httpStatus, const char* httpResponseBody) {
  if (error != 0 || httpResponseBody == NULL) { return; }
  // print to process stdout, so Arduino can process it
  fprintf(stdout, "%s", httpResponseBody);
  fflush(stdout);
}

void arduinoQueryHandler(ParseClient client, int error, int httpStatus, const char* httpResponseBody) {
  if (error != 0 || httpResponseBody == NULL) { return; }

  // get query results
  char results[PARSE_BUFSIZE];
  getValueFromJSON(httpResponseBody, "results", results, PARSE_BUFSIZE);

  char key[32];
  int count = getCountFromJSONArray(results);

  // handle commands from Arduino
  char ch;
  char* next = results;
  int nextSize = 0;
  while(1){
    ch = fgetc(stdin); //read(stdin, &ch, 1);
    if(ch == 'n') { // nextObject() called in Arduino
      next = getNextFromJSONArray(next, next + nextSize, &nextSize);
      if (nextSize > 0 && next) {
        char* object = malloc(sizeof(char) * (nextSize+1));
        snprintf(object, nextSize+1, "%s", next);
        fprintf(stdout, "%s", object);
        fflush(stdout);
      } else { // no more object, end
        return;
      }
    } else if (ch == 'c') { // count() called in Arduino
      fprintf(stdout, "%d", count);
      fflush(stdout);
    } else if (ch == 'e') {
      return; // end
    }
  }
}

void arduinoGetInstallationId(ParseClient client) {
  const char* iid = parseGetInstallationId(client);
  if (iid) {
    fprintf(stdout, "%s", iid);
  } else {
    fprintf(stdout, "\n");
  }
  fflush(stdout);
}

void arduinoGetSessionToken(ParseClient client) {
  const char* st = parseGetSessionToken(client);
  if (st) {
    fprintf(stdout, "%s", st);
  } else {
    fprintf(stdout, "\n");
  } 
  fflush(stdout);
}

/*
* main function to handle Parse GET/CREATE/DELETE/UPDATE/QUERY request
* @params
* -v - http verb
* -e - http endpoint
* -d - request body
* -p - parameters (used only for Parse query)
* -i - command to get installation id
*/
int main(int argc , char **argv) {
  char c;
  extern char* optarg;

  yunReadProvisioningInfo();

  ParseClient client = parseInitialize(g_cAppID,g_cClientKey);

  if(g_cInstallationID[0] != '\0') {
	parseSetInstallationId(client, g_cInstallationID);
  }
  if(g_cSessionToken[0] != '\0') {
	parseSetSessionToken(client, g_cSessionToken);
  }

  while((c=getopt(argc,argv,"v:e:d:p:is"))!=-1){
    switch(c){
      case 'v': // http verb
      if(!(verb=strdup(optarg))){
        return -1;
      }
      break;
      case 'e': // http endpoint
      if(!(endpoint=strdup(optarg))){
        return -1;
      }
      break;
      case 'd': // request body
      if(!(data=strdup(optarg))){
      }
      fprintf(stderr, "data = '%s'\n", data);
      break;
      case 'p': // parameters
      if(!(params=strdup(optarg))){
      }
      break;
      case 'i':
      arduinoGetInstallationId(client);
      exit(0);
      case 's':
      arduinoGetSessionToken(client);
      exit(0);
    }
  }

  if (strncmp("GET", verb, 3) == 0) {
    data = params;
  }

  if (client) {
    if (!params) {
      parseSendRequest(client, verb, endpoint, data, arduinoCallHandler);
    } else {
      parseSendRequest(client, verb, endpoint, params, arduinoQueryHandler);
    }
  }

  exit(0);
}

