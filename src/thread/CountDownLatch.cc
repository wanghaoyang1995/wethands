// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-12 22:26:14
// Description:

#include "src/thread/CountDownLatch.h"

using wethands::CountDownLatch;
using wethands::MutexLockGuard;

CountDownLatch::CountDownLatch(int count)
    : lock_(), cond_(lock_), count_(count) {}

void CountDownLatch::CountDown() {
  MutexLockGuard guard(lock_);
  --count_;
  if (count_ == 0) {
    cond_.NotifyAll();
  }
}

void CountDownLatch::Wait() {
  MutexLockGuard guard(lock_);
  while (count_ > 0) {
    cond_.Wait();
  }
}

int CountDownLatch::GetCount() const {
  MutexLockGuard guard(lock_);
  return count_;
}
