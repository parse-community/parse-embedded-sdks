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

#ifndef ParseQuery_h
#define ParseQuery_h

#include "ParseRequest.h"

/*! \file ParseQuery.h
 *  \brief ParseQuery object for the Yun
 *  include Parse.h, not this file
 */

/*! \class ParseQuery
 *  \brief Class responsible for query encapsulation
 */
class ParseQuery : public ParseRequest {
private:
	String whereClause;
	String order;
	String returnedFields;
	int limit;
	int skip;
	void addConditionKey(const char* key);
	void addConditionNum(const char* key, const char* comparator, double value);
public:
  /*! \fn ParseQuery()
   *  \brief Constructor of ParseCloudFunction object
   */
  ParseQuery();

  /*! \fn void whereExists(const char* key)
   *  \brief Add a constraint for finding objects that contain the given key.
   *
   *  \param key - the key that should exist.
   */
  void whereExists(const char* key);

  /*! \fn void whereDoesNotExist(const char* key)
   *  \brief add a constraint for finding objects that does not contain the given key.
   *
   *  \param key - the key that should not exist.
   */
  void whereDoesNotExist(const char* key);

  /*** string condition ***/

  /*! \fn void whereEqualTo(const char* key, const char* value)
   *  \brief add a constraint to the query that requires a particular key's value to be equal to the provided string value.
   *
   *  \param key - The key to check.
   *  \param value - The value that the ParseObject must contain.
   */
  void whereEqualTo(const char* key, const char* value);

  /*! \fn void whereNotEqualTo(const char* key, const char* value)
   *  \brief add a constraint to the query that requires a particular key's value not equal to the provided string value.
   *
   *  \param key - The key to check.
   *  \param value - The value that must not be equalled.
   */
  void whereNotEqualTo(const char* key, const char* value);

  /*** boolean condition ***/

  /*! \fn void whereEqualTo(const char* key, bool value)
   *  \brief add a constraint to the query that requires a particular key's value to be equal to the provided boolean value.
   *
   *  \param key - The key to check.
   *  \param value - The value that the ParseObject must contain.
   */
  void whereEqualTo(const char* key, bool value);

  /*! \fn void whereNotEqualTo(const char* key, bool value)
   *  \brief add a constraint to the query that requires a particular key's value to be equal to the provided string value.
   *
   *  \param key - The key to check.
   *  \param value - The value that must not be equalled.
   */
  void whereNotEqualTo(const char* key, bool value);

  /*** integer number condition ***/

  /*! \fn void whereEqualTo(const char* key, int value)
   *  \brief add a constraint to the query that requires a particular key's value to be equal to the provided string value.
   *
   *  \param key - The key to check.
   *  \param value - The value that the ParseObject must contain.
   */
  void whereEqualTo(const char* key, int value);

  /*! \fn void whereNotEqualTo(const char* key, int value)
   *  \brief add a constraint to the query that requires a particular key's value to be equal to the provided string value.
   *
   *  \param key - The key to check.
   *  \param value - The value that must not be equalled.
   */
  void whereNotEqualTo(const char* key, int value);

  /*! \fn void whereLessThan(const char* key, int value)
   *  \brief add a constraint to the query that requires a particular key's value to be less than the provided value.
   *
   *  \param key - The key to check.
   *  \param value - The value that provides an upper bound.
   */
  void whereLessThan(const char* key, int value);

  /*! \fn void whereGreaterThan(const char* key, int value)
   *  \brief add a constraint to the query that requires a particular key's value to be greater than the provided value.
   *
   *  \param key - The key to check.
   *  \param value - The value that provides an lower bound.
   */
  void whereGreaterThan(const char* key, int value);

  /*! \fn void whereLessThanOrEqualTo(const char* key, int value)
   *  \brief add a constraint to the query that requires a particular key's value to be less than or equal to the provided value.
   *
   *  \param key - The key to check.
   *  \param value - The value that provides an upper bound.
   */
  void whereLessThanOrEqualTo(const char* key, int value);

  /*! \fn void whereGreaterThanOrEqualTo(const char* key, int value)
   *  \brief add a constraint to the query that requires a particular key's value to be greater than or equal to the provided value.
   *
   *  \param key - The key to check.
   *  \param value - The value that provides an lower bound.
   */
  void whereGreaterThanOrEqualTo(const char* key, int value);

  /*** double number condition ***/

  /*! \fn void whereEqualTo(const char* key, double value)
   *  \brief add a constraint to the query that requires a particular key's value to be equal to the provided value.
   *
   *  \param key - The key to check.
   *  \param value - The value that the ParseObject must contain.
   */
  void whereEqualTo(const char* key, double value);

  /*! \fn void whereNotEqualTo(const char* key, double value)
   *  \brief add a constraint to the query that requires a particular key's value not equal to the provided double value.
   *
   *  \param key - The key to check.
   *  \param value - The value that must not be equalled.
   */
  void whereNotEqualTo(const char* key, double value);

  /*! \fn void whereLessThan(const char* key, double value)
   *  \brief add a constraint to the query that requires a particular key's value to be less than the provided value.
   *
   *  \param key - The key to check.
   *  \param value - The value that provides an upper bound.
   */
  void whereLessThan(const char* key, double value);

  /*! \fn void whereGreaterThan(const char* key, double value)
   *  \brief add a constraint to the query that requires a particular key's value to be greater than the provided value.
   *
   *  \param key - The key to check.
   *  \param value - The value that provides an lower bound.
   */
  void whereGreaterThan(const char* key, double value);

  /*! \fn void whereLessThanOrEqualTo(const char* key, double value)
   *  \brief add a constraint to the query that requires a particular key's value to be less than or equal to the provided value.
   *
   *  \param key - The key to check.
   *  \param value - The value that provides an upper bound.
   */
  void whereLessThanOrEqualTo(const char* key, double value);

  /*! \fn void whereGreaterThanOrEqualTo(const char* key, double value)
   *  \brief add a constraint to the query that requires a particular key's value to be greater than or equal to the provided value.
   *
   *  \param key - The key to check.
   *  \param value - The value that provides an lower bound.
   */
  void whereGreaterThanOrEqualTo(const char* key, double value);

  /*! \fn void setLimit(int n)
   *  \brief controls the maximum number of results that are returned.
   *
   *  controls the maximum number of results that are returned.
   *  setting a negative limit denotes retrieval without a limit.
   *  the default limit is 100, with a maximum of 1000 results being returned at a time.
   *
   *  \param n - limit.
   */
  void setLimit(int n);

  /*! \fn void setSkip(int n)
   *  \brief controls the number of results to skip before returning any results.
   *
   *  \param n - number to skip.
   */
  void setSkip(int n);

  /*! \fn void setKeys(const char* keys)
   *  \brief restrict the fields of returned ParseObjects to only include the provided keys.
   *
   *  \param keys - Keys to include. seperate multiple keys with comma
   *                e.g. "score,name"
   */
  void setKeys(const char* keys);

  /*! \fn void orderBy(const char* key)
   *  \brief sorts the results in ascending/descending order by the given key.
   *
   *  \param keys - Keys to sort on.
   *                use "field_name" for ascending order
   *                use a negative sign "-field_name" for descending order
   *                seperate multiple keys with comma e.g.  "score,-name"
   */
  void orderBy(const char* keys);

  /*! \fn ParseResponse send() override
   *  \brief launch query and execute
   *
   *  \result response of the request.
   */
  ParseResponse send() override;
};

#endif
