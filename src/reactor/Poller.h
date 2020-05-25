// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-23 21:31:49
// Description:

#ifndef SRC_REACTOR_POLLER_H_
#define SRC_REACTOR_POLLER_H_

#include <sys/epoll.h>
#include <map>
#include <vector>
#include "src/reactor/EventLoop.h"
#include "src/reactor/Channel.h"
#include "src/utils/Timestamp.h"
#include "src/utils/Uncopyable.h"

namespace wethands {

// 多路复用器. 使用 epoll.
class Poller : public Uncopyable {
 public:
  Poller(EventLoop* loop);
  ~Poller();
  // 阻塞并监控文件描述符, 直到有事件发生时或者超过timeout毫秒时返回.
  // 返回时 activeChannels 所指向的列表中填充了发生事件的 Channel.
  Timestamp Poll(int timeout, std::vector<Channel*>* activeChannels);
  void UpdateChannel(Channel* channel);
  void RemoveChannel(Channel* channel);
  bool HasChannel(Channel* channel);

  // 就绪事件列表的大小.
  static constexpr int kEventListSize_ = 16;

 private:
  EventLoop* loop_;
  int epollfd_;
  std::map<int, Channel*> channels_;  // 记录已注册的描述符事件.
  std::vector<struct epoll_event> events_;  // epoll_wait() 就绪事件列表.
};

}

#endif  // SRC_REACTOR_POLLER_H_
