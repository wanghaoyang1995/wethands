// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-08-14 17:58:11
// Description:

#ifndef SRC_NET_HTTP_HTTPCONTEXT_H_
#define SRC_NET_HTTP_HTTPCONTEXT_H_

#include "src/utils/Copyable.h"
#include "src/net/Buffer.h"
#include "src/net/http/HttpRequest.h"

namespace wethands {

class HttpContext : public Copyable {
 public:
  enum HttpRequestParseState {
    kExpectRequestLine,
    kExpectHeaders,
    kExpectBody,
    kGotAll
  };

  HttpContext() : state_(kExpectRequestLine) {}
  // 解析请求报文并构造request_. 如果成功返回 true.
  bool ParseRequest(Buffer* buf, Timestamp receiveTime);

  bool GotAll() const { return state_ == kGotAll; }
  void Reset() {
    HttpRequest invalidRequest;
    request_.Swap(invalidRequest);
    state_ = kExpectRequestLine;
  }

  const HttpRequest& GetRequest() const { return request_; }
  HttpRequest& GetRequest() { return request_; }

 private:
  // 解析请求行.
  bool ParseRequestLine(const char* begin, const char* end);

  HttpRequest request_;
  HttpRequestParseState state_;
};

}  // namespace wethands

#endif  // SRC_NET_HTTP_HTTPCONTEXT_H_
