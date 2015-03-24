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

#ifndef __PARSE_H__
#define __PARSE_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*! \mainpage Parse Embedded C SDK
 *
 *  The Parse Embedded C SDK offers set of methods that enable the use of the
 *  <a href="https://parse.com">Parse platform</a> from C and C++ applications running on
 *  Linux and RTOS platforms.
 */

/*! \file parse.h
 *  \brief The embedded API header
 */

/*! \def APPLICATION_ID_MAX_LEN
 *  \brief The length of application id
 */
#define APPLICATION_ID_MAX_LEN 40

/*! \def CLIENT_KEY_MAX_LEN
 *  \brief The length of client key
 */
#define CLIENT_KEY_MAX_LEN 40

/*! \def OBJECT_ID_MAX_LEN
 *  \brief The length of object id
 */
#define OBJECT_ID_MAX_LEN 16

/*! \def INSTALLATION_ID_MAX_LEN
 *  \brief The length of installation id
 */
#define INSTALLATION_ID_MAX_LEN 36

/*! \def SESSION_TOKEN_MAX_LEN
 *  \brief The length of session token
 */
#define SESSION_TOKEN_MAX_LEN 40

struct ParseClientInternal;
/*! \typedef ParseClient
 *  \brief Parse client handle
 *
 *  This handle is used with all SDK methods to associate the specific client
 *  (installation, and optionally user) with the API calls.
 *
 *  A client has to be initialized using parseInitialize() which will associate
 *  it with a specific Parse application and the key to use when making requests.
 *
 *  Each client has an associated installation id that represents the device,
 *  and an optional session token that represents the user to the Parse backend.
 *  Both installation id and session token can be explicitly set for the client.
 *  If an installation id is not explicitly set, the SDK will generate a new one
 *  the first time it need it.
 *  If a session token is not set, the SDK will NOT generate one and will work
 *  in unauthenticated mode.
 */
typedef const struct ParseClientInternal *ParseClient;

/*! \typedef parseRequestCallback
 *  \brief Callback for API requests.
 *
 *  Called when an API request is finished.
 *
 *  \param[in]  client           The Parse client that made the API request.
 *  \param[in]  error            OS-specific error code.
 *                               If the API request was successful, this will be 0.
 *                               If any error occured during the request, the rest of the
 *                               parameters are meaningless.
 *  \param[in]  httpStatus       HTTP status of the API request.
 *  \param[in]  httpResponseBody The response body for the request. If the request
 *                               is successful, this is a JSON document describing the
 *                               result. If the request is unsuccessful, this contains
 *                               error details.
 *
 *  The SDK retains ownership of the response body. If you need to retain any data from it
 *  for use outside of the scope of the callback, make a copy.
 */
typedef void (*parseRequestCallback)(ParseClient client, int error, int httpStatus, const char* httpResponseBody);

/*! \typedef parsePushCallback
 *  \brief Callback for push notifications and errors from the push service.
 *
 *  \param[in]  client           The Parse client associated with the callback.
 *  \param[in]  error            OS-specific error code.
 *                               If the push service received the notification data
 *                               normally, this will be 0.
 *                               If any error occurred during receiving
 *                               the notification data, the rest of the parameters are
 *                               meaningless.
 *  \param[in]  data             The data for the incoming push notification. If the notification
 *                               is received successfully, this is a JSON document describing the
 *                               payload for the notification.
 *
 *  The SDK retains ownership of the notification data buffer. If you need to retain any data from it
 *  for use outside of the scope of the callback, make a copy.
 */
typedef void (*parsePushCallback)(ParseClient client, int error, const char* data);

/*! \fn ParseClient parseInitialize(const char *applicationId, const char *clientKey)
 *  \brief Initialize the Parse client and user session
 *
 *  This method only initializes the Parse client, used for subsequent API requests. It does
 *  not start the push service, and does not create or load the installation object for the client.
 *
 *  \param[in]  applicationId    The application id for the Parse application. (required)
 *  \param[in]  clientKey        The client API key for the Parse application. (required)
 *
 *  \result                      The client handle to use with the SDK.
 *
 *  The caller retains ownership of the applicationId and clientKey buffers, and is responsible for
 *  freeing them and reclaiming the memory after this call.
 *  The SDK will make copies of the buffers.
 */
