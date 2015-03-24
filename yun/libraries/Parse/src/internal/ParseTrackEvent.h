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

#ifndef ParseTrackEvent_h
#define ParseTrackEvent_h

#include "ParseObjectCreate.h"

/*! \file ParseTrackEvent.h
 *  \brief ParseTrackEvent object for the Yun
 *  include Parse.h, not this file
 */

/*! \class ParseTrackEvent
 *  \brief Class responsible for tracking an event.
 */
class ParseTrackEvent : public ParseObjectCreate {
public:
  /*! \fn ParseTrackEvent()
   *  \brief Constructor of ParseTrackEvent object
   */
  ParseTrackEvent();

  /*! \fn void setEventName(const char* eventName)
   *  \brief set the track event name, which will be available in Parse Analytics
   *
   *  \param eventName event name.
   */
  void setEventName(const char* eventName);
};

#endif
