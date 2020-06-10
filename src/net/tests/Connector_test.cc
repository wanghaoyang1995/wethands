#include "src/net/Connector.h"
#include "src/net/InetAddress.h"
#include "src/reactor/EventLoop.h"
#include "src/logger/Logger.h"
#include <cstdio>
using namespace wethands;

int main() {
  //Logger::SetLogLevel(Logger::LogLevel::DEBUG);
  EventLoop loop;
  InetAddress servAddr("127.0.0.1", 7766);
  ConnectorPtr connector(new Connector(&loop, servAddr));
  connector->SetNewConnectionCallback([&loop](SocketPtr connSocket,
                                             const InetAddress& serverAddr){
    InetAddress localAddr(connSocket->LocalAddress());
    printf("fd: %d, serverAddr: %s, localAddr: %s.\n",
           connSocket->Fd(),
           serverAddr.ToString(true).c_str(),
           localAddr.ToString(true).c_str());
    //loop.Quit();
  });
  connector->Start();
  //loop.RunAfter(1.0, std::bind(&Connector::Stop, connector));
  loop.RunAfter(2.0, std::bind(&Connector::Restart, connector));  // 第二次调用.

  loop.Loop();
  return 0;
}
