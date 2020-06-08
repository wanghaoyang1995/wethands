#include "src/net/Acceptor.h"
#include "src/net/InetAddress.h"
#include "src/reactor/EventLoop.h"

#include <unistd.h>
#include <cstdio>

using namespace wethands;

int main() {
  EventLoop loop;
  InetAddress listenAddr("localhost", 7766);
  Acceptor acceptor(&loop, listenAddr, true);
  acceptor.SetNewConnectionCallback([&loop](int connfd, const InetAddress& addr){
    printf("new connection: fd = %d, %s\n", connfd, addr.ToString(true).c_str());
    ::close(connfd);
    loop.Quit();
  });
  acceptor.Listen();
  loop.Loop();
  return 0;
}
