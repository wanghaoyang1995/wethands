// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-04 10:26:32
// Description:

#ifndef SRC_NET_ACCEPTOR_H_
#define SRC_NET_ACCEPTOR_H_

#include <functional>
#include "src/net/InetAddress.h"
#include "src/net/Socket.h"
#include "src/reactor/Channel.h"
#include "src/reactor/EventLoop.h"
#include "src/utils/Uncopyable.h"

namespace wethands {
// 连接请求接受器. 不负责连接的维护.
// 有新的请求时调用使用者指定的 NewConnectionCallback.
// 如果不指定回调就直接关闭连接.
class Acceptor : public Uncopyable {
 public:
  using NewConnectionCallback =
    std::function<void (int connfd, const InetAddress& peerAddr)>;

  Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reusePort);
  ~Acceptor();

  void SetNewConnectionCallback(const NewConnectionCallback& cb) {
    newConnectionCallback_ = cb;
  }

  bool Listening() const { return listening_; }
  void Listen();

 private:
  // 监听套接字可读时调用此函数.
  void HandleRead();

  EventLoop* loop_;
  bool listening_;
  Socket listenSocket_;  // 监听套接字.
  Channel listenSocketChannel_;  // 与 socket_ 关联的 Channel.
  NewConnectionCallback newConnectionCallback_;
  int placeholderFd_;  // 用于占位, 为了处理描述符用尽的情况.
};

}  // namespace wethands

#endif  // SRC_NET_ACCEPTOR_H_
