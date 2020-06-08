#include "src/net/Connector.h"
#include "src/net/InetAddress.h"
#include "src/reactor/EventLoop.h"
#include <cstdio>
using namespace wethands;

int main() {
  EventLoop loop;
  InetAddress servAddr("localhost", 7766);
  Connector connector(&loop, servAddr);
  printf("servAddr: %s\n", servAddr.ToString(true).c_str());
  connector.SetNewConnectionCallback([&loop](int connfd){
    printf("fd: %d, new connection.\n", connfd);
    loop.Quit();
  });
  connector.Start();
  loop.Loop();
  return 0;
}
