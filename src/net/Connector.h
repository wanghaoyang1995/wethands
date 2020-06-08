// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-06 13:39:31
// Description:

#ifndef SRC_NET_CONNECTOR_H_
#define SRC_NET_CONNECTOR_H_

#include <atomic>
#include <functional>
#include <memory>
#include "src/net/InetAddress.h"
#include "src/net/Socket.h"
#include "src/reactor/Channel.h"
#include "src/reactor/EventLoop.h"
#include "src/utils/Uncopyable.h"


namespace wethands {
// 连接请求发起器.
// 连接成功后会调用用户定义的回调 NewConnectionCallback.
// 连接成功后套接字的生命周期由用户接管.
class Connector : public Uncopyable {
 public:
  using NewConnectionCallback =
    std::function<void (int connfd)>;

  Connector(EventLoop* loop, const InetAddress& serverAddr);
  ~Connector();

  void SetNewConnectionCallback(const NewConnectionCallback& cb) {
    newConnectionCallback_ = cb;
  }
  // 尝试连接, 如果失败会按一定间隔重试, 直到连接成功或者调用 Cancel().
  void Start();
  // 取消连接.
  void Stop();

 private:
  void StartInLoop();
  void StopInLoop();
  // 不会阻塞. 在 poller 中注册对套接字可写事件的关注.
  // 用来处理 connect 返回出错但又不能重启的情况.
  void RegisterForConnecting();
  void UnregisterForConnecting();
  // 处理套接字可写(等待的连接已完成)事件.
  void HandleWrite();
  // connect 暂时失败后间隔重试.
  void Retry();

  EventLoop* loop_;
  std::unique_ptr<Socket> socket_;  // 主动连接套接字.
  std::unique_ptr<Channel> socketChannel_;  // 与主动连接套接字关联.
  InetAddress serverAddr_;
  bool connecting_;
  int retryDelay_;  // 首次重试间隔时间(秒).
  std::atomic<bool> stop_;
  NewConnectionCallback newConnectionCallback_;

  static constexpr int kMaxRetryDelay = 30;  // 最大重试间隔时间.
};

}  // namespace wethands

#endif  // SRC_NET_CONNECTOR_H_
