// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-03 00:10:00
// Description:

#include "src/net/TcpConnection.h"
#include <functional>
#include <utility>
#include "src/logger/Logger.h"

using wethands::TcpConnection;

TcpConnection::TcpConnection(EventLoop* loop,
                             const std::string& name,
                             SocketPtr connSocket,
                             const InetAddress& localAddr,
                             const InetAddress& peerAddr)
    : loop_(loop),
      name_(name),
      state_(kConnecting),
      reading_(false),
      socket_(std::move(connSocket)),
      socketChannel_(new Channel(loop_, socket_->Fd())),
      localAddr_(localAddr),
      peerAddr_(peerAddr),
      inputBuffer_(),
      outputBuffer_(),
      highWaterMark_(64*1024*1024),
      connectionCallback_(),
      writeCompleteCallback_(),
      messageCallback_(),
      highWaterMarkCallback_(),
      closeCallback_() {
  socketChannel_->SetReadCallback(std::bind(&TcpConnection::HandleRead, this));
  socketChannel_->SetWriteCallback(
    std::bind(&TcpConnection::HandleWrite, this));
  socketChannel_->SetCloseCallback(  // EPOLLHUP, 对端异常关闭.
    std::bind(&TcpConnection::HandleClose, this));
  socketChannel_->SetErrorCallback(
    std::bind(&TcpConnection::HandleError, this));
  socket_->SetKeepAlive(true);
  LOG_INFO << "TcpConnection construct.";
}

TcpConnection::~TcpConnection() {
  assert(state_ == kDisconnected);
  LOG_INFO << "TcpConnection destruct.";
}

void TcpConnection::Send(const void* data, size_t len) {
  // 要考虑二进制数据的情况.
  if (state_ != kConnected) return;
  if (loop_->IsInLoopThread()) {  // 在 loop 线程中, 直接写.
    SendInLoop(data, len);
  } else {  // 不在 loop 线程中, 要将数据复制一份.
    std::string dataToWrite(static_cast<const char*>(data), len);
    // 定义一个指向成员的指针. 为了避免模糊的重载函数调用.
    void (TcpConnection::*funcPtr)(const std::string&) =
      &TcpConnection::SendInLoop;
    // std::bind() 默认是值传递, 所以不会出现调用时 string 已被销毁的问题.
    loop_->RunInLoop(std::bind(funcPtr, this, dataToWrite));
  }
}

void TcpConnection::Send(const std::string& data) {
  Send(data.data(), data.size());
}

void TcpConnection::Send(Buffer* buf) {
  Send(buf->RetrieveAllAsString());
}

void TcpConnection::StartRead() {
  loop_->RunInLoop(
    std::bind(&TcpConnection::StartReadInLoop, shared_from_this()));
}

void TcpConnection::StopRead() {
  loop_->RunInLoop(
    std::bind(&TcpConnection::StopReadInLoop, shared_from_this()));
}

void TcpConnection::Shutdown() {
  if (state_ == kConnected) {
    state_ = kDisconnecting;
    loop_->RunInLoop(std::bind(&TcpConnection::ShutdownInLoop,
                     shared_from_this()));
  }
}

void TcpConnection::ForceClose() {
  if (state_ == kConnected || kDisconnecting) {
    state_ = kDisconnecting;
    loop_->RunInLoop(std::bind(&TcpConnection::ForceCloseInLoop,
                               shared_from_this()));
  }
}

void TcpConnection::SetTcpNoDelay(bool on) {
  socket_->SetTcpNoDelay(on);
}

void TcpConnection::ConnectionEstablished() {
  // 该函数是连接建立后的第一步.
  assert(loop_->IsInLoopThread());
  assert(state_ == kConnecting);
  socketChannel_->EnableReading();  // 不用手动调用 StartRead.
  state_ = kConnected;
  connectionCallback_(shared_from_this());
}

void TcpConnection::ConnectionDestroyed() {
  // 该函数是连接销毁前的最后一步.
  assert(loop_->IsInLoopThread());
  assert(state_ == kDisconnected);
  socketChannel_->DisableAll();
  socketChannel_->RemoveFromPoller();
  // 用户应确保不能在回调中操作已断开的连接, 只能获取状态信息.
  connectionCallback_(shared_from_this());
}

