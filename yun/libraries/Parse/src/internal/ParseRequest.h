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

#ifndef ParseRequest_h
#define ParseRequest_h

/*! \file ParseRequest.h
 *  \brief ParseRequest object for the Yun
 *  include Parse.h, not this file
 */

/*! \class ParseRequest
 *  \brief Class responsible for Parse requests
 */
class ParseRequest {
protected:
	String httpPath;
	String requestBody;
	bool isBodySet;
public:
  /*! \fn ParseRequest()
   *  \brief Constructor of ParseRequest object
   */
  ParseRequest();

  /*! \fn ~ParseRequest()
   *  \brief Destructor of ParseRequest object
   */
  ~ParseRequest();

  /*! \fn void setClassName(const char* className)
   *  \brief set the ParseObject class name in which request will be performed.
   *
   * NOTE: ONLY use when request(GET/UPDATE/DELETE/CREATE/QUERY) is related with object
   *       for system object "Installation/User/Role", use "_Installatoin/_User/_Role" for className
   *       DO NOT setClassName for ParseCloudFunction, use setFunctionName
   *       DO NOT setClassName for ParseCloudFunction, use setEventName
   *
   *  \param className class name.
   */
  void setClassName(const char* className);

  /*! \fn void setObjectId(const char* objectId)
   *  \brief set the ParseObject object id in which request will be performed.
   *
   * NOTE: ONLY setObjectId for GET/UPDATE/DELETE request on a specific object
   *       for CREATE request, a new object id will be return on response
   *
   *  \param objectId object id.
   */
  void setObjectId(const char* objectId);

  /*! \fn virtual ParseResponse send()
   *  \brief execute the parse request.
   *
   *  \result response of request
   */
  virtual ParseResponse send() = 0;
};

#endif
