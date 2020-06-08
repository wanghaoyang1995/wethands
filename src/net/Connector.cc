// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-06 13:39:38
// Description:

#include "src/net/Connector.h"
#include <errno.h>
#include <sys/socket.h>
#include <utility>
#include "src/logger/Logger.h"

using wethands::Connector;

Connector::Connector(EventLoop* loop, const InetAddress& serverAddr)
    : loop_(loop),
      socket_(),
      socketChannel_(),
      serverAddr_(serverAddr),
      connecting_(false),
      retryDelay_(1),
      stop_(false),
      newConnectionCallback_() {}

Connector::~Connector() {
  StopInLoop();
}

void Connector::Start() {
  loop_->RunInLoop(std::bind(&Connector::StartInLoop, this));
}

void Connector::StartInLoop() {
  assert(loop_->IsInLoopThread());
  if (stop_) return;
  // 创建新的套接字.
  socket_.reset(new Socket());
  socketChannel_.reset(new Channel(loop_, socket_->Fd()));

  int ret = socket_->Connect(serverAddr_);
  int savedErrno = ret == 0 ? 0 : errno;
  // UNP 卷1 第三版 section 16.3.
  // https://stackoverflow.com/questions/8277970/
  switch(savedErrno) {
    // 以下错误不可重启, 需要等待连接完成(等待套接字可写).
    case 0:  // 连接成功.
    case EINPROGRESS:  // 套接字非阻塞且目前无法立即完成.
    case EINTR:  // 被信号中断.
    case EISCONN:  // 套接字已连接.
      RegisterForConnecting();
      connecting_ = true;
      break;

    // 稍候重启可能解决的错误.
    case EAGAIN:  // 套接字非阻塞且目前无法立即完成.
    case ENETUNREACH:  // 网络不可达.
    case EADDRINUSE:  // 本地地址正在使用.
    case EADDRNOTAVAIL:  // 所有临时端口都正在人使用.
    case ECONNREFUSED:  // 服务器端没有监听要连接的远程地址.
      Retry();
      break;

    // 程序无法处理的错误, 放弃.
    default:
      socket_.reset();
      socketChannel_.reset();
      LOG_SYSERROR << "connect error.";
  }
}

void Connector::Stop() {
  stop_ = true;
  loop_->RunInLoop(std::bind(&Connector::StopInLoop, this));
}

void Connector::StopInLoop() {
  socket_.reset();
  if (connecting_) {
    assert(socketChannel_);
    assert(socketChannel_->IsWriting());
    assert(socketChannel_->IsRegistered());
    UnregisterForConnecting();
  }
  socketChannel_.reset();
}

void Connector::RegisterForConnecting() {
  // 等待套接字可写.
  assert(socket_);
  assert(socketChannel_);
  assert(!socketChannel_->IsRegistered());
  socketChannel_->SetWriteCallback(std::bind(&Connector::HandleWrite, this));
  socketChannel_->EnableWriting();
}

void Connector::UnregisterForConnecting() {
  assert(socketChannel_);
  assert(socketChannel_->IsRegistered());
  socketChannel_->DisableWriting();
  socketChannel_->RemoveFromPoller();
}

void Connector::HandleWrite() {
  // 套接字可写, 连接完成或者出错.
  // https://www.man7.org/linux/man-pages/man2/connect.2.html
  if (stop_) return;
  UnregisterForConnecting();
  connecting_ = false;

  int isError = 0;
  socklen_t len = static_cast<socklen_t>(sizeof(isError));
  ::getsockopt(socket_->Fd(),
               SOL_SOCKET,
               SO_ERROR,
               static_cast<void*>(&isError),
               &len);
  if (isError == 0) {  // 没有错误, 说明连接已建立.
    if (socket_->IsSelfConnect()) {
      LOG_WARN << "self connected.";
      Retry();
    } else if (newConnectionCallback_) {
      newConnectionCallback_(std::move(socket_), serverAddr_);
    } else {
      LOG_INFO << "Callback empty. Connection established then closed.";
    }
  } else {  // 有错误发生.
    LOG_ERROR << "Connector::HandleWrite(): connection error.";
    Retry();
  }
}

void Connector::Retry() {
  LOG_INFO << "Connector::Retry(): retry after " << retryDelay_ << "s.";
  loop_->RunAfter(static_cast<double>(retryDelay_),
                  std::bind(&Connector::StartInLoop, this));
  retryDelay_ *= 2;
  if (retryDelay_ > kMaxRetryDelay) {
    retryDelay_ = kMaxRetryDelay;
  }
}
