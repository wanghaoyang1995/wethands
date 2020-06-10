// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-10 16:18:33
// Description:

#include "src/net/TcpClient.h"
#include <cassert>
#include <functional>

using std::placeholders::_1;
using std::placeholders::_2;

using wethands::TcpClient;

TcpClient::TcpClient(EventLoop* loop,
                     const InetAddress& serverAddr,
                     const std::string& name)
    : loop_(loop),
      connector_(new Connector(loop, serverAddr)),
      name_(name),
      connection_(),
      count_(0),
      retry_(false),
      connectionCallback_(),
      messageCallback_(),
      writeCompleteCallback_() {
  assert(loop_);
  connector_->SetNewConnectionCallback(
    std::bind(&TcpClient::NewConnection, this, _1, _2));
}

TcpClient::~TcpClient() {

}

void TcpClient::Connect() {
  connector_->Start();
}

void TcpClient::Disconnect() {
  connection_->Shutdown();
}

void TcpClient::Stop() {
  connector_->Stop();
}

void TcpClient::NewConnection(SocketPtr connSocket, const InetAddress& serverAddr) {
  assert(loop_->IsInLoopThread());
  // 连接名: 对端地址#序号.
  std::string connName = serverAddr.ToString(true) + "#" +
                         std::to_string(++count_);
  InetAddress localAddr(connSocket->LocalAddress());
  TcpConnectionPtr conn(new TcpConnection(loop_,
                                          connName,
                                          std::move(connSocket),
                                          localAddr,
                                          serverAddr));
  conn->SetConnectionCallback(connectionCallback_);
  conn->SetMessageCallback(messageCallback_);
  conn->SetWriteCompleteCallback(writeCompleteCallback_);
  conn->SetCloseCallback(  // 对端异常关闭.
    std::bind(&TcpClient::HandleClose, this, _1));
  connection_ = conn;
  conn->ConnectionEstablished();  // 直接在当前 loop 线程执行.
}

void TcpClient::HandleClose(const TcpConnectionPtr& conn) {
  assert(loop_->IsInLoopThread());
  connection_.reset();  // 放弃当前连接.
  conn->ConnectionDestroyed();  // 销毁.
  if (retry_) connector_->Restart();
}
