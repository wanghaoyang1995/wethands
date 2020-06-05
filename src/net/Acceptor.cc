// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-04 10:26:39
// Description:

#include "src/net/Acceptor.h"
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cassert>
#include <functional>
#include "src/logger/Logger.h"

namespace wethands {
namespace details {

int CreateSocket() {
  // 创建套接字, 使用 ipv4 的 TCP 协议.
  int fd = ::socket(AF_INET,
                    SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                    IPPROTO_TCP);
  if (fd < 0) {
    LOG_SYSFATAL << "CreatSocket() error.";
  }
  return fd;
}

void CloseFd(int fd) {
  int ret = ::close(fd);
  if (ret < 0) {
    LOG_SYSFATAL << "CloseFd() error.";
  }
}

}  // namespace details
}  // namespace wethads

using wethands::Acceptor;

Acceptor::Acceptor(EventLoop* loop,
                   const InetAddress& listenAddr,
                   bool reusePort)
    : loop_(loop),
      listening_(false),
      listenSocket_(details::CreateSocket()),
      listenSocketChannel_(loop, listenSocket_.Fd()),
      newConnectionCallback_(),
      placeholderFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC)) {
  listenSocket_.SetReuseAddr(true);
  listenSocket_.SetReusePort(true);
  listenSocket_.Bind(listenAddr);
  listenSocketChannel_.SetReadCallback(std::bind(&Acceptor::HandleRead, this));
}

Acceptor::~Acceptor() {
  listenSocketChannel_.DisableAll();
  listenSocketChannel_.RemoveFromPoller();
  details::CloseFd(placeholderFd_);
}

void Acceptor::Listen() {
  assert(loop_->IsInLoopThread());
  listenSocket_.Listen();
  listenSocketChannel_.EnableReading();
  listening_ = true;
}

void Acceptor::HandleRead() {
  // 有新的连接请求到达.
  assert(loop_->IsInLoopThread());
  InetAddress peerAddr;
  int connfd = listenSocket_.Accept(&peerAddr);
  if (connfd >= 0) {
    if (newConnectionCallback_) {
      newConnectionCallback_(connfd, peerAddr);
    } else {
      details::CloseFd(connfd);
    }
  } else {
    LOG_SYSERROR << "Socket::Accept() error.";
    if (errno == EMFILE) {  // 描述符用尽.
      // 连接后马上断开.
      details::CloseFd(placeholderFd_);
      placeholderFd_ = ::accept(listenSocket_.Fd(), nullptr, nullptr);
      details::CloseFd(placeholderFd_);
      placeholderFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
    }
  }
}
