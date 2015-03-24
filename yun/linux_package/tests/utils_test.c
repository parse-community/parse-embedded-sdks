#include "../src/utils.h"

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
  DUMP_ERROR_AND_EXIT(x, y, "ASSERT_EQ"); \
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

TEST(Utils)() {
  char json1[] = "{\"val1\":1,\"val2\":\"t2\",\"val3\":{\"val4\":false},\"val4\":true}";
  char json2[] = "{\"val1\":1,\"val2:\"t2\",\"val3\":{\"val4\":true}}";
  char json_array[] = "{\"valarr1\":[],\"valarr2\":[\"t2\",1,true,[false,[]],0]}";
  char malformed_json[] = "}{\"val1\":1,\"val2\":\"t2\",\"val3\":{\"val4\":false},"
  "\"val4\":true}";
  char buffer[512];
  ASSERT_EQ(getValueFromJSON(json1,"", buffer, sizeof(buffer)), 0);
  ASSERT_EQ(getValueFromJSON(json1,"val1", buffer, sizeof(buffer)), 1);
  ASSERT_EQ(strcmp(buffer, "1"), 0);
   ASSERT_EQ(getValueFromJSON(malformed_json,"val1", buffer, sizeof(buffer)), 0);
  ASSERT_EQ(getValueFromJSON(json1,"val2", buffer, sizeof(buffer)), 1);
  ASSERT_EQ(strcmp(buffer, "t2"), 0);
  ASSERT_EQ(getValueFromJSON(json1,"val3", buffer, sizeof(buffer)), 1);
  ASSERT_EQ(strcmp(buffer, "{\"val4\":false}"), 0);
  ASSERT_EQ(getValueFromJSON(json1,"val3", buffer, 4), 1);
  ASSERT_EQ(strcmp(buffer, "{\"v"), 0);
  ASSERT_EQ(getValueFromJSON(json1,"val4", buffer, sizeof(buffer)), 1);
  ASSERT_EQ(strcmp(buffer, "true"), 0);
  ASSERT_EQ(getValueFromJSON(json1,"val5", buffer, sizeof(buffer)), 0);
  ASSERT_EQ(getValueFromJSON(json2,"val2", buffer, sizeof(buffer)), 0);
  ASSERT_EQ(getValueFromJSON(json_array,"valarr1", buffer, sizeof(buffer)), 1);
  ASSERT_EQ(strcmp(buffer, "[]"), 0);
  ASSERT_EQ(getValueFromJSON(json_array,"valarr2", buffer, sizeof(buffer)), 1);
  ASSERT_EQ(strcmp(buffer, "[\"t2\",1,true,[false,[]],0]"), 0);

  char response[] = "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/xml; charset=utf-8\r\n"
    "Content-Length: length\r\n"
    "\r\n"
    "{\"foo\":\"bar\"}";
  char response2[] = "{\"foo\":\"bar\"}\r\n";
  ASSERT_EQ(stripHttpHeader(response, sizeof(response)), 1);
  ASSERT_EQ(strcmp(response, "{\"foo\":\"bar\"}"), 0);
  ASSERT_EQ(stripHttpHeader(response2, sizeof(response2)), 0);

  char json3[] = "{}\r\n{\"foo\":\"bar\"}\r\n{\"foo\":\"bar\"}\r\n";
  char json4[] = "{\"foo\":\"bar\"}\r\n{\"foo\":\"bar\"}\r\n{}\r\n";
  char json5[] = "{\"foo\":\"bar\"}\r\n{}\r\n{\"foo\":\"bar\"}\r\n";
  ASSERT_EQ(stripEmptyJSON(json3, sizeof(json3)), 1);
  ASSERT_EQ(strcmp(json3, "{\"foo\":\"bar\"}\r\n{\"foo\":\"bar\"}\r\n"), 0);
  ASSERT_EQ(stripEmptyJSON(json4, sizeof(json4)), 1);
  ASSERT_EQ(strcmp(json4, "{\"foo\":\"bar\"}\r\n{\"foo\":\"bar\"}\r\n"), 0);
  ASSERT_EQ(stripEmptyJSON(json5, sizeof(json5)), 1);
  ASSERT_EQ(strcmp(json5, "{\"foo\":\"bar\"}\r\n{\"foo\":\"bar\"}\r\n"), 0);

  char json6[] = "{\"foo\":{\"t1\":\"v1\"},\"bar\":1}\r\n{\"foo\":{\"t2\":\"v2\"}}\r\n";
  ASSERT_EQ(getValuesFromJSONs(json6, "foo", buffer, sizeof(buffer)), 1);
  ASSERT_EQ(strcmp(buffer, "{\"t1\":\"v1\"}{\"t2\":\"v2\"}"), 0);

  char json7[] = "[{\"playerName1\":\"Jang Min Chul\"},{\"playerName2\":{\"foo\":\"bar\"}},{\"playerName3\":[{},{},{}]}]";
  ASSERT_EQ(3, getCountFromJSONArray(json7));
  const char* next;
  int nextSize = 0;
  char nextJson[64];
  next = getNextFromJSONArray(json7, json7 + nextSize, &nextSize);
  snprintf(nextJson, nextSize+1, "%s", next);
  ASSERT_EQ(strcmp("{\"playerName1\":\"Jang Min Chul\"}", nextJson), 0);
  next = getNextFromJSONArray(next, next + nextSize, &nextSize);
  snprintf(nextJson, nextSize+1, "%s", next);
  ASSERT_EQ(strcmp("{\"playerName2\":{\"foo\":\"bar\"}}", nextJson), 0);
  next = getNextFromJSONArray(next, next + nextSize, &nextSize);
  snprintf(nextJson, nextSize+1, "%s", next);
  ASSERT_EQ(strcmp("{\"playerName3\":[{},{},{}]}", nextJson), 0);
  
  char str[] = "where={\"str\":\"hello\",\"num\":{\"$lt\":10}}&limit=10&order=name";
  char encodedStr[3 * sizeof(str)];
  urlEncode(str, encodedStr);
  ASSERT_EQ(strcmp(encodedStr, "where=%7B%22str%22%3A%22hello%22%2C%22num%22%3A%7B%22%24lt%22%3A10%7D%7D&limit=10&order=name"), 0);

  char uuid[64];
  getUUID(uuid);
  ASSERT_EQ(strlen(uuid), 36);

  const char* filename = "/tmp/installationId.bin";
  writeId(filename, uuid);
  char id[64];
  readId(filename, id, 64);
  ASSERT_EQ(strcmp(id, uuid), 0);
}


int main(int argc, char* argv[]) {
  return 0;
}
