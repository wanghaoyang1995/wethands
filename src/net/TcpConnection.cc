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
  socketChannel_->SetCloseCallback(
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

void TcpConnection::StartRead() {

}

void TcpConnection::StopRead() {

}

void TcpConnection::Shutdown() {

}

void TcpConnection::SetTcpNoDelay() {

}


void TcpConnection::ConnectionEstablished() {

}

void TcpConnection::ConnectionDestroyed() {

}

void TcpConnection::SendInLoop(const void* data, size_t len) {

}

void TcpConnection::SendInLoop(const std::string& data) {
  SendInLoop(static_cast<const void*>(data.data()), data.size());
}

void TcpConnection::HandleRead() {

}

void TcpConnection::HandleWrite() {

}

void TcpConnection::HandleClose() {

}

void TcpConnection::HandleError() {

}
