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
    const char* request =
      "GET / HTTP/1.1\r\n"
      "Host: 172.29.248.132:7766\r\n"
      "Connection: keep-alive\r\n"
      "Cache-Control: max-age=0\r\n"
      "Upgrade-Insecure-Requests: 1\r\n"
      "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/84.0.4147.125 Safari/537.36 Edg/84.0.522.59\r\n"
      "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n"
      "Accept-Encoding: gzip, deflate\r\n"
      "Accept-Language: zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6\r\n"
      "\r\n";
    buf.Append(request, strlen(request));
  }
  // void TearDown() override {}
  Buffer buf;
};

TEST_F(HttpContextTest, ParseTest) {
  HttpContext context;
  ASSERT_TRUE(context.ParseRequest(&buf, Timestamp::Now()));
  ASSERT_TRUE(context.GetRequest().GetMethod() == HttpRequest::kGet);
  ASSERT_TRUE(context.GetRequest().GetVersion() == HttpRequest::kHttp11);
  ASSERT_STREQ(context.GetRequest().GetPath().c_str(), "/");
  ASSERT_STREQ(context.GetRequest().GetQuery().c_str(), "");
  ASSERT_TRUE(context.GetRequest().GetHeader("Host") == "172.29.248.132:7766");
  ASSERT_TRUE(context.GetRequest().GetHeaders().size() == 8);
  ASSERT_TRUE(context.GetRequest().GetBody() == "");
}

}
}  // namespace wethands
