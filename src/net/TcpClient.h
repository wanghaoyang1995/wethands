// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-10 16:18:24
// Description:

#ifndef SRC_NET_TCPCLIENT_H_
#define SRC_NET_TCPCLIENT_H_

#include <atomic>
#include <memory>
#include "src/net/Connector.h"
#include "src/net/InetAddress.h"
#include "src/net/TcpConnection.h"
#include "src/reactor/EventLoop.h"
#include "src/utils/Uncopyable.h"

namespace wethands {
// Tcp 主动端.
// 与 TcpServer 不同, 所有工作都在当前 loop 线程中进行.
class TcpClient : public Uncopyable {
 public:
  using ConnectionCallback = TcpConnection::ConnectionCallback;
  using MessageCallback = TcpConnection::MessageCallback;
  using WriteCompleteCallback = TcpConnection::WriteCompleteCallback;

  TcpClient(EventLoop* loop,
            const InetAddress& serverAddr,
            const std::string& name);
  ~TcpClient();

  EventLoop* OwerLoop() const { return loop_; }
  const std::string& Name() const { return name_; }
  TcpConnectionPtr Connection() const { return connection_; }

  void Connect();
  // 关闭连接.
  void Disconnect();
  // 停止服务器.
  void Stop();
  // 如果参数为 true, 当对端连接异常断开时自动重连.
  void SetAutoRetry(bool on) { retry_ = on; }

  void SetConnectionCallback(const ConnectionCallback& cb) {
    connectionCallback_ = cb;
  }

  void SetMessageCallback(const MessageCallback& cb) {
    messageCallback_ = cb;
  }

  void SetWriteCompleteCallback(const WriteCompleteCallback& cb) {
    writeCompleteCallback_ = cb;
  }

 private:
  void NewConnection(SocketPtr connSocket, const InetAddress& serverAddr);
  void HandleClose(const TcpConnectionPtr& conn);

  EventLoop* loop_;
  ConnectorPtr connector_;
  std::string name_;
  TcpConnectionPtr connection_;
  int count_;
  std::atomic<bool> retry_;

  ConnectionCallback connectionCallback_;
  MessageCallback messageCallback_;
  WriteCompleteCallback writeCompleteCallback_;
};

}  // namespace wethands

#endif  // SRC_NET_TCPCLIENT_H_
