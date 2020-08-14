// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-08-14 15:42:53
// Description:

#include "src/net/http/HttpRequest.h"

using wethands::HttpRequest;

const char* HttpRequest::MethodNames[] = {
  "INVALID",
  "GET",
  "POST",
  "HEAD",
  "PUT",
  "DELETE"
};

const char* HttpRequest::VersionNames[] = {
  "UNKNOWN",
  "HTTP/1.0",
  "HTTP/1.1"
};
