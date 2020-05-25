// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-28 00:07:38
// Description:

#ifndef SRC_REACTOR_TIMERQUEUE_H_
#define SRC_REACTOR_TIMERQUEUE_H_

#include <set>
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

  TimerIndex AddTimer(Timer::TimerCallback cb,
                      Timestamp when,
                      double interval);

  void CancelTimer(TimerIndex timerIndex);

 private:
  void AddTimerInLoop(Timer* timer);
  void CancelTimerInLoop(TimerIndex timerIndex);
  // 处理 timerfd_ 唤醒时的可读事件.
  void HandleRead();

  EventLoop* loop_;
  int tiemerfd_;
  Channel timerfdChannel_;
  std::set<TimerIndex> timers_;
};

}

#endif  // SRC_REACTOR_TIMERQUEUE_H_
