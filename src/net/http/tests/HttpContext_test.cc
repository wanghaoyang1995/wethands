#include "src/net/http/HttpContext.h"
#include "src/net/Buffer.h"

#include <cstring>
#include "gtest/gtest.h"

namespace wethands {
namespace {

class HttpContextTest : public ::testing::Test {
 protected:
  void SetUp() override {
    HttpContext context;
    const char* request = "GET /index.html?a=10&&b=5 HTTP/1.1\r\n"
                          "Host: www.baidu.com\r\n"
                          "\r\n";
    buf.Append(request, strlen(request));
  }
  // void TearDown() override {}
  Buffer buf;
};

TEST_F(HttpContextTest, ParseTest) {
  HttpContext context;
  context.ParseRequest(&buf, Timestamp::Now());
  ASSERT_TRUE(context.GetRequest().GetMethod() == HttpRequest::kGet);
  ASSERT_TRUE(context.GetRequest().GetVersion() == HttpRequest::kHttp11);
  ASSERT_STREQ(context.GetRequest().GetPath().c_str(), "/index.html");
  ASSERT_STREQ(context.GetRequest().GetQuery().c_str(), "a=10&&b=5");
}

}
}  // namespace wethands
