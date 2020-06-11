#include <unistd.h>
#include <cstdio>
#include <functional>
#include <string>
#include "src/logger/Logger.h"
#include "src/reactor/Channel.h"
#include "src/net/Buffer.h"
#include "src/net/InetAddress.h"
#include "src/net/TcpClient.h"
#include "src/utils/Timestamp.h"
#include "src/utils/Uncopyable.h"

using namespace wethands;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

class EchoClient : public Uncopyable {
 public:
  EchoClient(EventLoop* loop,
             const InetAddress& serverAddr,
             const std::string& name)
      : loop_(loop),
        client_(loop, serverAddr, name),
        channel_(loop, 0) {
    client_.SetConnectionCallback(
      std::bind(&EchoClient::OnConnection, this, _1));
    client_.SetMessageCallback(
      std::bind(&EchoClient::OnMessage, this, _1, _2, _3));
    channel_.SetReadCallback(std::bind(&EchoClient::OnInput, this));
    channel_.EnableReading();
  }
  ~EchoClient() {
    channel_.DisableAll();
    channel_.RemoveFromPoller();
  }

  void Start() {
    client_.Connect();
  }

  void OnConnection(const TcpConnectionPtr& conn) {
    if (conn->IsConnected()) {  // 新连接.
      printf("new connection: %s -> %s\n",
            conn->LocalAddress().ToString(true).c_str(),
            conn->PeerAddress().ToString(true).c_str());
    } else {  // 连接异常断开.
      printf("connection diconnected: %s -> %s\n",
             conn->LocalAddress().ToString(true).c_str(),
             conn->PeerAddress().ToString(true).c_str());
      loop_->Quit();
    }
  }

  void OnMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp when) {
    std::string msg(buffer->RetrieveAllAsString());
    printf("[%s] new message: %s", when.ToFormattedString().c_str(), msg.c_str());
  }

  void OnInput() {
    ssize_t n = ::read(channel_.Fd(), buf, sizeof(buf));
    if (n < 0) {
      LOG_SYSERROR << "OnInput(): read() error.";
    }
    client_.Connection()->Send(buf, static_cast<size_t>(n));
  }

 private:
  EventLoop* loop_;
  TcpClient client_;
  Channel channel_;  // 监听用户的标准输入.
  char buf[512];
};

int main() {
  Logger::SetLogLevel(Logger::LogLevel::NONE);
  EventLoop loop;
  InetAddress serverAddr("127.0.0.1", 7766);
  EchoClient client(&loop, serverAddr, "EchoClient");
  client.Start();
  loop.Loop();
  return 0;
}
