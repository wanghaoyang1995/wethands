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
                                            const InetAddress& addr) {
    printf("new connection: fd = %d, %s\n", connSocket->Fd(), addr.ToString(true).c_str());
    //connSocket.reset();
    loop.Quit();
  });
  acceptor.Listen();
  loop.Loop();
  return 0;
}
