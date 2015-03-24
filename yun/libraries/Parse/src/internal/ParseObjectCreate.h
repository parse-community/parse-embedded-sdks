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

#ifndef ParseObjectCreate_h
#define ParseObjectCreate_h

#include "ParseRequest.h"

/*! \file ParseObjectCreate.h
 *  \brief ParseObjectCreate object for the Yun
 *  include Parse.h, not this file
 */

/*! \class ParseObjectCreate
 *  \brief Class responsible for new Parse object creation
 */
class ParseObjectCreate : public ParseRequest {
protected:
	void addKey(const char* key);
public:
  /*! \fn ParseObjectCreate()
   *  \brief Constructor of ParseObjectCreate object
   */
  ParseObjectCreate() : ParseRequest() {}

  /*! \fn void add(const char* key, int d)
   *  \brief Add a key and integer value pair to object.
   *
   *  \param key The key name.
   *  \param d   The value.
   */
  void add(const char* key, int d);

  /*! \fn void add(const char* key, double d)
   *  \brief Add a key and double value pair to object.
   *
   *  \param key The key name.
   *  \param d   The value.
   */
  void add(const char* key, double d);

  /*! \fn add(const char* key, const char* s)
   *  \brief add a key and string value pair to object.
   *
   *  \param key The key name.
   *  \param s   The value.
   */
  void add(const char* key, const char* s);

  /*! \fn void add(const char* key, bool b)
   *  \brief add a key and boolean value pair to object.
   *
   *  \param key The key name.
   *  \param b   The value.
   */
  void add(const char* key, bool b);

  /*! \fn void addGeoPoint(const char* key, double lat, double lon)
   *  \brief add a key and GeoPoint value pair to object.
   *
   *  \param key The key name.
   *  \param lat The latitude value.
   *  \param lon The logitude value.
   */
  void addGeoPoint(const char* key, double lat, double lon);

  /*! \fn void addJSONValue(const char* key, const char* json)
   *  \brief add a key and json value pair to object.
   *
   *  \param key  The key name.
   *  \param json The value.
   */
  void addJSONValue(const char* key, const char* json);

  /*! \fn void addJSONValue(const char* key, const String& json)
   *  \brief add a key and json value pair to object.
   *
   *  \param key  The key name.
   *  \param json The value.
   */
  void addJSONValue(const char* key, const String& json);

  /*! \fn void setJSONBody(const char* jsonBody)
   *  \brief set the json body of object directly.
   *
   *  NOTE: this will remove all previous key-value pairs set if there are any
   *  \param jsonBody The new JSON body.
   */
  void setJSONBody(const char* jsonBody);

  /*! \fn void setJSONBody(const String& jsonBody)
   *  \brief set the json body of object directly.
   *
   *  NOTE: this will remove all previous key-value pairs set if there are any
   *  \param jsonBody The new JSON body.
   */
  void setJSONBody(const String& jsonBody);

  /*! \fn virtual ParseResponse send()
   *  \brief launch the object creation request and execute.
   *
   */
  virtual ParseResponse send();
};

#endif
