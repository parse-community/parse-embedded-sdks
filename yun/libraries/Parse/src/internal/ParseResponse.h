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

#ifndef ParseResponse_h
#define ParseResponse_h

#include <Process.h>

/*! \file ParseResponse.h
 *  \brief ParseResponse object for the Yun
 *  include Parse.h, not this file
 */

/*! \class ParseResponse
 *  \brief Class that encapsulates Rest API response.
 *  This object created indirectly.
 */
class ParseResponse {
protected:
  const static int BUFSIZE = 128;
  int bufSize;
  char* buf;
  char* tmpBuf;
  bool isUserBuffer;
  int p = 0;
  int resultCount = -1;
  Process* client;
  void read();
  void readWithTimeout(int maxSec);
  int available();
  void freeBuffer();
  ParseResponse(Process* client);

public:
  /*! \fn void setBuffer(char* buffer, int size)
   *  \brief set the customer buffer for writing response data.
   *
   *  \param buffer - char array buffer
   *  \param size - size of buffer
   *                NOTE: if buffer is not set, a default size of 128
   *                will be initialized.
   */
  void setBuffer(char* buffer, int size);

  /*! \fn int getErrorCode()
   *  \brief get the "error" field in the response.
   *
   *  \result error_code when error happens, 0 when there is no error
   */
  int getErrorCode();

  /*! \fn int getInt(const char* key)
   *  \brief get the integer value in the response by key
   *
   *  \param key - key
   *  \result the value
   */
  int getInt(const char* key);

  /*! \fn double getDouble(const char* key)
   *  \brief get the double value in the response by key.
   *
   *  \param key - key
   *  \result the value
   */
  double getDouble(const char* key);

  /*! \fn bool getBoolean(const char* key)
   *  \brief get the boolean value in the response by key
   *
   *  \param key - key
   *  \result the value
   */
  bool getBoolean(const char* key);

  /*! \fn const char* getString(const char* key)
   *  \brief get the string value in the response by key
   *
   *  \param key - key
   *  \result the value
   */
  const char* getString(const char* key);

  /*! \fn const char* getJSONBody()
   *  \brief get the complete json value of response.
   *
   *  \result returned JSON
   */
  const char* getJSONBody();

  /*! \fn bool nextObject()
   *  \brief ParseQuery ONLY: iterate to next object in query result
   *
   *  it has to be called before any object opreation(getInt/String/Double)
   *  including the first object
   *  \result  true if successfully iterate to next object
   *           false if there is no more object
   */
  bool nextObject();

  /*! \fn int count()
   *  \brief ParseQuery ONLY: get the count of the objects in query results
   *
   *  NOTE: if the query resutls exceed 2048 bytes(query result buffer size),
   *        it will only return the count of objects in the buffer.
   *  \result number of objects in the result
   */
  int count();

  /*! \fn void close()
   *  \brief free resource including system data buffer and client
   *
   *  NOTE: the customer buffer will NOT be freed, you need
   *        to free them by yourself
   */
  void close();

  friend class ParseClient;
};

#endif
