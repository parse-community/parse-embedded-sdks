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

#include "ParseClient.h"

ParseClient::ParseClient() {
}

void ParseClient::begin(const char *applicationId, const char *clientKey) {
	Console.println("begin");

	// send the info to linux through Bridge
	if(applicationId) {
		Bridge.put("appId", applicationId);
	}
	if (clientKey) {
		Bridge.put("clientKey", clientKey);
	}
}

void ParseClient::setInstallationId(const char *installationId) {
	this->installationId = installationId;
	if (installationId) {
		Bridge.put("installationId", installationId);
	} else {
		Bridge.put("installationId", "");
	}
}

const char* ParseClient::getInstallationId() {
	if (!this->installationId) {
		char *buf = new char[37];

		requestClient.begin("parse_request");
		requestClient.addParameter("-i");
		requestClient.run();
    read(&requestClient, buf, 37);
		this->installationId = buf;
	}
	return this->installationId;
}

void ParseClient::setSessionToken(const char *sessionToken) {
	if ((sessionToken != NULL) && (strlen(sessionToken) > 0 )){
		this->sessionToken = sessionToken;
		Bridge.put("sessionToken", sessionToken);
	} else {
	  Bridge.put("sessionToken", "");
	}
}

void ParseClient::clearSessionToken() {
	setSessionToken(NULL);
}

const char* ParseClient::getSessionToken() {
	if (!this->sessionToken) {
		char *buf = new char[33];

		requestClient.begin("parse_request");
		requestClient.addParameter("-s");
		requestClient.run();
		read(&requestClient, buf, 33);
		this->sessionToken = buf;
	}
	return this->sessionToken;
}

ParseResponse ParseClient::sendRequest(const char* httpVerb, const char* httpPath, const char* requestBody, const char* urlParams) {
  return sendRequest(String(httpVerb), String(httpPath), String(requestBody), String(urlParams));
}

ParseResponse ParseClient::sendRequest(const String& httpVerb, const String& httpPath, const String& requestBody, const String& urlParams) {
	requestClient.begin("parse_request");  // start a process that launch the "parse_request" command

	requestClient.addParameter("-v");
	requestClient.addParameter(httpVerb);
	requestClient.addParameter("-e");
	requestClient.addParameter(httpPath);
	if (requestBody != "") {
		requestClient.addParameter("-d");
		requestClient.addParameter(requestBody);
	}
	if (urlParams != "") {
		requestClient.addParameter("-p");
		requestClient.addParameter(urlParams);
    requestClient.runAsynchronously();
	} else {
	  requestClient.run(); // Run the process and wait for its termination
	}

	ParseResponse response(&requestClient);
	return response;
}

bool ParseClient::startPushService() {
	pushClient.begin("parse_push");  // start a process that launch the "parse_request" command
	pushClient.runAsynchronously();

	while(1) {
		if(pushClient.available()) {
			// read the push starting result
			char c = pushClient.read();
			while(pushClient.available()) {
				pushClient.read();
			}
			if (c == 's') {
                 pushClient.write('n'); // send a signal that ready to consume next available push
				return true;
			} else {
				return false;
			}
		}
	}
}

ParsePush ParseClient::nextPush() {
	ParsePush push(&pushClient);
	return push;
}

bool ParseClient::pushAvailable() {
    pushClient.write('n'); // send a signal that ready to consume next available push
    if(pushClient.available()) {
        return 1;
    } else {
        return 0;
    }
}

void ParseClient::stopPushService() {
	pushClient.close();
}

void ParseClient::end() {
	stopPushService();
}

void ParseClient::read(Process* client, char* buf, int len) {
  memset(buf, 0, len);
  int p = 0;
	while(1) {
		if(client->available()) {
			while (p < (len-1) && client->available()) {
				buf[p++] = client->read();
			}
			break;
		}
	}
}

ParseClient Parse;
