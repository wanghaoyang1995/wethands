// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-04 10:09:32
// Description:

#ifndef SRC_NET_TCPSERVER_H_
#define SRC_NET_TCPSERVER_H_

#include <atomic>
#include <functional>
#include <memory>
#include <map>
#include <string>
#include "src/net/Acceptor.h"
#include "src/net/Buffer.h"
#include "src/net/InetAddress.h"
#include "src/net/TcpConnection.h"
#include "src/reactor/EventLoop.h"
#include "src/reactor/EventLoopThreadPool.h"
#include "src/utils/Uncopyable.h"

namespace wethands {
// TCP 服务器类.
// 维护着一个 TCP 连接列表, 和一个 I/O 线程池.
class TcpServer : public Uncopyable {
 public:
  using ThreadInitCallback = EventLoopThreadPool::InitCallback;
  using ConnectionCallback = TcpConnection::ConnectionCallback;
  using WriteCompleteCallback = TcpConnection::WriteCompleteCallback;
  using MessageCallback = TcpConnection::MessageCallback;

  TcpServer(EventLoop* loop,
            const InetAddress& listenAddr,
            const std::string& name,
            bool resuePort);
  ~TcpServer();

  EventLoop* OwerLoop() const { return loop_; }
  const std::string& Name() const { return name_; }
  const std::string& IpPort() const { return ipPort_; }
  std::shared_ptr<EventLoopThreadPool> ThreadPool() const {
    return threadPool_;
  }

  void SetThreadInitCallback(const ThreadInitCallback& cb) {
    threadInitCallback_ = cb;
  }
  void SetConnectionCallback(const ConnectionCallback& cb) {
    connectionCallback_ = cb;
  }
  void SetWriteCompleteCallback(const WriteCompleteCallback& cb) {
    writeCompleteCallback_ = cb;
  }
  void SetNewMessageCallback(const MessageCallback& cb) {
    messageCallback_ = cb;
  }
  // 多次调用只会生效一次.
  // 只能由 TcpServer 的 loop 线程调用.
  void Start(int numThreads);

 private:
  // 当有新连接时由 Acceptor 调用此函数, 为新的连接增加一条维护信息.
  void NewConnection(SocketPtr connSocket, const InetAddress& peerAddr);
  // 当对端关闭连接时, 由 I/O 线程调用.
  // 通知 TcpServer loop 线程移除该连接的信息.
  void RemoveConnection(const TcpConnectionPtr& conn);
  void RemoveConnectionInLoop(const TcpConnectionPtr& conn);

  EventLoop* loop_;
  std::unique_ptr<Acceptor> acceptor_;
  std::string name_;
  std::string ipPort_;
  std::shared_ptr<EventLoopThreadPool> threadPool_;
  std::atomic<bool> started_;
  int connCount_;

  ThreadInitCallback threadInitCallback_;
  ConnectionCallback connectionCallback_;
  WriteCompleteCallback writeCompleteCallback_;
  MessageCallback messageCallback_;

  std::map<std::string, TcpConnectionPtr> connections_;
};

}  // namespace wethands

#endif  // SRC_NET_TCPSERVER_H_
