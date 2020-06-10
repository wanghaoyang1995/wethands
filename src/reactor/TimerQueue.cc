// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-28 00:07:47
// Description:

#include "src/reactor/TimerQueue.h"

#include <unistd.h>
#include <sys/timerfd.h>

#include <cassert>
#include <cstdint>
#include <cstring>
#include <functional>
#include <utility>

#include "src/reactor/EventLoop.h"
#include "src/logger/Logger.h"

namespace wethands {
namespace details {

// 创建并检查 timerfd.
int CreateTimerfd() {
  int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
  if (timerfd < 0) {
    LOG_SYSFATAL << "timerfd_create() error.";
  }
  return timerfd;
}

// 有事件到来时读 timerfd.
void ReadTimerfd(int timerfd) {
  uint64_t count;  // 从 timerfd 读到的超时次数.
  ssize_t n = ::read(timerfd, &count, sizeof(count));
  LOG_TRACE << "TimerQueue::HandleRead(), count = " << count;
  if (n != sizeof(count)) {
    LOG_ERROR << "TimerQueue::HandleRead(): bytes read is not "
              << sizeof(count);
  }
}

// 更新 timerfd 的到期时间.
void UpdateTimerfd(int timerfd, Timestamp expiration) {
  struct itimerspec newVal;
  ::memset(&newVal, 0, sizeof(newVal));
  Timestamp now = Timestamp::Now();
  if (expiration > now) {
    // 不需要设置 newVal.it_interval, 重复运行的功能由 TimerQueue 实现.
    newVal.it_value = TimeDifference(expiration, Timestamp::Now());
  } else {  // 已经过期了, 在 timerfd 中设置为1纳秒后过期.
    newVal.it_value.tv_nsec = 1;
  }

  // flag 默认为 0, 表明 it_value 使用相对时间.
  int ret = ::timerfd_settime(timerfd, 0, &newVal, nullptr);
  if (ret < 0) {
    LOG_SYSERROR << "timerfd_settime() error.";
  }
}

}  // namespace details
}  // namespace wethands

using wethands::TimerQueue;
using wethands::Timer;
using wethands::TimerIndex;

TimerQueue::TimerQueue(EventLoop* loop)
    : loop_(loop),
      timerfd_(details::CreateTimerfd()),
      timerfdChannel_(loop, timerfd_),
      timers_() {
  timerfdChannel_.SetReadCallback(std::bind(&TimerQueue::HandleRead, this));
  timerfdChannel_.EnableReading();
}

TimerQueue::~TimerQueue() {
  timerfdChannel_.DisableAll();
  timerfdChannel_.RemoveFromPoller();
  ::close(timerfd_);
  // 删除剩余的定时器.
  for (const TimerIndex& timerIndex : timers_) {
    delete timerIndex.timer_;
  }
}

TimerIndex TimerQueue::AddTimer(Timer::TimerCallback cb,
                                Timestamp when,
                                double interval) {
  Timer* timer = new Timer(cb, when, interval);  // 在这里 new, 记得delete.
  loop_->RunInLoop(std::bind(&TimerQueue::AddTimerInLoop, this, timer));
  return TimerIndex(timer, when, timer->Sequence());
}

void TimerQueue::CancelTimer(TimerIndex timerIndex) {
  loop_->RunInLoop(std::bind(&TimerQueue::CancelTimerInLoop, this, timerIndex));
}

void TimerQueue::AddTimerInLoop(Timer* timer) {
  assert(loop_->IsInLoopThread());
  TimerIndex toAdd(timer, timer->Expiration(), timer->Sequence());

  if (toAdd.Expiration() < timers_.begin()->Expiration() ||
      timers_.empty()) {  // 如果即将加入的计时器到期时间最早.
    // 更新  timerfd 到期时间.
    details::UpdateTimerfd(timerfd_, toAdd.Expiration());
  }
  // 将其入 timers_ 中.
  std::pair<std::set<TimerIndex>::iterator, bool> ret
    = timers_.insert(toAdd);
  assert(ret.second == true);
}

void TimerQueue::CancelTimerInLoop(TimerIndex timerIndex) {
  assert(loop_->IsInLoopThread());
  if (timers_.find(timerIndex) != timers_.end()) {
    // 只需要移除, 没必要再更改 timerfd 的到期时间.
    timers_.erase(timerIndex);
    delete timerIndex.timer_;  // 删除要取消的定时器.
  }
}

void TimerQueue::HandleRead() {
  // 处理可读事件, 说明有计时器到期.
  assert(loop_->IsInLoopThread());
  Timestamp now = Timestamp::Now();
  details::ReadTimerfd(timerfd_);

  // 执行已到期的定时器回调, 之后复用或者删除定时器.
  std::vector<TimerIndex> expired = GetExpired(now);
  for (const TimerIndex& timerIndex : expired) {
    timerIndex.timer_->Run();

    if (timerIndex.timer_->IsRepeated()) {
      timerIndex.timer_->Restart();
      AddTimerInLoop(timerIndex.timer_);
    } else {
      delete timerIndex.timer_;  // 删除已过期且不需要重启的定时器.
    }
  }
  // 如果队列中还有定时器, 更新 timerfd_ 的到期时间.
  if (!timers_.empty()) {
    details::UpdateTimerfd(timerfd_, timers_.begin()->Expiration());
  }
}

std::vector<TimerIndex> TimerQueue::GetExpired(Timestamp now) {
  // 在 timers_ 中寻找 now 时刻已到期的定时器.
  // 如果到期时间相同, sentry 总是大者.
  TimerIndex sentry(nullptr, now, INT64_MAX);
  std::set<TimerIndex>::iterator it = timers_.lower_bound(sentry);
  std::vector<TimerIndex> expired(timers_.begin(), it);
  timers_.erase(timers_.begin(), it);
  return expired;
}
