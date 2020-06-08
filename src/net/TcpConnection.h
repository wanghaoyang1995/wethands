// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-03 00:09:52
// Description:

#ifndef SRC_NET_TCPCONNECTION_H_
#define SRC_NET_TCPCONNECTION_H_

#include <string>
#include <memory>
#include "src/net/Buffer.h"
#include "src/net/InetAddress.h"
#include "src/net/Socket.h"
#include "src/reactor/Channel.h"
#include "src/reactor/EventLoop.h"
#include "src/utils/Uncopyable.h"

namespace wethands {

class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

// Tcp 连接的封装.
// 从 Accept 和 Connector 那里接管已连接套接字的生命周期.
class TcpConnection : public Uncopyable {
 public:
  using ConnectionCallback = std::function<void (const TcpConnectionPtr&)>;
  using WriteCompleteCallback = std::function<void (const TcpConnectionPtr&)>;
  using MessageCallback =
    std::function<void (const TcpConnectionPtr&, const Buffer&)>;
  using HighWaterMarkCallback =
    std::function<void (const TcpConnectionPtr&, size_t)>;
  using CloseCallback = std::function<void (const TcpConnectionPtr&)>;

  TcpConnection(EventLoop* loop,  // 通常是 I/O 线程.
                const std::string& name,
                SocketPtr connSocket,
                const InetAddress& localAddr,
                const InetAddress& peerAddr);
  ~TcpConnection();

  EventLoop* OwerLoop() const { return loop_; }
  const std::string& Name() const { return name_; }
  InetAddress LocalAddress() const { return localAddr_; }
  InetAddress PeerAddress() const { return peerAddr_; }
  Buffer* GetInputBuffer() { return &inputBuffer_; }
  Buffer* GetOutputBuffer() { return &outputBuffer_; }

  bool Connected() const { return state_ == kConnected; }
  // 向套接字写入数据.
  void Send(const void* data, size_t len);
  void Send(const std::string& data, size_t len);
  void StartRead();
  void StopRead();
  void Shutdown();
  void SetTcpNoDelay();

  void SetConnectionCallback(const ConnectionCallback& cb) {
    connectionCallback_ = cb;
  }
  void SetWriteCompleteCallback(const WriteCompleteCallback& cb) {
    writeCompleteCallback_ = cb;
  }
  void SetMessageCallback(const MessageCallback& cb) {
    messageCallback_ = cb;
  }
  void SetHighWaterMarkCallback(const HighWaterMarkCallback& cb) {
    highWaterMarkCallback_ = cb;
  }
  void SetHighWaterMark(size_t highWaterMark) {
    highWaterMark_ = highWaterMark;
  }

  // 当对端连接关闭时, 用来通知 TcpServer 的 loop 线程更新其连接列表的信息.
  // 不应该由用户调用.
  void SetCloseCallback(const CloseCallback& cb) {
    closeCallback_ = cb;
  }
  void ConnectionEstablished();
  void ConnectionDestroyed();

 private:
  // 四种状态: 正准备连接, 已连接, 正断开连接, 已断开连接.
  enum States { kConnecting, kConnected, kDisconnecting, kDisconnected };
  void SendInLoop(const void* data, size_t len);
  // string 中有可能是二进制数据. 注意避免空字符截断.
  void SendInLoop(const std::string& data);

  // socketChannel_ 相关的事件处理.
  void HandleRead();
  void HandleWrite();
  void HandleClose();
  void HandleError();

  EventLoop* loop_;
  std::string name_;
  States state_;
  std::unique_ptr<Socket> socket_;  // TCP 本地端套接字.
  std::unique_ptr<Channel> socketChannel_;
  InetAddress localAddr_;
  InetAddress peerAddr_;
  Buffer inputBuffer_;
  Buffer outputBuffer_;
  size_t highWaterMark_;

  ConnectionCallback connectionCallback_;
  WriteCompleteCallback writeCompleteCallback_;
  MessageCallback messageCallback_;
  HighWaterMarkCallback highWaterMarkCallback_;
  CloseCallback closeCallback_;
};

}  // namespace wethands

#endif  // SRC_NET_TCPCONNECTION_H_