ParseClient parseInitialize(const char *applicationId, const char *clientKey);

/*! \fn void parseSetInstallationId(ParseClient client, const char *installationId)
 *  \brief Set the installation object id for this client.
 *
 *  Set the installation object id for this client. If this is not called before making an
 *  API request or starting push service, the SDK will create a new installation object for
 *  the client.
 *
 *  \param[in]  client           The Parse client for which the installation id will be set.
 *  \param[in]  installationId   The installation id to be used by the client.
 *
 *  The caller retains ownership of the installationId buffer, and is responsible for
 *  freeing it and reclaiming the memory after this call.
 *  The SDK will make copies of the buffer.
 */
void parseSetInstallationId(ParseClient client, const char *installationId);

/*! \fn const char *parseGetInstallationId(ParseClient client)
 *  \brief Return the client installation id
 *
 *  Return the installation id for the client. If called before the installation id for the
 *  client was set, this method will NOT create a new installation object.
 *  The typical usage scenario is:
 *
 *  1.   Call parseInitialize().
 *  2.   Call parseGetInstallationId().
 *  3.a. If the result is not NULL, the SDK has a previous installation id,
 *       there's nothing else to do.
 *       Otherwise:
 *  3.b.1. If the device has already an installation id, call parseSetInstallationId().
 *  3.b.2. If the device doesn't have an installation id, the SDK will generate one
 *         the first time it needs it.
 *
 *  \param[in]  client           The Parse client for which the installation id should be returned.
 *
 *  \result                      The instalaltion id.
 *
 *  The SDK retains ownership of the result buffer, and still owns it after
 *  this call. Do not free it.
 */
const char *parseGetInstallationId(ParseClient client);

/*! \fn void parseSetSessionToken(ParseClient client, const char *sessionToken)
 *  \brief Set the session token for the Parse client
 *
 *  \param[in]  client           The Parse client for which the session token is set.
 *  \param[in]  sessionToken     The new session token. All subsequent API calls will
 *                               use this token for authentication and authorization.
 *                               If this is NULL, the client will be unauthenticated.
 *
 *  The caller retains ownership of the sessionToken buffer, and is responsible for
 *  freeing it and reclaiming the memory after this call.
 *  The SDK will make copies of the buffer.
 */
void parseSetSessionToken(ParseClient client, const char *sessionToken);

/*! \fn void parseClearSessionToken(ParseClient client)
 *  \brief Clear the session token
 *
 *  Same as parseSetSessionToken(client, NULL);
 */
void parseClearSessionToken(ParseClient client);

/*! \fn const char *parseGetSessionToken(ParseClient client)
 *  \brief Return the client session token.
 *
 *  Return the session token for the client, or NULL if there isn't one.
 *
 *  \param[in]  client           The Parse client for which the session token should be returned.
 *
 *  \result                      The session token.
 *
 *  The SDK retains ownership of the result buffer, and still owns it after
 *  this call. Do not free it.
 */
const char *parseGetSessionToken(ParseClient client);

/*! \fn void parseSetPushCallback(ParseClient client, parsePushCallback callback)
 *  \brief Set the callback for push notifications and errors
 *
 *  This method sets or clears the callback for push notifications and errors from
 *  the push service, associated with this client.
 *
 *  This method DOES NOT start or stop the push service.
 *
 *  \param[in]  client           The Parse client for which the callback is set.
 *  \param[in]  callback         The new callback method. All subsequent push notifications
 *                               or errors will result  in a call to this callback method.
 *                               If this parameter is NULL, the callback will be removed.
 *
 *  Any push notifications received while there is no callback associated with the client
 *  will be skipped, and the application will not received them.
 */
void parseSetPushCallback(ParseClient client, parsePushCallback callback);

/*! \fn int parseStartPushService(ParseClient client)
 *  \brief Start the push notifications service.
 *
 *  This method will start the push notification service and will listen for incoming
 *  push notifications. If the push service is already started, this will do nothing.
 *  To actually process incoming push notifications, it is still necessary to repeatedly
 *  call parseProcessNextPushNotification() or call parseRunPushLoop().
 *
 *  \param[in]  client           The Parse client for which the service should be started.
 *
 *  \result                      OS-specific error if the push can't be started or 0 if
 *                               started successfully.
 *
 *  If a push callback has been set for the client, any errors during starting the service will
 *  passed to it as well.
 *
 */
