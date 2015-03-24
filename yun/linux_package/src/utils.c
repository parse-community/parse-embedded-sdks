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

#include "utils.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int stripHttpHeader(char* data, int size) {
  int found = 0;
  char* pre = data;
  char* cur = data+1;
  while(*cur != 0) {
    if(*pre == '\r' && *cur == '\n') {
      pre += 2;
      cur += 2;
      if(*pre == '\r' && *cur == '\n') {
        //http header end found
        found = 1;
        break;
      }
    }
    pre = cur;
    cur++;
  }
  if (!found) {
    return 0;
  }

  cur++;
  char* s = data;
  while(*cur != 0) {
    *s = *cur;
    s++;
    cur++;
  }
  while(*s != 0) {
    *s = 0;
    s++;
  }
  return 1;
}

int stripEmptyJSON(char* data, int size) {
  int found = 0;
  int p = 0, i = 0;
  while (p < size - 2) {
    if (strncmp("{}", data+p, 2) == 0) {
      found = 1;
      break;
    }
    p++;
  }
  if (!found) {
    return 0;
  }
  i = p+2;
  while(i < size && data[i] != '{') {
    i++;
  }
  for (; i < size; p++, i++) {
    data[p] = data[i];
  }
  for (; p < size; p++) {
    data[p] = 0;
  }
  return 1;
}

int getValueFromJSON(const char *data, const char *key, char* value, int size) {
  const char* found = NULL;
  int inString = 0;
  int json = 0;
  int backslash = 0;
  int endWithQuote = 1;
  int keyLen = strlen(key);
  int escape = 0;
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
      if (*found == '\"') //if(!json)
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

int getValuesFromJSONs(const char* data, const char *key, char* values, int size) {
  const char* p = data;
  int s = 0;
  int c = 0;
  int found = 0;
  while(*p) {
    if(*p == '{') {
      c++;
    } else if (*p == '}') {
      c--;
    }
    if(*p == '{' && c == 1 ) {// a new json start
      if(getValueFromJSON(p, key, values+s, size-s)) {
        found = 1;
        s = strlen(values);
      }
    }
    p++;
  }

  return found;
}

int getCountFromJSONArray (const char* data) {
  int c = 0;
  const char* next = data;
  int nextSize = 0;
  while((next = getNextFromJSONArray(data, next + nextSize, &nextSize)) != NULL) {
    if(nextSize > 0) {c++;}
    else {break;}
  }
  return c;
}

const char* getNextFromJSONArray(const char* data, const char* begin, int* size) {
  const char* p = begin;
  const char* start = NULL;
  int tmp = 0;
  int found = 0;
  int s = 0;
  while (*p) {
    if (*p == '{') {
      if (tmp == 0) {
        start = p;
        found = 1;
      }
      tmp++;
    } else if (*p == '}') {
      if( tmp > 0) tmp--;
      if (tmp == 0) {
        if (found) {s++;}
        break;
      }
    }
    if (found) {s++;}
    p++;
  }
  *size = s;
  return start;
}

void urlEncode(char *s, char* enc) {
  int i;
  char rfc3986[256] = {0};
  for (i = 0; i < 256; i++) {
    rfc3986[i] = isalnum(i)||i == '~'||i == '-'||i == '.'||i == '_' || i == '&' || i == '='
      ? i : 0;
  }
  for (; *s; s++) {
    if (rfc3986[*s]) {
      sprintf(enc, "%c", rfc3986[*s]);
    } else {
      sprintf(enc, "%%%02X", *s);
    }
    while (*++enc);
  }
}

/*
* !!! IMPORTANT !!!
* The libuuid is not complete in Yun
* This function does not conform to RFC 4122, even though it returns
* a string formatted as an UUID. Do not use this to generate proper UUID!
*/
void getUUID(char *id) {
  /*uuid_t uuid;
  char uuid_string[37];
  uuid_generate(uuid);
  uuid_unparse(uuid, uuid_string);
  memcpy(id, uuid_string, 37);
  int i = 0;
  for ( ; i < strlen(id) ; i++) {
  id[i] = tolower(id[i]);
  }*/
  static int randInitialized = 0;
  if (!randInitialized) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    srand(tm.tm_sec);
    randInitialized = 1;
  }
  sprintf(id, "%x%x%x%x%x%x%x%x-%x%x%x%x-%x%x%x%x-%x%x%x%x-%x%x%x%x%x%x%x%x%x%x%x%x",
  rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16,
  rand()%16, rand()%16, rand()%16, rand()%16,
  rand()%16, rand()%16, rand()%16, rand()%16,
  rand()%16, rand()%16, rand()%16, rand()%16,
  rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16, rand()%16
    );
}

void writeId(const char* filename, const char* id) {
  FILE *f = fopen(filename, "w");
  if (f == NULL || id == NULL) {
    return;
  }
  fprintf(f, "%s", id);
  fclose(f);
}

void readId(const char* filename, char* id, int size) {
  FILE *f = fopen(filename, "r");
  if (f == NULL || id == NULL) {
    return;
  }
  fgets(id, size, f);
  fclose(f);
}

