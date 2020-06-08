// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-04 10:26:39
// Description:

#include "src/net/Acceptor.h"
#include <fcntl.h>
#include <sys/socket.h>
#include <cassert>
#include <utility>
#include "src/logger/Logger.h"

using wethands::Acceptor;

Acceptor::Acceptor(EventLoop* loop,
                   const InetAddress& listenAddr,
                   bool reusePort)
    : loop_(loop),
      listening_(false),
      listenSocket_(Socket::CreateSocketFd()),
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
  Socket::CloseFd(placeholderFd_);
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
  InetAddress clientAddr;
  int connfd = listenSocket_.Accept(&clientAddr);
  if (connfd >= 0) {
    SocketPtr connSocket(new Socket(connfd));
    if (newConnectionCallback_) {
      newConnectionCallback_(std::move(connSocket), clientAddr);
    } else {
      LOG_INFO << "Callback empty. Connection accepted then closed.";
    }
  } else {
    LOG_SYSERROR << "Socket::Accept() error.";
    if (errno == EMFILE) {  // 描述符用尽.
      // 连接后马上断开.
      Socket::CloseFd(placeholderFd_);
      placeholderFd_ = ::accept(listenSocket_.Fd(), nullptr, nullptr);
      Socket::CloseFd(placeholderFd_);
      placeholderFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
    }
  }
}
