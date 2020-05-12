// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-12 22:25:59
// Description:

#ifndef SRC_THREAD_COUNTDOWNLATCH_H_
#define SRC_THREAD_COUNTDOWNLATCH_H_

#include "src/utils/Uncopyable.h"
#include "src/thread/Mutex.h"
#include "src/thread/Condition.h"

namespace wethands {

// 倒计门闩量.
class CountDownLatch : public Uncopyable {
 public:
  explicit CountDownLatch(int count);
  ~CountDownLatch() = default;

  // 递减计数. 当计数减为0时会唤醒所有线程.
  void CountDown();
  // 等待计数为0的条件发生.
  void Wait();
  // 获取内部计数值.
  int GetCount() const;

 private:
  mutable MutexLock lock_;
  Condition cond_;
  int count_;
};

}  // namespace wethands

#endif  // SRC_THREAD_COUNTDOWNLATCH_H_