void TcpConnection::SendInLoop(const void* data, size_t len) {
  assert(loop_->IsInLoopThread());
  if (state_ != kConnected) {
    LOG_WARN << "Not connected. Abort to send.";
    return;
  }

  ssize_t writen = 0;
  size_t remaining = len;
  // 输出缓冲区没有待写数据, 可以将新的数据直接写入套接字.
  if (!socketChannel_->IsWriting() && outputBuffer_.ReadableBytes() == 0) {
    writen = socket_->Send(data, remaining);
    if (writen >= 0) {  // 写正常.
      remaining -= static_cast<size_t>(writen);
      if (remaining == 0 && writeCompleteCallback_) {  // 所有的都写完了.
        loop_->RunInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
        return;
      }
    } else {  // 写出错.
      switch (errno) {
        case EPIPE:  // 对端已关闭.
        case EINTR:  // 被信号中断.
        case EWOULDBLOCK:  // 同EAGAIN, 套接字非阻塞且目前无法立即写.
          LOG_ERROR << "Socket::Send() error.";
          writen = 0;
          break;
        default:
          LOG_SYSFATAL << "Socket::Send() fatal.";
      }
    }
  }

  assert(remaining <= len);
  size_t oldBytes = outputBuffer_.ReadableBytes();
  if (oldBytes + remaining >= highWaterMark_ &&
      oldBytes < highWaterMark_ &&
      highWaterMarkCallback_) {
    loop_->QueueInLoop(std::bind(highWaterMarkCallback_,
                       shared_from_this(),
                       oldBytes + remaining));
  }
  // 剩余数据放入输出缓冲区.
  outputBuffer_.Append(static_cast<const char*>(data) + writen, remaining);
  // 等待 HandleWrite 处理.
  if (!socketChannel_->IsWriting()) socketChannel_->EnableWriting();
}

void TcpConnection::SendInLoop(const std::string& data) {
  SendInLoop(data.data(), data.size());
}

void TcpConnection::StartReadInLoop() {
  assert(loop_->IsInLoopThread());
  if (reading_ == false && !socketChannel_->IsReading()) {
    socketChannel_->EnableReading();
    reading_ = true;
  }
}

void TcpConnection::StopReadInLoop() {
  assert(loop_->IsInLoopThread());
  if (reading_ == true && socketChannel_->IsReading()) {
    socketChannel_->DisableReading();
    reading_ = false;
  }
}

void TcpConnection::ShutdownInLoop() {
  assert(loop_->IsInLoopThread());
  // 在连接状态, 且输出缓冲区没有待发送的数据时, 直接关闭写端.
  // 如果输出缓冲区有数据待发送, 就先不关闭, 留给 HandleWrite 处理后关闭.
  if (state_ == kConnected && !socketChannel_->IsWriting()) {
    socket_->ShutdownWrite();
  }
}

void TcpConnection::ForceCloseInLoop() {
  assert(loop_->IsInLoopThread());
  if (state_ == kConnected || state_ == kDisconnecting) {
    HandleClose();
  }
}

void TcpConnection::HandleRead() {
  assert(loop_->IsInLoopThread());
  int errorCode = 0;
  ssize_t n = inputBuffer_.ReadFd(socket_->Fd(), &errorCode);
  if (n < 0) {
    LOG_SYSERROR << "HandleRead: ReadFd() error.";
    HandleError();
  } else if (n == 0) {  // 遇到EOF, 关闭连接.
    HandleClose();  // 正常关闭.
  } else if (messageCallback_) {  // 正常读入.
    messageCallback_(shared_from_this(), &inputBuffer_, Timestamp::Now());
  }
}

void TcpConnection::HandleWrite() {
  // 当 Send 函数不能一次性写完时, 会将剩余内容放入输出缓冲区并注册可写事件关注.
  // 套接字可写时被调用, 该函数将输出缓冲区剩余内容写入套接字.
  assert(loop_->IsInLoopThread());
  // 连接有可能已经关闭, 放弃.
  if (!socketChannel_->IsWriting()) {
    LOG_WARN << "HandleWrite(): connection closed, abort to write.";
    return;
  }

  ssize_t n = socket_->Send(outputBuffer_.Peek(),
                                 outputBuffer_.ReadableBytes());
  if (n > 0) {
    outputBuffer_.Retrieve(n);
    if (outputBuffer_.ReadableBytes() == 0) {  // 将缓冲区内容写完了.
      socketChannel_->DisableWriting();  // 取消关注.
      if (writeCompleteCallback_) writeCompleteCallback_(shared_from_this());
      // 用户调用了 Shutdown(), 写完了就立即执行.
      if (state_ == kDisconnecting) ShutdownInLoop();
    }
  }
}

void TcpConnection::HandleClose() {
  // 对端正常(HandleRead EOF)或异常(channel->CloseCallback)关闭时,
  // 或者己方主动关闭(ForceClose), 都会调用该函数.
  // 此处 state_ 为 kConnected 说明是对端关闭.
  // state_ 为 kDisconnecting 说明是己方主动关闭.
  assert(loop_->IsInLoopThread());
  assert(state_ == kConnected || state_ == kDisconnecting);
  socketChannel_->DisableAll();
  state_ = kDisconnected;
  // 通知 TcpServer(TcpServer::RemoveConnection) 将自己从列表移除.
  // 移除后 TcpServer 会通知自己调用 ConnectionDestroyed() 处理后事.
  closeCallback_(shared_from_this());
}

void TcpConnection::HandleError() {
  int errorCode = socket_->ErrorCode();
  char errbuf[512];
  const char* errstr = ::strerror_r(errorCode, errbuf, sizeof(errbuf));
  LOG_ERROR << "TcpConnection::HandleError(): errorCode = " << errorCode
            << ", " << errstr;
}
