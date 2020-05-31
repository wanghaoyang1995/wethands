// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-28 00:07:38
// Description:

#ifndef SRC_REACTOR_TIMERQUEUE_H_
#define SRC_REACTOR_TIMERQUEUE_H_

#include <set>
#include <vector>
#include "src/reactor/Channel.h"
#include "src/reactor/Timer.h"
#include "src/utils/Timestamp.h"
#include "src/utils/Uncopyable.h"

namespace wethands {

class EventLoop;

class TimerQueue : Uncopyable {
 public:
  explicit TimerQueue(EventLoop* loop);
  ~TimerQueue();
  // 向队列中添加定时器. 可以由非loop线程调用.
  TimerIndex AddTimer(Timer::TimerCallback cb,
                      Timestamp when,
                      double interval);
  // 从队列中删除定时器. 可以由非loop线程调用.
  void CancelTimer(TimerIndex timerIndex);

 private:
  void AddTimerInLoop(Timer* timer);
  void CancelTimerInLoop(TimerIndex timerIndex);
  // 处理 timerfd_ 唤醒时的可读事件. 执行到期定时器的回调.
  void HandleRead();
  // 返回 now 时刻及以前到期的定时器, 并将其从 timers_ 中删除.
  std::vector<TimerIndex> GetExpired(Timestamp now);

  EventLoop* loop_;
  const int timerfd_;
  Channel timerfdChannel_;
  std::set<TimerIndex> timers_;
};

}  // namespace wethands

#endif  // SRC_REACTOR_TIMERQUEUE_H_
