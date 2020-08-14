#include "src/net/http/HttpRequest.h"
#include "gtest/gtest.h"

namespace wethands {
namespace {

class HttpRequestTest : public ::testing::Test {
 protected:
  void SetUp() override {
    request1.SetMethod(request_message, request_message + 3);
    request1.SetPath(request_message + 4, request_message + 22);
    request1.SetVersion(request_message + 23, request_message + 31);
    request1.SetReceiveTime(Timestamp(9999));
    request1.AddHeader(
      request_message + 33, request_message + 39, request_message + 47);
    request1.AddHeader(
      request_message + 49, request_message + 53, request_message + 76);
  }
  // void TearDown() override {}

  const char* request_message =
    "GET /test/hi-there.txt HTTP/1.1\r\n"
    "Accept: test/*\r\n"
    "Host: www.joes-hardware.com\r\n\r\n";
  HttpRequest request1, request2;
};

TEST_F(HttpRequestTest, SetGetTest) {
  ASSERT_STREQ(request1.MethodString(), "GET");
  ASSERT_STREQ(request1.GetPath().c_str(), "/test/hi-there.txt");
  ASSERT_STREQ(request1.GetQuery().c_str(), "");
  ASSERT_STREQ(request1.VersionString(), "HTTP/1.1");
  ASSERT_TRUE(request1.GetReceiveTime() == Timestamp(9999));

  ASSERT_STREQ(request2.MethodString(), "INVALID");
  ASSERT_STREQ(request2.VersionString(), "UNKNOWN");
}

TEST_F(HttpRequestTest, SwapTest) {
  request1.Swap(request2);
  ASSERT_STREQ(request2.MethodString(), "GET");
  ASSERT_STREQ(request2.GetPath().c_str(), "/test/hi-there.txt");
  ASSERT_STREQ(request2.GetQuery().c_str(), "");
  ASSERT_STREQ(request2.VersionString(), "HTTP/1.1");
  ASSERT_TRUE(request2.GetReceiveTime() == Timestamp(9999));

  ASSERT_STREQ(request1.MethodString(), "INVALID");
  ASSERT_STREQ(request1.VersionString(), "UNKNOWN");
}

}
}  // namespace wethands
