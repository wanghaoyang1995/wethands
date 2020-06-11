#include "src/logger/Logger.h"
#include "src/net/Buffer.h"
#include "src/net/TcpServer.h"
#include "src/net/InetAddress.h"
#include "src/reactor/EventLoop.h"
#include "src/utils/Timestamp.h"
#include "src/utils/Uncopyable.h"

using namespace wethands;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

class EchoServer : public Uncopyable {
 public:
  EchoServer(EventLoop* loop,
             const InetAddress& listenAddr,
             const std::string& name)
      : loop_(loop),
        server_(loop, listenAddr, name, false) {
    server_.SetConnectionCallback(
      std::bind(&EchoServer::OnConnection, this, _1));
    server_.SetNewMessageCallback(
      std::bind(&EchoServer::OnMessage, this, _1, _2, _3));
  }

  void Start(int numThreads) {
    server_.Start(numThreads);
  }

  void OnConnection(const TcpConnectionPtr& conn) {
    if (conn->IsConnected()) {  // 新连接.
      conn->Send("hello\n");;
      printf("new connection: %s -> %s\n",
            conn->LocalAddress().ToString(true).c_str(),
            conn->PeerAddress().ToString(true).c_str());
    } else {  // 已有连接断开.
      printf("connection diconnected: %s -> %s\n",
             conn->LocalAddress().ToString(true).c_str(),
             conn->PeerAddress().ToString(true).c_str());
    }
  }

  void OnMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp when) {
    std::string msg(buffer->RetrieveAllAsString());
    printf("[%s] new message: %s", when.ToFormattedString().c_str(), msg.c_str());
    if (msg == "quit") {
      conn->Shutdown();
    } else {
      conn->Send(msg);
    }
  }

 private:
  EventLoop* loop_;
  TcpServer server_;
};

int main() {
  Logger::SetLogLevel(Logger::LogLevel::NONE);
  EventLoop loop;
  InetAddress listenAddr(7766);
  EchoServer echoServer(&loop, listenAddr, "EchoServer");
  echoServer.Start(4);

  loop.Loop();
  return 0;
}
