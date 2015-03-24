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

#ifndef Utils_h
#define Utils_h

/*! \file ParseUtils.h
 *  \brief ParseUtils object for the Yun
 *  include Parse.h, not this file
 */

/*! \class ParseUtils
 *  \brief Utility class.
 */
class ParseUtils {
public:
  /*! \fn static int getStringFromJSON(const char* data, const char *key, char* value, int size)
   *  \brief A very lightweight JSON parser to get the string value by key
   *
   *  \param data - JSON string to parse
   *  \param key - key to find
   *  \param value - returned value (always as string) or NULL if just to check values'presence
   *  \param size - size of the return buffer
   *  \result 1 if found 0 otherwise
   */
  static int getStringFromJSON(const char* data, const char *key, char* value, int size) {
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

  /*! \fn static int getIntFromJSON(const char* data, const char* key)
   *  \brief A very lightweight JSON parser to get the integer value by key
   *
   * \param data - JSON string to parse
   * \param key - key to find
   * \param value - returned integer value, 0 if key not found
   */
  static int getIntFromJSON(const char* data, const char* key) {
    char* value = new char[10];
    if (ParseUtils::getStringFromJSON(data, key, value, 10)) {
      String c = value;
      int v = c.toInt();
      delete[] value;
      return  v;
    }
    return 0;
  }

  /*! \fn static double getFloatFromJSON(const char* data, const char* key)
   *  \brief A very lightweight JSON parser to get the float value by key
   *
   * \param data - JSON string to parse
   * \param key - key to find
   * \param value - returned double value, .0 if key not found
   */
  static double getFloatFromJSON(const char* data, const char* key) {
    char* value = new char[10];
    if (ParseUtils::getStringFromJSON(data, key, value, 10)) {
      String c = value;
      double v = c.toFloat();
      delete[] value;
      return  v;
    }
    return .0;
  }

  /*! \fn static double getFloatFromJSON(const char* data, const char* key)
   *  \brief A very lightweight JSON parser to get the boolean value by key
   *
   * \param data - JSON string to parse
   * \param key - key to find
   * \param value - returned boolean value. false if key not found
   */
  static bool getBooleanFromJSON(const char* data, const char* key) {
    char* value = new char[10];
    if (ParseUtils::getStringFromJSON(data, key, value, 10)) {
      String c = value;
      bool v = (c == "true")?true:false;
      delete[] value;
      return v;
    }
    return false;
  }
};

#endif
