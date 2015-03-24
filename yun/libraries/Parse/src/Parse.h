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

#ifndef PARSE_H_
#define PARSE_H_

/*! \mainpage Parse Arduino Yun SDK
 *
 *  The Parse Arduino Yun SDK offers set of classes that enable the use of the
 *  <a href="https://parse.com">Parse platform</a> Wire-API applications running on
 *  Arduino Yun platform.
 */

/*! \file Parse.h
 *  \brief Main include file for object in Yun SDK
 *  include this file, not individual object files.
 */

#include <internal/ParseClient.h>
#include <internal/ParseResponse.h>
#include <internal/ParsePush.h>
#include <internal/ParseQuery.h>
#include <internal/ParseUtils.h>
#include <internal/ParseObjectCreate.h>
#include <internal/ParseObjectDelete.h>
#include <internal/ParseObjectGet.h>
#include <internal/ParseObjectUpdate.h>
#include <internal/ParseCloudFunction.h>
#include <internal/ParseTrackEvent.h>

#endif
