#include "src/net/Connector.h"
#include "src/net/InetAddress.h"
#include "src/reactor/EventLoop.h"
#include <cstdio>
using namespace wethands;

int main() {
  EventLoop loop;
  InetAddress servAddr("127.0.0.1", 7766);
  Connector connector(&loop, servAddr);
  connector.SetNewConnectionCallback([&loop](SocketPtr connSocket,
                                             const InetAddress& serverAddr){
    printf("fd: %d, serverAddr: %s.\n", connSocket->Fd(), serverAddr.ToString(true).c_str());
    loop.Quit();
  });
  connector.Start();
  loop.Loop();
  return 0;
}
