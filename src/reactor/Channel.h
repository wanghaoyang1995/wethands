// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-23 20:51:56
// Description:

#ifndef SRC_REACTOR_CHANNEL_H_
#define SRC_REACTOR_CHANNEL_H_

#include <sys/epoll.h>
#include <functional>
#include "src/utils/Uncopyable.h"

namespace wethands {

class EventLoop;

// 对文件描述符的包装.
// 包含了文件描述符及关联的回调事件信息.
class Channel : public Uncopyable {
 public:
  using EventCallback = std::function<void ()>;

  Channel(EventLoop* loop, int fd);
  ~Channel();

  int Fd() const { return fd_; }
  int Events() const { return events_; }
  EventLoop* OwerLoop() const { return loop_; }
  bool Registered() const { return registeredToPoller; }
  void UpdateRevents(int revents) { revents_ = revents; }

  void EnableReading() { events_ |= kReadEvent; RegisterToPoller(); }
  void DisableReading() { events_ &= ~kReadEvent; RegisterToPoller(); }
  void EnableWriting() { events_ |= kWriteEvent; RegisterToPoller(); }
  void DisableWriting() { events_ &= ~kWriteEvent; RegisterToPoller(); }
  void DisableAll() { events_ = kNoneEvent; RegisterToPoller(); }

  // 根据 revents_ 标记的事件类型, 调用相应的事件处理回调.
  void HandleEvent();
  void SetReadCallback(EventCallback cb) { readCallback_ = std::move(cb); }
  void SetWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }
  void SetCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }
  void SetErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }

  // 通过 loop_, 将当前 Channel 从 Poller 的列表中删除.
  void RemoveFromPoller();

 private:
  // 通过 loop_, 将 fd 的 events_ 事件变更注册给 Poller.
  // 会根据当前对象的状态, 为 Poller 的 Channel 列表添加/更新 Channel.
  void RegisterToPoller();

  EventLoop* loop_;
  bool registeredToPoller;  // 是否已被添加到 Poller 的等待列表中.
  const int fd_;
  int events_;  // 注册给 Poller 的事件.
  int revents_;  // Poll() 返回时被标记为触发的事件.

  EventCallback readCallback_;
  EventCallback writeCallback_;
  EventCallback closeCallback_;
  EventCallback errorCallback_;

  // 用于设置 events_ 的常量(epoll_wait的输入掩码). 使用 epoll.
  static constexpr int kNoneEvent = 0;
  static constexpr int kReadEvent = EPOLLIN && EPOLLPRI;
  static constexpr int kWriteEvent = EPOLLOUT;
};

}  // namespace wethands

#endif  // SRC_REACTOR_CHANNEL_H_
