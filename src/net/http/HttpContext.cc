// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-08-14 17:58:23
// Description:

#include "HttpContext.h"
#include <algorithm>
#include <regex>
#include "src/logger/Logger.h"

using wethands::HttpContext;

bool HttpContext::ParseRequest(Buffer* buf, Timestamp receiveTime) {
  bool ok = true;
  bool hasMore = true;
  while (hasMore) {
      if (state_ == kExpectRequestLine) {
        // 处理请求行.
        const char* crlf = buf->FindCRLF();
        if (crlf && ParseRequestLine(buf->Peek(), crlf)) {
          request_.SetReceiveTime(receiveTime);
          buf->RetrieveUntil(crlf + 2);
          state_ = kExpectHeaders;
        } else {  // 解析失败.
          LOG_ERROR << "ParseRequest(): Request line parsing error.";
          hasMore = false;
          ok = false;
        }
      } else if (state_ == kExpectHeaders) {
        // 处理首部行.
        std::regex reg("([^:]+):\\s*(.+)");
        const char* crlf = buf->FindCRLF();
        if (crlf) {
          std::cmatch m;
          bool match = std::regex_match(buf->Peek(), crlf, m, reg);
          if (match) {
            request_.AddHeader(m[1].str(), m[2].str());
            buf->RetrieveUntil(crlf + 2);
          } else {  // 匹配失败, 首部行结束或格式错误.
            if (buf->Peek() == crlf) {  // 首部行结束.
              buf->RetrieveUntil(crlf + 2);
              state_ = kExpectBody;
            } else {  // 首部行格式错误.
              LOG_ERROR << "ParseRequest(): Header line parsing error.";
              ok = false;
              hasMore = false;
            }
          }
        } else {
            hasMore = false;
        }
      } else if (state_ == kExpectBody) {
        request_.SetBody(buf->Peek(), buf->BeginWrite());
        state_ = kGotAll;
        hasMore = false;
      }
  }
  return ok;
}

bool HttpContext::ParseRequestLine(const char* begin, const char* end) {
  std::regex reg("(GET|POST|HEAD|PUT|DELETE)\\s*"
                 "([\\w/\\.]+)[\\?]?([%-=&\\w]*)\\s*"
                 "(HTTP/1\\.[01])");
  std::cmatch m;
  bool ok = std::regex_match(begin, end, m, reg);
  if (ok) {
    request_.SetMethod(m[1].str());
    request_.SetPath(m[2].str());
    request_.SetQuery(m[3].str());
    request_.SetVersion(m[4].str());
  } else {
    LOG_ERROR << "ParseRequestLine error.";
  }
  return ok;
}
