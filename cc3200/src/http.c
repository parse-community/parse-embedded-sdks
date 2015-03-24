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

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "parse_impl.h"

/**
 * Simple implementation of HTTP response status and body parsing
 */

// We always request HTTP 1.1, so Parse should always response with 'HTTP/1.1 '
// Sadly, this can be broken in the rare case of an intermediate proxy which
// doesn't understand HTTP 1.1.
#define HTTP_HEADERS_BEGIN "HTTP/1.1 "
static short magicOffset = 9;

// As per HTTP RFC, HTTP headers finish with two CR LF in a row
// Order matters, and is dfefined in the RFC
#define HTTP_HEADERS_END "\r\n\r\n"
static short magicOffset2 = 4;

// HTTP Status codes are always three digits
static short magicOffset3 = 3;

int addHttpRequestLine(char *httpRequest, unsigned int httpRequestSize, const char *format, ...) {
    if (httpRequest == NULL) {
        return -1;
    }

    va_list list;
    va_start(list, format);
    int status = vsnprintf(httpRequest, httpRequestSize, (char *)format, list);
    va_end(list);

    if (status >= httpRequestSize) {
        // Not enough space in the buffer :-(
        status = -1;
    }

    return status;
}

int beginHttpRequest(char *httpRequest, unsigned int httpRequestSize, const char *host, const char *httpVerb) {
    return addHttpRequestLine(httpRequest, httpRequestSize, "%s %s HTTP/1.1\r\n", httpVerb, host);
}

int beginHttpGetRequest(char *httpRequest, unsigned int httpRequestSize, const char *host, const char *httpVerb, const char *httpQuery) {
    if ((httpQuery != NULL) && (strlen(httpQuery) > 0)) {
        return addHttpRequestLine(httpRequest, httpRequestSize, "%s %s?%s HTTP/1.1\r\n", httpVerb, host, httpQuery);
    }

    return addHttpRequestLine(httpRequest, httpRequestSize, "%s %s HTTP/1.1\r\n", httpVerb, host);
}

int addHttpRequestHeader(char *httpRequest, unsigned int httpRequestSize, const char *httpHeader, const char *httpRequestHeaderValue) {
    if (strlen(httpRequestHeaderValue) == 0) {
        return 0;
    }

    return addHttpRequestLine(httpRequest, httpRequestSize, "%s: %s\r\n", httpHeader, httpRequestHeaderValue);
}

int addHttpRequestHeaderInt(char *httpRequest, unsigned int httpRequestSize, const char *httpHeader, int httpRequestHeaderValue) {
    return addHttpRequestLine(httpRequest, httpRequestSize, "%s: %d\r\n", httpHeader, httpRequestHeaderValue);
}

int getHttpResponseStatus(const char *httpResponse) {
    char httpStatus[4] = {0};
    memcpy(httpStatus, httpResponse + magicOffset, magicOffset3);

    return atoi(httpStatus);
}

const char *getHttpResponseBody(const char *httpResponse) {
    // If we don't find the header terminator, that means there's no body, so passing NULL
    // or empty string to callback is ok
    char *httpResponseBody = strstr(httpResponse, HTTP_HEADERS_END);
    if ((httpResponseBody != NULL) && (strlen(httpResponseBody) >= magicOffset2)) {
        httpResponseBody += magicOffset2;
    }

    return httpResponseBody;
}
