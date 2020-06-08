// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-03 14:39:37
// Description:

#ifndef SRC_NET_SOCKET_H_
#define SRC_NET_SOCKET_H_

#include <memory>
#include "src/net/InetAddress.h"
#include "src/utils/Uncopyable.h"

namespace wethands {
// 套接字. 构造时创建, 析构时关闭.
// 可以是主动或被动套接字, 由使用者维护它的正确性.
class Socket : public Uncopyable {
 public:
  Socket() : sockfd_(CreateSocketFd()) {}
  explicit Socket(int sockfd) : sockfd_(sockfd) {}
  ~Socket();

  int Fd() const { return sockfd_; }
  ssize_t Send(const char* msg, size_t len);
  int Connect(const InetAddress& serverAddr);
  // 如果 sockfd_ 是自连接, 返回 true.
  bool IsSelfConnect();
  void Bind(const InetAddress& localAddr);
  void Listen();
  int Accept(InetAddress* peerAddr);

  struct sockaddr_in LocalAddress() const;
  struct sockaddr_in PeerAddress() const;
  void SetTcpNoDelay(bool on);
  void SetReuseAddr(bool on);
  void SetReusePort(bool on);
  void SetKeepAlive(bool on);
  void ShutdownWrite();

  // 创建并返回套接字, 默认是非阻塞的.
  static int CreateSocketFd();
  // 关闭套接字文件描述符. 也可以用来关闭正常描述符.
  static void CloseFd(int fd);

 private:
  const int sockfd_;
};

using SocketPtr = std::unique_ptr<Socket>;

}  // namespace wethands

#endif  // SRC_NET_SOCKET_H_
