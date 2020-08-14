// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-08-14 17:58:11
// Description:

#ifndef SRC_NET_HTTP_HTTPCONTEXT_H_
#define SRC_NET_HTTP_HTTPCONTEXT_H_

#include "src/utils/Copyable.h"
#include "src/net/http/HttpRequest.h"

namespace wethands {

class HttpContext : public Copyable {
 public:
  enum RequestParseState {

  };

  HttpContext() {}

 private:
  HttpRequest request_;
};

}  // namespace wethands

#endif  // SRC_NET_HTTP_HTTPCONTEXT_H_
