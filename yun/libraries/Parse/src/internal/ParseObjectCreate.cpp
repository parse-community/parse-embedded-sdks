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
#include "ParseObjectCreate.h"

void ParseObjectCreate::addKey(const char* key) {
	if (isBodySet) {
		return;
	}
	if (!requestBody.equals("{")) {
		requestBody += ",";
	}
	requestBody += "\"";
	requestBody += key;
	requestBody += "\"";
	requestBody += ":";
}

void ParseObjectCreate::add(const char* key, int d) {
	addKey(key);
	requestBody += d;
}

void ParseObjectCreate::add(const char* key, double d) {
	addKey(key);
	requestBody += d;
}

void ParseObjectCreate::add(const char* key, bool b) {
	addKey(key);
	if(b) {
		requestBody += "true";
	} else {
		requestBody += "false";
	}
}

void ParseObjectCreate::add(const char* key, const char* s) {
	addKey(key);
	requestBody += "\"";
	requestBody += s;
	requestBody += "\"";
}

void ParseObjectCreate::addGeoPoint(const char* key, double lat, double lon) {
	addKey(key);
	String geoPoint = "{\"__type\":\"GeoPoint\",\"latitude\":";
	geoPoint += lat;
	geoPoint += ",\"longitude\":";
	geoPoint += lon;
	geoPoint += "}";
  requestBody += geoPoint;
}

void ParseObjectCreate::addJSONValue(const char* key, const char* json) {
	addKey(key);
	requestBody += json;
}

void ParseObjectCreate::addJSONValue(const char* key, const String& json) {
	addKey(key);
	requestBody += json;
}

void ParseObjectCreate::setJSONBody(const char* jsonBody) {
	requestBody = jsonBody;
	isBodySet = true;
}

void ParseObjectCreate::setJSONBody(const String& jsonBody) {
	requestBody = jsonBody;
	isBodySet = true;
}

ParseResponse ParseObjectCreate::send() {
	if (!isBodySet) {
		requestBody += "}";
	}
	return Parse.sendRequest("POST", httpPath, requestBody, "");
}
