// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-08-14 17:58:23
// Description:

#include "HttpContext.h"
#include <algorithm>
#include <regex>

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
            hasMore = false;
            ok = false;
        }
      } else if (state_ == kExpectHeaders) {
        // 处理首部行.
        const char* crlf = buf->FindCRLF();
        if (crlf) {
          const char* colon = std::find(buf->Peek(), crlf, ':');
          if (colon != crlf) {  // 找到了冒号.
            request_.AddHeader(buf->Peek(), colon, crlf);
          } else {  // 未找到冒号, 说明首部行结束.
            buf->Retrieve(2);
            state_ = kExpectBody;
          }
          buf->Retrieve(2);
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
  std::regex reg("([A-Z]+)\\s([0-9a-zA-Z/\\.]+)[\\?]?([%-=&\\w]*)\\sHTTP/1\\.([01])\\r\\n");
  std::cmatch m;
  bool ok = std::regex_match(begin, end, m, reg);
  if (ok) {
    request_.SetMethod(m[0].str());
    request_.SetPath(m[1].str());
    request_.SetQuery(m[2].str());
    request_.SetVersion(m[3].str());
  }
  return ok;
}
