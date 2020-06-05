// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-03 14:39:37
// Description:

#ifndef SRC_NET_SOCKET_H_
#define SRC_NET_SOCKET_H_

#include "src/net/InetAddress.h"
#include "src/utils/Uncopyable.h"

namespace wethands {
// 套接字的包装.
// 可以是主动或被动套接字, 由使用者维护它的正确性.
class Socket : public Uncopyable {
 public:
  explicit Socket(int sockfd) : sockfd_(sockfd) {}
  ~Socket();

  int Fd() const { return sockfd_; }
  ssize_t Send(const char* msg, size_t len);
  int Connect(const InetAddress& serverAddr);
  void Bind(const InetAddress& localAddr);
  void Listen();
  int Accept(InetAddress* peerAddr);

  void SetTcpNoDelay(bool on);
  void SetReuseAddr(bool on);
  void SetReusePort(bool on);
  void SetKeepAlive(bool on);

  void ShutdownWrite();

 private:
  const int sockfd_;
};

}  // namespace wethands

#endif  // SRC_NET_SOCKET_H_
