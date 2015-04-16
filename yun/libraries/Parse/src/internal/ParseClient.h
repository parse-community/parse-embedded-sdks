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

#ifndef ParseClient_h
#define ParseClient_h

#if defined (ARDUINO_AVR_YUN) || defined (ARDUINO_AVR_TRE)

#include "ParseResponse.h"
#include "ParsePush.h"
#include <Process.h>

/*! \file ParseClient.h
 *  \brief ParseClient object for the Yun
 *  include Parse.h, not this file
 */

/*! \class ParseClient
 *  \brief Class responsible for Parse connection
 */
class ParseClient {
private:
  const char* installationId;
  const char* sessionToken;
  void read(Process* client, char* buf, int size);
  Process pushClient;
  Process requestClient;

public:
  /*! \fn ParseClient()
   *  \brief Constructor of ParseClient object
   */
  ParseClient();

  /*! \fn begin(const char *applicationId, const char *clientKey)
   *  \brief Initialize the Parse client and user session
   *
   *  This method only initializes the Parse client, used for subsequent API requests. It does
   *  not start the push service, and does not create or load the installation object for the client.
   *
   *  \param applicationId The application id for the Parse application. (required)
   *  \param clientKey     The client API key for the Parse application. (required)
   */
  void begin(const char *applicationId, const char *clientKey);

  /*! \fn void setInstallationId(const char *installationId)
   *  \brief Set the installation object id for this client.
   *
   *  Set the installation object id for this client. If this is not called before making an
   *  API request or starting push service, the SDK will create a new installation object for
   *  the client.
   *
   *  \param  installationId   The installation id to be used by the client.
   *
   */
  void setInstallationId(const char *installationId);

  /*! \fn const char * getInstallationId()
   *  \brief Return the client installation id
   *
   *  Return the installation id for the client. If called before the installation id for the
   *  client was set, this method will NOT create a new installation object.
   *  The typical usage scenario is:
   *
   *  1.   Call begin().
   *  2.   Call getInstallationId().
   *  3.a. If the result is not NULL, the SDK has a previous installation id,
   *       there's nothing else to do.
   *       Otherwise:
   *  3.b.1. If the device has already an installation id, call setInstallationId().
   *  3.b.2. If the device doesn't have an installation id, the SDK will generate one
   *         the first time it needs it.
   *
   *  \result                      The instalaltion id.
   *
   */
  const char* getInstallationId();

  /*! \fn void setSessionToken(const char *sessionToken)
   *  \brief Set the session token for the Parse client
   *
   *  \param  sessionToken     The new session token. All subsequent API calls will
   *                           use this token for authentication and authorization.
   *                           If this is NULL, the client will be unauthenticated.
   *
   */
  void setSessionToken(const char* sessionToken);

  /*! \fn void clearSessionToken()
   *  \brief Clear the session token
   *
   *  Same as setSessionToken(NULL);
   *
   */
  void clearSessionToken();

  /*! \fn const char *getSessionToken()
   *  \brief Return the client session token.
   *
   *  Return the session token for the client, or NULL if there isn't one.
   *
   *  \result                  The session token.
   */
  const char* getSessionToken();

  /*! \fn ParseResponse sendRequest(const char* httpVerb, const char* httpPath, const char* requestBody, const char* urlParams)
   *  \brief Directly call REST API in Parse.
   *
   *  \param   httpVerb - GET/DELETE/PUT/POST
   *  \param   httpPath - the endpoint of REST API e.g. /1/installations
   *  \param   requestBody - http request body in json format, leave it as "" for DELTE/GET requests
   *  \param   urlParams - leave it as "" unless to perform a Parse query,
   *            use it to specify query condition e.g. where={"KEY1"":VALUE1}&limit=10&keys=KEY1,KEY2
   *  \result response of request
   *  NOTE: ParseObjectCreate, ParseObjectUpdate, ParseObjectDelete, ParseGETObject,
   *  ParseQuery, ParseCloudFunction, ParseTrackEvent can also be used for REST API call
   */
  ParseResponse sendRequest(const char* httpVerb, const char* httpPath, const char* requestBody, const char* urlParams);

  /*! \fn ParseResponse sendRequest(const String& httpVerb, const String& httpPath, const String& requestBody, const String& urlParams)
   *  \brief Directly call REST API in Parse.
   *
   *  \param   httpVerb - GET/DELETE/PUT/POST
   *  \param   httpPath - the endpoint of REST API e.g. /1/installations
   *  \param   requestBody - http request body in json format, leave it as "" for DELTE/GET requests
   *  \param   urlParams - leave it as "" unless to perform a Parse query,
   *            use it to specify query condition e.g. where={"KEY1"":VALUE1}&limit=10&keys=KEY1,KEY2
   *  \result response of request
   *  NOTE: ParseObjectCreate, ParseObjectUpdate, ParseObjectDelete, ParseGETObject,
   *  ParseQuery, ParseCloudFunction, ParseTrackEvent can also be used for REST API call
   */
  ParseResponse sendRequest(const String& httpVerb, const String& httpPath, const String& requestBody, const String& urlParams);

  /*! \fn int startPushService()
   *  \brief Start the push notifications service.
   *
   *  This method will start the push notification service and will listen for incoming
   *  push notifications. If the push service is already started, this will do nothing.
   *  To actually process incoming push notifications, it is still necessary to repeatedly
   *  call
   *  \code
   *  if (Parse.pushAvailable()) {
   *    ParsePush push = Parse.nextPush();
   *    // Process pushes here
   *    ...
   *  }
   *  \endcode
   *
   *  \result                    false if the push can't be started or true if
   *                             started successfully.
   */
  bool startPushService();

  /*! \fn void stopPushService()
   *  \brief Stop the push notifications service.
   *
   *  This method will stop the push notification service. If the push service is not started,
   *  this will do nothing.
   */
  void stopPushService();

  /*! \fn bool pushAvailable()
   *  \brief Check if there is any new push message.
   *
   *  use only after startPushService()
   *
   *  \result                    true if there is a push message, false otherwise.
   */
  bool pushAvailable();

  /*! \fn ParsePush nextPush()
   *  \brief Get next push message.
   *
   *  NOTE: use only when pushAvailable() is TRUE
   *
   *  \result                    a ParsePush instance which contains one or multiple push messages
   */
  ParsePush nextPush();

  /*! \fn void end()
   *  \brief Release resource.
   */
  void end();

  friend class ParseResponse;
  friend class ParsePush;
};

/*! \var Parse
 *  \brief The global parse object.
 */
extern ParseClient Parse;

#endif //ARDUINO_AVR_YUN
#endif
