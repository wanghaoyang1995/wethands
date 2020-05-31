// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-22 22:54:29
// Description:

#ifndef SRC_REACTOR_TIMER_H_
#define SRC_REACTOR_TIMER_H_

#include <cstdint>
#include <functional>
#include "src/thread/Atomic.h"
#include "src/utils/Timestamp.h"
#include "src/utils/Copyable.h"
#include "src/utils/Uncopyable.h"

namespace wethands {

// 定时器.
// 本身并没有定时功能, 只是将回调函数与时间信息相关联.
// 不要单独使用. 与 TimerIndex, TimerQueue配合使用.
class Timer : public Uncopyable {
 public:
  using TimerCallback = std::function<void ()>;

  Timer(TimerCallback callback, Timestamp when, double interval)
      : callback_(std::move(callback)),
        expiration_(when),
        interval_(interval),
        repeated_(interval_ > 0.0),
        sequence_(count_.IncrementAndGet()) {}
  ~Timer() = default;

  void Run() const { callback_(); }
  Timestamp Expiration() const { return expiration_; }
  double Interval() const { return interval_; }
  bool IsRepeated() const { return repeated_; }
  int64_t Sequence() const { return sequence_; }
  // 如果设置了间隔重启, Restart() 会就更新它的 expiration_.
  void Restart();

 private:
  const TimerCallback callback_;
  Timestamp expiration_;  // 到期时刻, Timer重启复用时会改变.
  const double interval_;  // 重启间隔.
  const bool repeated_;  // 是否反复运行.
  const int64_t sequence_;  // 唯一的计时器序号, 从 1 开始.

  static AtomicInt64 count_;
};

// Timer 的包装类, 用来标识和索引 Timer 对象.
// 可拷贝的(浅拷贝).
// 可以比较大小.
class TimerIndex : public Copyable {
 public:
  TimerIndex() : TimerIndex(nullptr, Timestamp::Invalid(), 0) {}
  TimerIndex(Timer* timer, Timestamp expiration, int64_t sequence)
      : timer_(timer),
        expiration_(expiration),
        sequence_(sequence) {}

  TimerIndex(const TimerIndex&) = default;
  // 默认析构足够了. 管理 Timer 的生命周期是 TimerQueue的责任.
  ~TimerIndex() = default;

  Timestamp Expiration() const { return expiration_; }
  int64_t Sequence() const { return sequence_; }

  // 为了在有序容器中使用.
  // 首先比较到期时间, 先到期者小.
  // 如果到期时间相同, 就比较序号, 序号小者小.
  bool operator<(const TimerIndex& rhs) const {
    return expiration_ != rhs.expiration_ ?
        expiration_ < rhs.expiration_ :
        sequence_ < rhs.sequence_;
  }

  bool operator==(const TimerIndex& rhs) const {
    return timer_ == rhs.timer_ &&
           expiration_ == rhs.expiration_ &&
           sequence_ == rhs.sequence_;
  }

  friend class TimerQueue;

 private:
  Timer* timer_;
  const Timestamp expiration_;
  const int64_t sequence_;
};

static_assert(
  sizeof(TimerIndex) == sizeof(Timer*) + sizeof(Timestamp) + sizeof(int64_t),
  "unexpect size.");

}  // namespace wethands

#endif  // SRC_REACTOR_TIMER_H_
