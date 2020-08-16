#include "src/net/http/HttpResponse.h"
#include <cstdio>
#include "gtest/gtest.h"

namespace wethands {
namespace {

class HttpResponseTest : public ::testing::Test {
 protected:
  void SetUp() override {
    response.SetStatusCode(HttpResponse::k200Ok);
    response.SetStatusMessage("ok");
    response.SetCloseConnection(true);
    response.AddHeader("Content-Type", "text/html");
    response.SetBody("<p>你好！</p>\r\n");
    response.AppendToBuffer(&buf1);
    response.SetCloseConnection(false);
    response.AppendToBuffer(&buf2);
  }
  // void TearDown() override {}

  HttpResponse response;
  Buffer buf1, buf2;
};

TEST_F(HttpResponseTest, test) {
  std::string str1 = buf1.RetrieveAllAsString();
  printf("buf1:\n%s", str1.c_str());
  ASSERT_STREQ(str1.c_str(),
    "HTTP/1.1 200 ok\r\n"
    "Connection: close\r\n"
    "Content-Type: text/html\r\n"
    "\r\n"
    "<p>你好！</p>\r\n");
  std::string str2 = buf2.RetrieveAllAsString();
  printf("buf2:\n%s", str2.c_str());
  ASSERT_STREQ(str2.c_str(),
    "HTTP/1.1 200 ok\r\n"
    "Content-Length: 18\r\n"
    "Connection: Keep-Alive\r\n"
    "Content-Type: text/html\r\n"
    "\r\n"
    "<p>你好！</p>\r\n");
}

}  // namespace
}  // namespace wethands
