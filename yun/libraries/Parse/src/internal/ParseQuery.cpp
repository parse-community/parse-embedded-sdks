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
#include "ParseQuery.h"
#include "ParseUtils.h"

ParseQuery::ParseQuery() : ParseRequest() {
	whereClause = "";
	limit = -1;
	skip = -1;
	order = "";
	returnedFields = "";
}

void ParseQuery::addConditionKey(const char* key) {
	if (whereClause == "") {
		whereClause += "{";
	} else {
		whereClause += ",";
	}

	whereClause += "\"";
	whereClause += key;
	whereClause += "\"";
	whereClause += ":";
}

void ParseQuery::addConditionNum(const char* key, const char* comparator, double v) {
	addConditionKey(key);

	if (comparator == "$=") {
		whereClause += v;
	} else {
		whereClause += "{\"";
		whereClause += comparator;
		whereClause += "\":";
		whereClause += v;
		whereClause += "}";
	}
}

void ParseQuery::whereExists(const char* key) {
	addConditionKey(key);
	whereClause += "{\"exists\":true}";
}

void ParseQuery::whereDoesNotExist(const char* key) {
	addConditionKey(key);
	whereClause += "{\"exists\":false}";
}

void ParseQuery::whereEqualTo(const char* key, const char* v) {
	addConditionKey(key);
	whereClause += "\"";
	whereClause += v;
	whereClause += "\"";
}

void ParseQuery::whereNotEqualTo(const char* key, const char* v) {
	addConditionKey(key);
	whereClause += "{\"$ne\":\"";
	whereClause += v;
	whereClause += "\"}";
}

void ParseQuery::whereEqualTo(const char* key, bool v) {
	addConditionKey(key);
	whereClause += v?"true":"false";
}

void ParseQuery::whereNotEqualTo(const char* key, bool v) {
	addConditionKey(key);
	whereClause += "{\"$ne\":";
	whereClause += v?"true":"false";
	whereClause += "}";
}

void ParseQuery::whereEqualTo(const char* key, int v) {
	addConditionNum(key, "$=", v);
}

void ParseQuery::whereNotEqualTo(const char* key, int v) {
	addConditionNum(key, "$ne", v);
}

void ParseQuery::whereLessThan(const char* key, int v) {
	addConditionNum(key, "$lt", v);
}

void ParseQuery::whereGreaterThan(const char* key, int v) {
	addConditionNum(key, "$gt", v);
}

void ParseQuery::whereLessThanOrEqualTo(const char* key, int v) {
	addConditionNum(key, "$lte", v);
}

void ParseQuery::whereGreaterThanOrEqualTo(const char* key, int v) {
	addConditionNum(key, "$gte", v);
}

void ParseQuery::whereEqualTo(const char* key, double v) {
	addConditionNum(key, "$=", v);
}

void ParseQuery::whereNotEqualTo(const char* key, double v) {
	addConditionNum(key, "$ne", v);
}

void ParseQuery::whereLessThan(const char* key, double v) {
	addConditionNum(key, "$lt", v);
}

void ParseQuery::whereGreaterThan(const char* key, double v) {
	addConditionNum(key, "$gt", v);
}

void ParseQuery::whereLessThanOrEqualTo(const char* key, double v) {
	addConditionNum(key, "$lte", v);
}

void ParseQuery::whereGreaterThanOrEqualTo(const char* key, double v) {
	addConditionNum(key, "$gte", v);
}

void ParseQuery::setLimit(int n) {
	limit = n;
}

void ParseQuery::setSkip(int n) {
	skip = n;
}

void ParseQuery::orderBy(const char* key) {
	order = key;
}

void ParseQuery::setKeys(const char* keys) {
	returnedFields = keys;
}

ParseResponse ParseQuery::send() {
  String urlParameters = "";
  if (whereClause != "") {
    whereClause += "}"; // close where clause if there is any
    urlParameters += "where=";
    urlParameters += whereClause;
  } 
  
	if (limit>0) {
		urlParameters += "&limit=";
		urlParameters += limit;
	}
	if (skip>0) {
		urlParameters += "&skip=";
		urlParameters += skip;
	}
	if (order != "") {
		urlParameters += "&order=";
		urlParameters += order;
	}
	if (returnedFields != "") {
		urlParameters += "&keys=";
		urlParameters += returnedFields;
	}
	return Parse.sendRequest("GET", httpPath, "", urlParameters);
}

