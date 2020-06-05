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
  if (::close(sockfd_) < 0) {
    LOG_SYSFATAL << "close() error.";
  }
}

ssize_t Socket::Send(const char* msg, size_t len) {
  ssize_t writen = ::write(sockfd_, msg, len);
  if (writen < 0) {
    LOG_SYSERROR << "write() error.";
  }
  return writen;
}

int Socket::Connect(const InetAddress& serverAddr) {
  int ret = ::connect(sockfd_, serverAddr.SockAddr(), serverAddr.Len());
  if (ret < 0) {
    LOG_ERROR << "connect() error.";
  }
  return ret;
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
      case EAGAIN:
      case ECONNABORTED:
      case EINTR:
      case EMFILE:
        LOG_SYSERROR << "accept4() error.";
        break;
      default:
        LOG_SYSFATAL << "unexpected accept4() error.";
    }
  }
  return connfd;
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
