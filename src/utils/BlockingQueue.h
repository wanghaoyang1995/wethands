// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-16 11:40:55
// Description:

#ifndef SRC_UTILS_BLOCKINGQUEUE_H_
#define SRC_UTILS_BLOCKINGQUEUE_H_

#include <deque>
#include <utility>
#include "src/utils/Uncopyable.h"
#include "src/thread/Mutex.h"
#include "src/thread/Condition.h"

namespace wethands {

// 无界阻塞队列. 线程安全的.
template<typename T>
class BlockingQueue : public Uncopyable {
 public:
  BlockingQueue()
      : lock_(),
        notEmpty_(lock_),
        queue_() {}

  void Put(const T& x) {
    MutexLockGuard guard(lock_);
    queue_.push_back(x);
    notEmpty_.Notify();
  }

  void Put(T&& x) {
    MutexLockGuard guard(lock_);
    queue_.push_back(std::move(x));
    notEmpty_.Notify();
  }

  T Take() {
    MutexLockGuard guard(lock_);
    while (queue_.empty()) {
      notEmpty_.Wait();
    }
    T x = std::move(queue_.front());
    queue_.pop_front();
    return x;
  }

  size_t Size() const {
    MutexLockGuard guard(lock_);
    return queue_.size();
  }

 private:
  mutable MutexLock lock_;
  Condition notEmpty_;
  std::deque<T> queue_;
};

}  // namespace wethands

#endif  // SRC_UTILS_BLOCKINGQUEUE_H_
