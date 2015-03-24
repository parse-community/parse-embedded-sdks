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

#include "simplejson.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// Helper functions
static const char *findJsonBeginning(const char* data);
static const char *findJsonBeginningParams(
  const char* data, size_t size, int *inString, int *pos);
static const char *findJsonEnd(const char* data);
static const char *findJsonEndParams(
  const char* data, size_t size, int *inString, int *level, int *pos);


int simpleJsonProcessor(const char *data, const char *key, char* value, int size) {
    const char* found = NULL;
    int inString = 0;
    int json = 0;
    int backslash = 0;
    int endWithQuote = 1;
    int keyLen = strlen(key);
    int jsonLevel = 0;
    if (!data || !key || !*key)
      return 0;
    for (found = data; *found; ++found) {
        if (strncmp(found, key, keyLen)) {
            switch (*found) {
            case '\"':
                inString = 1 - inString;
                break;
            case '\\':
                if (inString) {
                    if (*(found + 1))
                        ++found;
                }
                break;
            case '{':
            case '[': // treat array as JSON
                if (!inString) {
                  ++jsonLevel;
                }
                break;
            case '}':
            case ']':
                if (!inString) {
                  --jsonLevel;
                  if (jsonLevel < 0) {
                    // Quit on malformed json
                    return 0;
                  }
                }
                break;
            }
            continue;
        } else if (jsonLevel > 1) {
            continue;
        }
        found += keyLen;
        if (*found != '\"') {
            data = found;
            continue;
        } else {
            ++found;
            for (; *found == ' ' || *found == '\t'; ++found);
            if (*found != ':') {
                data = found;
                continue;
            }
            ++found;
            for (; *found == ' ' || *found == '\t'; ++found);
            if (*found == '{' || *found == '[') {
                json = 1;
                endWithQuote = 0;
            } else {
                if (*found != '\"')
                    endWithQuote = 0;
            }
            break;
        }
    }

    if (!*found)
        return 0;

    if (!value)
        return 1;

    if (endWithQuote)
      ++found;
    inString = !json;
    jsonLevel = 0;

    for (; size > 1 && *found; --size, ++found, ++value) {
        if (backslash) {
            backslash = 0;
            *value = *found;
            continue;
        }
        switch (*found) {
            case '{':
            case '[':
              if (!inString)
                ++jsonLevel;
              break;
            case '}':
            case ']':
                if (!endWithQuote) {
                    if (!inString) {
                        if (jsonLevel) {
                            --jsonLevel;
                            if (jsonLevel < 0) {
                              // Quit on malformed json
                              return 0;
                            }
                        }
                        if (!jsonLevel) {
                            if (json) {
                                *value = *found;
                                ++value;
                            }
                            goto RETURN_VALUE;
                        }
                        break;
                    }
                    // Fall through
                } else {
                    // Fall through
                }
            case '\"':
            case ',':
                if (endWithQuote && *found != '\"')
                  break;

                if (*found == '\"')
                  inString = 1 - inString;
                if (!json)
                    goto RETURN_VALUE;
                break;
            case '\\': backslash = 1; break;
        }
        *value = *found;
    }
RETURN_VALUE:
    *value = 0;
    if (!*found) // malformed JSON
      return 0;
    return 1;
}

static const char *findJsonBeginning(const char* data) {
  int inString = 0;
  int pos = 0;
  return findJsonBeginningParams(data, strlen(data), &inString, &pos);
}

static const char *findJsonBeginningParams(
    const char* data, size_t size, int *inString, int *pos) {
  for (; *data && *pos < size; ++data, ++(*pos)) {
    switch (*data) {
      case '{':
        if (!*inString)
          return data;
        break;
      case '\"':
        *inString = 1 - *inString;
        break;
      case '\\':
        if (*inString && *(data + 1)) {
          ++(*pos);
          ++data;
        }
        break;
    }
  }
  return NULL;
}

static const char *findJsonEnd(const char* data) {
  int inString = 0;
  int level = 0;
  int pos = 0;
  return findJsonEndParams(data, strlen(data), &inString, &level, &pos);
}

static const char *findJsonEndParams(
    const char* data, size_t size, int *inString, int *level, int *pos) {
  for (; *data && *pos < size; ++data, ++(*pos)) {
    switch (*data) {
      case '{':
        if (!*inString)
          ++(*level);
        break;
      case '}':
        if (!*inString)
          --(*level);
        if (!*level)
          return data + 1;
        break;
      case '\"':
        *inString = 1 - *inString;
        break;
      case '\\':
        if (*inString && *(data + 1)) {
          ++(*pos);
          ++data;
        }
        break;
    }
  }
  return NULL;
}

const char* findSecondJson(const char *data) {
  data = findJsonBeginning(data);
  if (!data)
    return NULL;
  data = findJsonEnd(data);
  if (!data)
    return NULL;
  return findJsonBeginning(data);
}

int isCompleteJson(const char *data) {
  data = findJsonBeginning(data);
  if (!data)
    return 0;
  return (findJsonEnd(data) != NULL);
}

const char *getPushJson(const char *data, size_t dataSize, int *start, int *length) {
  int inString = 0;
  int pos = 0;
  const char *beginning = findJsonBeginningParams(data, dataSize, &inString, &pos);
  if (!beginning) {
    *start = -1;
    *length = -1;
    return NULL;
  }

  inString = 0;
  int endPos = 0;
  int level = 0;
  const char *end;

  end = findJsonEndParams(data + pos, dataSize - pos, &inString, &level, &endPos);

  *start = pos;
  if (end)
    *length = endPos + 1;
  else
    *length = -1;
  return data + pos;
}
