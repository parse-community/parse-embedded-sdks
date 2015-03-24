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

#pragma once

#ifndef __SIMPLEJSON_H__
#define __SIMPLEJSON_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>

/**
 * A very lightweight JSon parser
 * @params
 *   data - JSON string to parse
 *   key - key to find
 *   value - returned value (always as string) or NULL if just to check values'
 *     presence
 *   size - size of the return buffer
 * @return 1 if found 0 otherwise
 */
int simpleJsonProcessor(const char *data, const char *key, char* value, int size);

/**
 * @params
 *   data - JSON object(s)
 * @return pointer to the second json object, NULL otherwise.
 */
const char* findSecondJson(const char *data);

/**
 * @params
 *   data - JSON object(s)
 * @return 1 if it is complete, 0 if you need to read more.
 */
int isCompleteJson(const char *data);

/**
 * @params
 *   data - [in] data to parse. Do not need to be NULL terminated.
 *   dataSize - [in] size of the data in bytes
 *   start - [out] offset of the first byte of the JSON or -1 if there nothing.
 *   length - [out] the length of the JSON  or -1 if the message is partial.
 * @return
 *   start of the JSON or NULL.
 */
const char *getPushJson(const char *data, size_t dataSize,
                        int *start, int *length);

#ifdef __cplusplus
}
#endif

#endif
