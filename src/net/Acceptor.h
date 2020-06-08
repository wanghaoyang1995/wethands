// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-04 10:26:32
// Description:

#ifndef SRC_NET_ACCEPTOR_H_
#define SRC_NET_ACCEPTOR_H_

#include <functional>
#include <memory>
#include "src/net/InetAddress.h"
#include "src/net/Socket.h"
#include "src/reactor/Channel.h"
#include "src/reactor/EventLoop.h"
#include "src/utils/Uncopyable.h"

namespace wethands {
// 连接请求接受器. 不负责连接的维护.
// 有新的请求时调用指定回调, 转移已连接套接字的生命周期.
class Acceptor : public Uncopyable {
 public:
  using NewConnectionCallback =
    std::function<void (SocketPtr connSocket,
                        const InetAddress& clientAddr)>;

  Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reusePort);
  ~Acceptor();

  void SetNewConnectionCallback(const NewConnectionCallback& cb) {
    newConnectionCallback_ = cb;
  }

  bool IsListening() const { return listening_; }
  void Listen();

 private:
  // 监听套接字可读时调用此函数.
  void HandleRead();

  EventLoop* loop_;
  bool listening_;
  // 监听套接字.
  // Acceptor 要负责管理监听套接字的生命周期, 所以使用 Socket.
  Socket listenSocket_;
  Channel listenSocketChannel_;  // 与 socket_ 关联的 Channel.
  // 调用该回调,  转移新完成连接的控制权.
  NewConnectionCallback newConnectionCallback_;
  int placeholderFd_;  // 用于占位, 为了处理描述符用尽的情况.
};

}  // namespace wethands

#endif  // SRC_NET_ACCEPTOR_H_
