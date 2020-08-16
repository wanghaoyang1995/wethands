// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-08-16 21:46:45
// Description:

#include "src/net/http/HttpServer.h"
#include <functional>
#include "src/logger/Logger.h"
#include "src/net/Buffer.h"
#include "src/net/http/HttpContext.h"
#include <boost/any.hpp>

using wethands::HttpServer;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

namespace wethands {
namespace detail {

void DefaultHttpCallback(const HttpRequest& req, HttpResponse* resp) {
  resp->SetStatusCode(HttpResponse::k200Ok);
  resp->SetStatusMessage("ok");
  resp->SetCloseConnection(true);
  resp->SetBody("<p>Hello, wethands!</p>\r\n");
}

}  // namespace detail
}  // namespace wethands

HttpServer::HttpServer(EventLoop* loop,
                       const InetAddress& listenAddr,
                       const std::string& name,
                       bool reusePort)
    : server_(loop, listenAddr, name, reusePort),
      httpCallback_(detail::DefaultHttpCallback) {
  server_.SetConnectionCallback(
      std::bind(&HttpServer::onConnection, this, _1));
  server_.SetNewMessageCallback(
      std::bind(&HttpServer::onMessage, this, _1, _2, _3));
}

void HttpServer::Start(int numThreads) {
  LOG_INFO << "HttpServer " << server_.Name()
           << "starts listening on " << server_.IpPort();
  server_.Start(numThreads);
}

void HttpServer::onConnection(const TcpConnectionPtr& conn) {
  if (conn->IsConnected()) {
    conn->SetContext(HttpContext());
  }
}

void HttpServer::onMessage(const TcpConnectionPtr& conn,
                 Buffer* buf,
                 Timestamp receiveTime) {
  HttpContext* context =
    boost::any_cast<HttpContext>(conn->GetMutableContext());
  if (!context->ParseRequest(buf, receiveTime)) {
    LOG_ERROR << "ParseRequest() error";
    conn->Send("HTTP/1.1 400 Bad Request\r\n\r\n", 28);
    conn->Shutdown();
  }

  if (context->GotAll()) {
    handleRequest(conn, context->GetRequest());
    context->Reset();
  }
}

void HttpServer::handleRequest(const TcpConnectionPtr& conn,
                               const HttpRequest& req) {
  const std::string headerConnection = req.GetHeader("Connection");
  bool close = headerConnection == "close" ||
               req.GetVersion() == HttpRequest::kHttp10 &&
               headerConnection != "Keep-Alive";
  HttpResponse response(close);
  // 调用业务逻辑代码.
  httpCallback_(req, &response);
  Buffer buf;
  response.AppendToBuffer(&buf);
  conn->Send(&buf);
  if (response.CloseConnection()) {
    conn->Shutdown();
  }
}
