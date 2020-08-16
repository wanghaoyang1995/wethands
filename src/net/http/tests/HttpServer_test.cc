#include "src/net/http/HttpServer.h"
#include "src/net/InetAddress.h"
#include "src/reactor/EventLoop.h"
#include <cstdio>

using namespace wethands;

void MyHttpCallback(const HttpRequest& req, HttpResponse* resp) {
  resp->SetStatusCode(HttpResponse::k200Ok);
  resp->SetStatusMessage("ok");
  resp->SetCloseConnection(true);
  resp->SetBody("<p>hello</p>\r\n");
}

int main() {
  EventLoop loop;
  InetAddress listenAddr("localhost", 7766);
  HttpServer server(&loop, listenAddr, "http server");
  server.SetHttpCallback(MyHttpCallback);
  server.Start(4);
  loop.Loop();
  return 0;
}
