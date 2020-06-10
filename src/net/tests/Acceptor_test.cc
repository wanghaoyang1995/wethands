#include "src/net/Acceptor.h"
#include "src/net/InetAddress.h"
#include "src/net/Socket.h"
#include "src/reactor/EventLoop.h"
#include <unistd.h>
#include <cstdio>

using namespace wethands;

int main() {
  EventLoop loop;
  InetAddress listenAddr("localhost", 7766);
  Acceptor acceptor(&loop, listenAddr, true);
  acceptor.SetNewConnectionCallback([&loop](SocketPtr connSocket,
                                            const InetAddress& clientAddr) {
    InetAddress localAddr(connSocket->LocalAddress());
    printf("fd: %d, clientAddr: %s, localAddr: %s\n",
           connSocket->Fd(),
           clientAddr.ToString(true).c_str(),
           localAddr.ToString(true).c_str());
    //connSocket.reset();
    //loop.Quit();
  });
  acceptor.Listen();
  loop.Loop();
  return 0;
}
