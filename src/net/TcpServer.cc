// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-04 10:09:40
// Description:

#include "src/net/TcpServer.h"
#include <cassert>
#include <utility>

using wethands::TcpServer;
using std::placeholders::_1;
using std::placeholders::_2;

TcpServer::TcpServer(EventLoop* loop,
                     const InetAddress& listenAddr,
                     const std::string& name,
                     bool resuePort)
    : loop_(loop),
      acceptor_(new Acceptor(loop, listenAddr, resuePort)),
      name_(name),
      ipPort_(listenAddr.ToString(true)),
      threadPool_(new EventLoopThreadPool(loop, name_)),
      started_(false),
      connCount_(0),
      threadInitCallback_(),
      connectionCallback_(),
      writeCompleteCallback_(),
      messageCallback_(),
      connections_() {
  assert(loop_);
  acceptor_->SetNewConnectionCallback(
    std::bind(&TcpServer::NewConnection, this, _1, _2));
}

TcpServer::~TcpServer() {
  assert(loop_->IsInLoopThread());
  for (std::pair<std::string, TcpConnectionPtr> item : connections_) {
    TcpConnectionPtr conn = item.second;
    conn->OwerLoop()->RunInLoop(
      std::bind(&TcpConnection::ConnectionDestroyed, conn));
  }
}

void TcpServer::Start(int numThreads) {
  assert(loop_->IsInLoopThread());
  assert(numThreads >= 0);
  if (started_) return;

  threadPool_->Start(numThreads, threadInitCallback_);
  acceptor_->Listen();
}

void TcpServer::NewConnection(SocketPtr connSocket,
                              const InetAddress& peerAddr) {
  // Acceptor 的回调函数.
  assert(loop_->IsInLoopThread());
  EventLoop* ioLoop = threadPool_->NextLoop();  // 取出一个 I/O 子线程.
  std::string connName = ipPort_ + "#" + std::to_string(++connCount_);
  InetAddress localAddr(connSocket->LocalAddress());
  TcpConnectionPtr conn(new TcpConnection(ioLoop,  // 新连接由子线程管理.
                                          connName,
                                          std::move(connSocket),
                                          localAddr,
                                          peerAddr));
  connections_[connName] = conn;  // 将连接记录在列表中.
  // 三个由用户定义的回调. 事件直接在 I/O 线程中处理.
  conn->SetConnectionCallback(connectionCallback_);
  conn->SetMessageCallback(messageCallback_);
  conn->SetWriteCompleteCallback(writeCompleteCallback_);
  // 对端关闭事件, 交由主 loop 线程处理.
  // 如果连接关闭了, I/O 线程就通知 TcpServer(主loop线程)将其从列表中删除.
  conn->SetCloseCallback(
    std::bind(&TcpServer::RemoveConnection, this, _1));
  // 通知 I/O 子线程有新连接, 让其做一些初始化工作.
  ioLoop->RunInLoop(std::bind(&TcpConnection::ConnectionEstablished, conn));
}

void TcpServer::RemoveConnection(const TcpConnectionPtr& conn) {
  loop_->RunInLoop(std::bind(&TcpServer::RemoveConnectionInLoop, this, conn));
}

void TcpServer::RemoveConnectionInLoop(const TcpConnectionPtr& conn) {
  assert(loop_->IsInLoopThread());
  connections_.erase(conn->Name());
  EventLoop* ioLoop = conn->OwerLoop();
  // 主 loop 线程已经将 conn 从列表中移除, 通知 conn 做清理工作.
  ioLoop->RunInLoop(std::bind(&TcpConnection::ConnectionDestroyed, conn));
}
