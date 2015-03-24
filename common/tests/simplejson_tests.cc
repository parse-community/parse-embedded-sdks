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

#include "../simplejson.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

std::string getStrValue(const char *s) {
   return s ? std::string("NULL") : std::string(s);
}

std::string getStrValue(int i) {
   char buff[16];
   snprintf(buff, 16, "%d", i);
   return std::string(buff);
}

#define DUMP_ERROR_AND_EXIT(x, y, request) \
  printf("Failed %s(%s, %s) actual \"%s\" at line %d\n", request, # x, # y, getStrValue(x).c_str(), __LINE__); \
  exit(1)

#define ASSERT_EQ(x, y) \
  if ((x) != (y)) { \
    DUMP_ERROR_AND_EXIT(x, y, "ASSERT_EQ");                 \
  }

#define TEST(x) \
class test ## x { \
public: \
test ## x() { \
  printf("Running TEST %s\n", #x); \
  x(); \
  printf("success!\n"); \
} \
void x(); \
}; \
test ## x test ## x ## temp; \
void test ## x::x

TEST(JSONParser)() {
  const char* test1 = "{\"val1\":1,\"val2\":\"t2\",\"val3\":{\"val4\":false},"
                      "\"val4\":true}";
  const char* test1a = "}{\"val1\":1,\"val2\":\"t2\",\"val3\":{\"val4\":false},"
                      "\"val4\":true}";
  char buffer[512];
  const char* test2 = "{\"val1\":1,\"val2:\"t2\",\"val3\":{\"val4\":true}}";
  const char* test3 = "{\"valarr1\":[],\"valarr2\":[\"t2\",1,true,[false,[]],0]}";

  ASSERT_EQ(simpleJsonProcessor(test1,"", buffer, sizeof(buffer)), 0);
  ASSERT_EQ(simpleJsonProcessor(test1,"val1", buffer, sizeof(buffer)), 1);
  ASSERT_EQ(strcmp(buffer, "1"), 0);
  ASSERT_EQ(simpleJsonProcessor(test1a,"val1", buffer, sizeof(buffer)), 0);
  ASSERT_EQ(simpleJsonProcessor(test1,"val2", buffer, sizeof(buffer)), 1);
  ASSERT_EQ(strcmp(buffer, "t2"), 0);
  ASSERT_EQ(simpleJsonProcessor(test1,"val3", buffer, sizeof(buffer)), 1);
  ASSERT_EQ(strcmp(buffer, "{\"val4\":false}"), 0);
  ASSERT_EQ(simpleJsonProcessor(test1,"val3", buffer, 4), 1);
  ASSERT_EQ(strcmp(buffer, "{\"v"), 0);
  ASSERT_EQ(simpleJsonProcessor(test1,"val4", buffer, sizeof(buffer)), 1);
  ASSERT_EQ(strcmp(buffer, "true"), 0);
  ASSERT_EQ(simpleJsonProcessor(test1,"val5", buffer, sizeof(buffer)), 0);
  ASSERT_EQ(simpleJsonProcessor(test2,"val2", buffer, sizeof(buffer)), 0);
  ASSERT_EQ(simpleJsonProcessor(test3,"valarr1", buffer, sizeof(buffer)), 1);
  ASSERT_EQ(strcmp(buffer, "[]"), 0);
  ASSERT_EQ(simpleJsonProcessor(test3,"valarr2", buffer, sizeof(buffer)), 1);
  ASSERT_EQ(strcmp(buffer, "[\"t2\",1,true,[false,[]],0]"), 0);
}

TEST(findSecondJsonTest)() {
  const char* test1 = "{\"val1\":1,\"val2\":\"t2\",\"val3\":{\"val4\":false},"
                      "\"val4\":true}{\"second\":true}  {\third\":1}";
  const char* test2 = "{\"val1\":1,\"val2\":\"t2\",\"val3\":{\"val4\":false},"
                      "\"val4\":true}";
  const char* test3 = "";

  const char* result = findSecondJson(test1);
  ASSERT_EQ(strcmp(result, "{\"second\":true}  {\third\":1}"), 0);
  result = findSecondJson(result);
  ASSERT_EQ(strcmp(result, "{\third\":1}"), 0);
  result = findSecondJson(result);
  ASSERT_EQ(result, NULL);
  result = findSecondJson(test2);
  ASSERT_EQ(result, NULL);
  result = findSecondJson(test3);
  ASSERT_EQ(result, NULL);
}

TEST(isCompleteJsonTest)() {
  const char* test1 = "{\"val1\":1,\"val2\":\"t2\",\"val3\":{\"val4\":false},"
                      "\"val4\":true}{\"second\":true}  {\third\":1}";
  const char* test2 = "{\"val1\":1,\"val2\":\"t2\",\"val3\":{\"val4\":false},"
                      "\"val4\":true";
  const char* test3 = "";
  const char* test4 = "{}";
  ASSERT_EQ(isCompleteJson(test1), 1);
  ASSERT_EQ(isCompleteJson(test2), 0);
  ASSERT_EQ(isCompleteJson(test3), 0);
  ASSERT_EQ(isCompleteJson(test4), 1);
}

TEST(getPushJsonTest)() {
  const char test1[] = "...\r\n...{}\r\n{\"v1\":true}{\"v2\":\"Hello\",\"v2\":1}";
  const char test2[] = "  ";

  int start = 0;
  int length = 0;
  const char *data = getPushJson(test1, sizeof(test1) - 1, &start, &length);
  ASSERT_EQ(strncmp(data, "{}", 2), 0);
  ASSERT_EQ(start, 8);
  ASSERT_EQ(length, 2);

  int start2 = 0;
  int length2 = 0;
  data = getPushJson(test1 + (start + length),
                     sizeof(test1) - 1 - (start + length),
                     &start2, &length2);
  ASSERT_EQ(strncmp(data, "{\"v1\":true}", 11), 0);
  ASSERT_EQ(start2, 2);
  ASSERT_EQ(length2, 11);

  int start3 = 0;
  int length3 = 0;
  data = getPushJson(test1 + (start + length) + (start2 + length2),
                     11,
                     &start3, &length3);
  ASSERT_EQ(strncmp(data, "{\"v2\":\"Hell", 11), 0);
  ASSERT_EQ(start3, 0);
  ASSERT_EQ(length3, -1);

  data = getPushJson(test1 + (start + length) + (start2 + length2),
                     sizeof(test1) - 1 - (start + length  + start2 + length2),
                     &start3, &length3);
  ASSERT_EQ(strncmp(data, "{\"v2\":\"Hello\",\"v2\":1}", 21), 0);
  ASSERT_EQ(start3, 0);
  ASSERT_EQ(length3, 21);

  data = getPushJson(test2, sizeof(test2), &start, &length);
  ASSERT_EQ(data, NULL);
  ASSERT_EQ(start, -1);
  ASSERT_EQ(length, -1);
}

int main(int argc, char* argv[]) {
  return 0;
}