int parseStartPushService(ParseClient client);

/*! \fn void parseStopPushService(ParseClient client)
 *  \brief Stop the push notifications service.
 *
 *  This method will stop the push notification service. If the push service is not started,
 *  this will do nothing.
 *
 *  \param[in]  client           The Parse client for which the service should be stopped.
 */
void parseStopPushService(ParseClient client);

/*! \fn int parseProcessNextPushNotification(ParseClient client)
 *  \brief Process next pending push notification.
 *
 *  Push notifications are processed one at a time. This method will process the next push
 *  notification and will call the client callback, if one is set.
 *
 *  \param[in]  client           The Parse client for which the next event should be processed.
 *
 *  \result                      0 if there are no more pending notification events or the push
 *                               service is not started.
 *                               Positive number if there are more pending notification events.
 *
 *  If the push notifications callback has been set for the client, it will also be called in
 *  the case of an error during the processing of the notification. If the callback is not set,
 *  the client will not get notified about any errors that during the processing of the notification.
 */
int parseProcessNextPushNotification(ParseClient client);

/*! \fn int parseGetPushSocket(ParseClient client)
 *  \brief Request Parse push file handle for use with select call.
 *
 * You may want to wait on several system events. This returns
 * the file descriptor used for push so that you can use a
 * 'select' call to wait on multiple descriptors.
 *
 * Example of using this handle on posix systems with select
 * while implementing a custom event loop:
 *
 * \code
 * int socket = parseGetPushSocket(client);
 * while(1) {
 *   struct timeval tv;
 *   fd_set receive, send, error;
 *
 *   ...
 *
 *   FD_SET(socket, &error);
 *   FD_SET(socket, &receive);
 *
 *   // Add other file handles with FD_SET
 *   // that you are interrested in
 *
 *   select(socket + 1, &receive, &send, &error, &tv);
 *   parseProcessNextPushNotification(client);
 * }
 * \endcode
 *
 *  \param[in]  client           The Parse client for which the push has been started
 */
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__)) || defined (PART_CC3200)
int parseGetPushSocket(ParseClient client);
#endif

/*! \fn void parseRunPushLoop(ParseClient client)
 *  \brief Process push notifications events in a loop.
 *
 *  This method will keep processing the push notification events for the client and will call
 *  the client callback for each one. If the push notification service for the client is not
 *  started or is stopped, this method will exit.
 *
 *  \param[in]  client           The Parse client for which the push events should be processed.
 */
void parseRunPushLoop(ParseClient client);

/*! \fn void parseSendRequest(ParseClient client, const char *httpVerb, const char *httpPath, const char *httpRequestBody, parseRequestCallback callback)
 *  \brief Send an API request.
 *
 *  This method makes an API request for the client. If called before installation id for the client
 *  is set, this will also create a new installation object.
 *
 *  The API requests supported are any valid reuests that can be made through the REST API as well.
 *
 *  \param[in]  client           The Parse client for which the request is made.
 *  \param[in]  httpVerb         The type of request - POST, GET, PUT, DELETE
 *  \param[in]  httpPath         The path for the request, i.e. /1/classes/MyClass
 *  \param[in]  httpRequestBody  The JSON payload for the request
 *  \param[in]  callback         The callback to process the result  of the request.
 *
 *  The caller retains ownership of the httpVerb, httpPath, and requestBody buffers, and is responsible for
 *  freeing them and reclaiming the memory after this call.
 */
void parseSendRequest(ParseClient client, const char *httpVerb, const char *httpPath, const char *httpRequestBody, parseRequestCallback callback);

/*! \fn int parseGetErrorCode(const char *httpResponseBody)
 *  \brief Extract Parse error code.
 *
 *  Helper function to extract Parse errors from the response body, when httpStatus of the response is 4xx or 5xx.
 *  TODO: Link to the public documentation of the Parse error codes
 *
 *  \param[in]  httpResponseBody The response body for the request.
 *
 *  \result                      Return the error code from the server.
 *
 * The caller retains ownership of the httpResponseBody buffer, and is responsible for
 * freeing it and reclaiming the memory after this call.
 */
int parseGetErrorCode(const char *httpResponseBody);

#ifdef __cplusplus
}
#endif

#endif /* __PARSE_H__ */
