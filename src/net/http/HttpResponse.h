// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-08-16 18:37:38
// Description:

#ifndef SRC_NET_HTTP_HTTPRESPONSE_H_
#define SRC_NET_HTTP_HTTPRESPONSE_H_

#include <map>
#include <string>
#include "src/net/Buffer.h"
#include "src/utils/Copyable.h"

namespace wethands {
// Http 响应.
class HttpResponse : public Copyable {
 public:
  enum HttpStatusCode {
    kUnknown,
    k200Ok = 200,
    k301MovedPermanently = 301,
    k400BadRequest = 400,
    k404NotFound = 404
  };

  explicit HttpResponse(bool close = false)
      : statusCode_(kUnknown),
        closeConnection_(close) {}

  void SetStatusCode(HttpStatusCode code) { statusCode_ = code; }
  void SetStatusMessage(const std::string& message) {
    statusMessage_ = message;
  }
  void SetCloseConnection(bool on) { closeConnection_ = on; }
  bool CloseConnection() const { return closeConnection_; }
  void AddHeader(const std::string& key, const std::string& value) {
    headers_[key] = value;
  }
  void SetBody(const std::string& body) { body_ = body; }
  void AppendToBuffer(Buffer* output) const;

 private:
  HttpStatusCode statusCode_;
  std::string statusMessage_;
  bool closeConnection_;
  std::map<std::string, std::string> headers_;
  std::string body_;
};

}  // namespace wethands

#endif  // SRC_NET_HTTP_HTTPRESPONSE_H_
