// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-27 20:30:02
// Description:

#ifndef SRC_REACTOR_EVENTLOOP_H_
#define SRC_REACTOR_EVENTLOOP_H_

#include <sys/types.h>
#include <atomic>
#include <functional>
#include <memory>
#include <vector>
#include "src/reactor/Channel.h"
#include "src/thread/CurrentThread.h"
#include "src/thread/Mutex.h"
#include "src/reactor/Poller.h"
#include "src/reactor/Timer.h"
#include "src/reactor/TimerQueue.h"
#include "src/utils/Timestamp.h"
#include "src/utils/Uncopyable.h"

namespace wethands {
// 事件循环, Reactor 模式的核心类.
// 使用者应保证输入给 EventLoop 的 Functor 不为空.
// Functor 尽量不要有太多耗时操作.
class EventLoop : public Uncopyable {
 public:
  using Functor = std::function<void ()>;

  EventLoop();
  ~EventLoop();

  void Loop();
  void Quit();

  // 如果当前处于 Loop 线程, 返回true.
  bool IsInLoopThread() { return loopThreadId_ == CurrentThread::Tid(); }

  // 在loop线程中执行任务 cb.
  // 如果在非loop线程中调用, 会转而调用 QueueInLoop().
  void RunInLoop(Functor cb);

  // 在 time 指定的时刻执行回调函数 cb.
  TimerIndex RunAt(Timestamp time, Timer::TimerCallback cb);
  // 在 delay 秒后执行回调函数 cb.
  TimerIndex RunAfter(double delay, Timer::TimerCallback cb);
  // 以 interval 秒的间隔定期执行回调函数 cb.
  TimerIndex RunEvery(double interval, Timer::TimerCallback cb);
  // 取消 timerIndex 所指定的定时器任务 .
  void CancelTimer(TimerIndex timerIndex);
  // 用 channel 更新 Poller 中的 Channel 列表.
  void UpdateChannel(Channel* channel);
  void RemoveChannel(Channel* channel);
  bool HasChannel(Channel* channel);

  static EventLoop* CurrentThreadLoop();

 private:
  // 将 cb 加入待处理队列中. loop线程会取出执行.
  // 可能会在非loop线程执行.
  void QueueInLoop(Functor cb);

  // 读 eventfd_.
  void HandleEventfdRead();
  void Wakeup();  // 通过写 eventfd_ 唤醒阻塞的 EventLoop.

  MutexLock lock_;
  std::atomic<bool> quit_;
  const pid_t loopThreadId_;  // loop线程(创建EventLoop的线程)ID.
  const int eventfd_;
  std::unique_ptr<Channel> eventfdChannel;
  std::unique_ptr<Poller> poller_;

  std::vector<Channel*> activeChannels_;  // 发生poll事件的Channel列表.
  std::vector<Functor> pendingFunctors_;  // 待执行的任务.
  std::unique_ptr<TimerQueue> timerQueue_;
};

}  // namespace wethands

#endif  // SRC_REACTOR_EVENTLOOP_H_
