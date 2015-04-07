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
#ifndef PARSE_UTILS_H
#define PARSE_UTILS_H

#ifdef __cplusplus
extern "C"
{
#endif

  /*
  * Strip the http header from complete http response
  * @params
  * data - http response
  * size - size of data
  */
  int stripHttpHeader(char* data, int size);

  /*
  * Strip the empty JSON "{}" from multiple JSONs
  * data - JSON string contain one or multiple JSONs
  * size - size of data
  */
  int stripEmptyJSON(char* data, int size);

  /*
  * A very lightweight JSON parser to get the value by key
  * @params
  * data - JSON string to parse
  * key - key to find
  * value - returned value (always as string) or NULL if just to check values'
  * presence
  * size - size of the return buffer
  * @return 1 if found 0 otherwise
  */
  int getValueFromJSON(const char *data, const char *key, char* value, int size);

  /*
  * A very lightweight JSON parser to get the values by key
  * @params
  * data - JSON string contain one or multiple JSONs* key - key to find
  * values - returned values (always as string) or NULL if just to check values'presence
  * size - size of the return buffer
  * @return 1 if found 0 otherwise                  */
  int getValuesFromJSONs(const char* data, const char *key, char* values, int size);
  
  /*
  * Count the element number in a JSON array
  * @params
  *  data - JSON array, e.g. [{"foo":"bar"},{},{}]
  */
  int getCountFromJSONArray(const char* data);
  
  /*
  * Get the next JSON object in a JSON array
  * @params
  *  data - JSON array, e.g. [{"foo":"bar"},{},{}]
  *  begin - the starting point to look for next JSON
  *  size - next JSON's size will be filled in this variable
  * @return the starting point of next JSON
  */
  const char* getNextFromJSONArray(const char* data, const char* begin, int* size);
  
  /*
  * A url encoder to encode url parameters
  * @params
  * str - string to be encoded
  * enc - a url-encoded version of str
  */
  void urlEncode(char *str, char* enc);

  /*
  * A UUID generator
  * @params
  * id - uuid to generate
  */
  void getUUID(char *id);

  /*
  * Write a single id string to file
  */
  void writeId(const char* filename, const char* id);

  /*
  * read a single id string from file
  */
  void readId(const char* filename, char* id, int size);

#ifdef __cplusplus
}
#endif
#endif
