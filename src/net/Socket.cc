// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-03 14:39:42
// Description:

#include "src/net/Socket.h"
#include <errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cassert>
#include <cstring>
#include "src/logger/Logger.h"

using wethands::Socket;

static_assert(sizeof(Socket) == sizeof(int),
              "unexcepted size of Socket.");

Socket::~Socket() {
  CloseFd(sockfd_);
}

ssize_t Socket::Send(const char* msg, size_t len) {
  ssize_t writen = ::write(sockfd_, msg, len);
  if (writen < 0) {
    LOG_SYSERROR << "write() error.";
  }
  return writen;
}

int Socket::Connect(const InetAddress& serverAddr) {
  // 如果套接字是非阻塞的, 会立刻返回. 所以把处理错误留给外层.
  return ::connect(sockfd_, serverAddr.SockAddr(), serverAddr.Len());
}

bool Socket::IsSelfConnect() {
  struct sockaddr_in localAddr = LocalAddress();
  struct sockaddr_in peerAddr = PeerAddress();
  return localAddr.sin_addr.s_addr == peerAddr.sin_addr.s_addr &&
         localAddr.sin_port == peerAddr.sin_port;
}

void Socket::Bind(const InetAddress& localAddr) {
  int ret = ::bind(sockfd_, localAddr.SockAddr(), localAddr.Len());
  if (ret < 0) {
    LOG_SYSFATAL << "bind() error.";
  }
}

void Socket::Listen() {
  // https://www.man7.org/linux/man-pages/man2/listen.2.html
  int ret = ::listen(sockfd_, SOMAXCONN);
  if (ret < 0) {
    LOG_SYSFATAL << "listen() error.";
  }
}

int Socket::Accept(InetAddress* peerAddr) {
  socklen_t len = peerAddr->Len();
  int connfd = ::accept4(sockfd_,
                       peerAddr->SockAddr(),
                       &len,
                       SOCK_NONBLOCK | SOCK_CLOEXEC);
  if (connfd < 0) {
    // https://man7.org/linux/man-pages/man2/accept.2.html
    switch (errno) {
      // 可以解决的错误.
      case EAGAIN:  // 套接字非阻塞且没有连接可以接受. 忽略.
      case ECONNABORTED:  // 已完成三路握手但在 accept 之前收到RST. 忽略.
      case EPROTO:  // 同 ECONNABORTED.
      case EINTR:  // 被信号中断. 等待 poller 重启.
      case EMFILE:  // 单进程可用描述符已用尽. 留给外层处理.
        LOG_SYSERROR << "accept4() error.";
        break;
      // 其他错误, 放弃.
      default:
        LOG_SYSFATAL << "unexpected accept4() error.";
    }
  }
  return connfd;
}

struct sockaddr_in Socket::LocalAddress() const {
  struct sockaddr_in localAddr;
  socklen_t len = sizeof(localAddr);
  int ret = ::getsockname(sockfd_,
                          reinterpret_cast<struct sockaddr*>(&localAddr),
                          &len);
  if (ret < 0) {
    LOG_SYSFATAL << "getsockname() error.";
  }
  return localAddr;
}

struct sockaddr_in Socket::PeerAddress() const {
  struct sockaddr_in peerAddr;
  socklen_t len = sizeof(peerAddr);
  int ret = ::getpeername(sockfd_,
                          reinterpret_cast<struct sockaddr*>(&peerAddr),
                          &len);
  if (ret < 0) {
    LOG_SYSFATAL << "getpeername() error.";
  }
  return peerAddr;
}

void Socket::SetTcpNoDelay(bool on) {
  int opt = on ? 1 : 0;
  int ret = ::setsockopt(sockfd_,
                         IPPROTO_TCP,
                         TCP_NODELAY,
                         &opt,
                         static_cast<socklen_t>(sizeof(opt)));
  if (ret < 0) {
    LOG_SYSERROR << "setsockopt() error.";
  }
}

void Socket::SetReuseAddr(bool on) {
  int opt = on ? 1 : 0;
  int ret = ::setsockopt(sockfd_,
                         SOL_SOCKET,
                         SO_REUSEADDR,
                         &opt,
                         static_cast<socklen_t>(sizeof(opt)));
  if (ret < 0) {
    LOG_SYSERROR << "setsockopt() error.";
  }
}

void Socket::SetReusePort(bool on) {
  int opt = on ? 1 : 0;
  int ret = ::setsockopt(sockfd_,
                         SOL_SOCKET,
                         SO_REUSEPORT,
                         &opt,
                         static_cast<socklen_t>(sizeof(opt)));
  if (ret < 0) {
    LOG_SYSERROR << "setsockopt() error.";
  }
}

void Socket::SetKeepAlive(bool on) {
  int opt = on ? 1 : 0;
  int ret = ::setsockopt(sockfd_,
                         SOL_SOCKET,
                         SO_KEEPALIVE,
                         &opt,
                         static_cast<socklen_t>(sizeof(opt)));
  if (ret < 0) {
    LOG_SYSERROR << "setsockopt() error.";
  }
}

void Socket::ShutdownWrite() {
  if (::shutdown(sockfd_, SHUT_WR) < 0) {
    LOG_SYSERROR << "ShutdownWrite() error.";
  }
}

int Socket::CreateSocketFd() {
  // 创建非阻塞套接字, 使用 ipv4 的 TCP 协议.
  int fd = ::socket(AF_INET,
                    SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                    IPPROTO_TCP);
  if (fd < 0) {
    LOG_SYSFATAL << "CreatSocket() error.";
  }
  return fd;
}

void Socket::CloseFd(int fd) {
  int ret = ::close(fd);
  if (ret < 0) {
    LOG_SYSFATAL << "CloseFd() error.";
  }
}
