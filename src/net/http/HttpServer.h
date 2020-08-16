// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-08-16 21:46:36
// Description:

#ifndef SRC_NET_HTTP_HTTPSERVER_H_
#define SRC_NET_HTTP_HTTPSERVER_H_

#include <functional>
#include <string>
#include "src/reactor/EventLoop.h"
#include "src/utils/Uncopyable.h"
#include "src/net/InetAddress.h"
#include "src/net/TcpServer.h"
#include "src/net/http/HttpRequest.h"
#include "src/net/http/HttpResponse.h"

namespace wethands {

class HttpServer : public Uncopyable {
 public:
  using HttpCallback = std::function<void (const HttpRequest&,
                                           HttpResponse*)>;

  HttpServer(EventLoop* loop,
             const InetAddress& listenAddr,
             const std::string& name,
             bool reusePort = false);

  EventLoop* GetLoop() const { return server_.OwerLoop(); }
  void SetHttpCallback(const HttpCallback& cb) { httpCallback_ = cb; }
  void Start(int numThreads);

 private:
  void onConnection(const TcpConnectionPtr& conn);
  void onMessage(const TcpConnectionPtr& conn,
                 Buffer* buf,
                 Timestamp receiveTime);
  void handleRequest(const TcpConnectionPtr& conn, const HttpRequest& req);

  TcpServer server_;
  HttpCallback httpCallback_;
};

}  // namespace wethands

#endif  // SRC_NET_HTTP_HTTPSERVER_H_
