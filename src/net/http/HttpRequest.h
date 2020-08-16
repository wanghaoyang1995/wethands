// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-08-14 11:19:04
// Description:

#ifndef SRC_NET_HTTP_HTTPREQUEST_H_
#define SRC_NET_HTTP_HTTPREQUEST_H_

#include <cassert>
#include <cctype>
#include <string>
#include <map>
#include <utility>

#include "src/utils/Copyable.h"
#include "src/utils/Timestamp.h"

namespace wethands {
// http 请求.
class HttpRequest : public Copyable {
 public:
  enum Method {
    kInvalid = 0,
    kGet,
    kPost,
    kHead,
    kPut,
    kDelete,
    NUM_Method
  };
  static const char* MethodNames[NUM_Method];

  enum Version {
    kUnknown = 0,
    kHttp10,
    kHttp11,
    NUM_Version
  };
  static const char* VersionNames[NUM_Version];

  HttpRequest() : method_(kInvalid), version_(kUnknown) {}
  bool SetVersion(const char* start, const char* end) {
    const std::string v(start, end);
    return SetVersion(v);
  }
  bool SetVersion(const std::string& v) {
    assert(version_ == kUnknown);
    for (int i = 1; i < NUM_Version; ++i) {
      if (v == VersionNames[i]) {
        version_ = static_cast<Version>(i);
        break;
      }
    }
    return version_ == kUnknown;
  }

  bool SetMethod(const char* start, const char* end) {
    const std::string m(start, end);
    return SetMethod(m);
  }
  bool SetMethod(const std::string& m) {
    assert(method_ == kInvalid);
    for (int i = 1; i < NUM_Method; ++i) {
      if (m == MethodNames[i]) {
        method_ = static_cast<Method>(i);
        break;
      }
    }
    return method_ != kInvalid;
  }
  void SetPath(const char* start, const char* end) {
    path_.assign(start, end);
  }
  void SetPath(const std::string& p) {
    path_ = p;
  }
  void SetQuery(const char* start, const char* end) {
    query_.assign(start, end);
  }
  void SetQuery(const std::string& q) {
    query_ = q;
  }
  void SetBody(const char* start, const char* end) {
    body_.assign(start, end);
  }
  void SetBody(const std::string& b) {
    body_ = b;
  }
  void SetReceiveTime(Timestamp t) { receiveTime_ = t; }

  Version GetVersion() const { return version_; }
  Method GetMethod() const { return method_; }
  const char* MethodString() const {
    return MethodNames[method_];
  }
  const char* VersionString() const {
    return VersionNames[version_];
  }
  const std::string& GetPath() const { return path_; }
  const std::string& GetQuery() const { return query_; }
  const std::string& GetBody() const { return body_; }
  Timestamp GetReceiveTime() const { return receiveTime_; }

  // 输入的三个参数分别为字符串起始位置、冒号位置、终止位置.
  void AddHeader(const char* start, const char* colon, const char* end) {
    assert(*colon == ':');
    std::string field(start, colon);
    ++colon;
    while (colon != end && isspace(*colon)) ++colon;
    std::string value(colon, end);
    while (!value.empty() & isspace(*value.rbegin())) {
      value.pop_back();
    }
    headers_[field] = value;
  }
  void AddHeader(const std::string& key, const std::string& value) {
    headers_[key] = value;
  }
  std::string GetHeader(const std::string& field) const {
    std::map<std::string, std::string>::const_iterator it =
      headers_.find(field);
    return it == headers_.end() ? std::string() : it->second;
  }

  const std::map<std::string, std::string>& GetHeaders() const {
    return headers_;
  }

  void Swap(HttpRequest& that) {
    std::swap(method_, that.method_);
    std::swap(version_, that.version_);
    path_.swap(that.path_);
    query_.swap(that.query_);
    body_.swap(that.body_);
    receiveTime_.Swap(that.receiveTime_);
    headers_.swap(that.headers_);
  }

 private:
  Method method_;  // http 方法
  Version version_;  // http 版本
  std::string path_;  // 请求资源路径
  std::string query_;  // 查询字符串
  std::string body_;  // 请求体
  Timestamp receiveTime_;
  std::map<std::string, std::string> headers_;  // 首部行
};

}  // namespace wethands

#endif  // SRC_NET_HTTP_HTTPREQUEST_H_
