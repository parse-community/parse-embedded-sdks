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

#include<stdarg.h>
#include<stdio.h>

#include<parse_log.h>
#include<parse_time.h>

static int parse_log_level = PARSE_LOG_NONE;
static const char *str[] = { "NONE", "TEST", "ERROR", "WARN", "DEBUG", "INFO" };

void parseSetLogLevel(int level) {
    parse_log_level = level;
}


void parseLog(int level, const char* format, ...) {
    if (level == 0 || parse_log_level == 0) {
        return;
    }
    if (level > PARSE_LOG_MAX) {
        level = PARSE_LOG_MAX;
    }
    if (level <= parse_log_level) {
        va_list argptr;
        fprintf(stderr, "PARSELOG %u (%s): ", secondsSinceBoot(), str[level]);
        va_start(argptr, format);
        vfprintf(stderr, format, argptr);
        va_end(argptr);
    }
}
