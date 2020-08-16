// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-08-16 18:37:49
// Description:

#include "src/net/http/HttpResponse.h"
#include <cstdio>
#include <cstring>
#include <utility>

using wethands::HttpResponse;

void HttpResponse::AppendToBuffer(Buffer* output) const {
  char buf[32];
  snprintf(buf, sizeof(buf), "HTTP/1.1 %d ", statusCode_);
  output->Append(buf, strlen(buf));
  output->Append(statusMessage_);
  output->Append("\r\n", 2);

  if (closeConnection_) {
      output->Append("Connection: close\r\n");
  } else {
    snprintf(buf, sizeof(buf), "Content-Length: %zd\r\n", body_.size());
    output->Append(buf, strlen(buf));
    output->Append("Connection: Keep-Alive\r\n", 24);
  }

  for (const std::pair<std::string, std::string>& p : headers_) {
    output->Append(p.first);
    output->Append(": ", 2);
    output->Append(p.second);
    output->Append("\r\n", 2);
  }
  output->Append("\r\n", 2);
  output->Append(body_);
}
