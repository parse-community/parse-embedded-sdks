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

#include <ctype.h>

char fromHex(char ch) {
    return isdigit((int)ch) ? ch - '0' : tolower((int)ch) - 'a' + 10;
}

char toHex(char code) {
    static char hex[] = "0123456789abcdef";
    return hex[code & 15];
}

void urlEncode(const char *str, char *buf) {
    const char *pstr = str;
    char *pbuf = buf;
    while (*pstr) {
        if (isalnum((int)(*pstr)) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~') {
            *pbuf++ = *pstr;
        } else if (*pstr == ' ') {
            *pbuf++ = '+';
        } else {
            *pbuf++ = '%', *pbuf++ = toHex(*pstr >> 4), *pbuf++ = toHex(*pstr & 15);
        }
        pstr++;
    }
    *pbuf = '\0';
}

void urlDecode(const char *str, char *buf) {
    const char *pstr = str;
    char *pbuf = buf;
    while (*pstr) {
        if (*pstr == '%') {
            if (pstr[1] && pstr[2]) {
                *pbuf++ = fromHex(pstr[1]) << 4 | fromHex(pstr[2]);
                pstr += 2;
            }
        } else if (*pstr == '+') {
            *pbuf++ = ' ';
        } else {
            *pbuf++ = *pstr;
        }
        pstr++;
    }
    *pbuf = '\0';
}
