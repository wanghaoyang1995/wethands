// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-16 11:41:18
// Description:

#ifndef SRC_UTILS_BOUNDEDBLOCKINGQUEUE_H_
#define SRC_UTILS_BOUNDEDBLOCKINGQUEUE_H_

#include <cassert>
#include <deque>
#include <utility>
#include <vector>
#include "src/utils/Uncopyable.h"
#include "src/thread/Mutex.h"
#include "src/thread/Condition.h"

namespace wethands {

// 有界阻塞队列. 线程安全的.
template<typename T>
class BoundedBlockingQueue : public Uncopyable {
 public:
  explicit BoundedBlockingQueue(int maxSize)
      : lock_(),
        notEmpty_(lock_),
        notFull_(lock_),
        queue_(maxSize) {}

  void Put(const T& x) {
    MutexLockGuard guard(lock_);
    while (queue_.full()) {
      notFull_.Wait();
    }
    assert(!queue_.full());
    queue_.push_back(x);
    notEmpty_.Notify();
  }

  void Put(T&& x) {
    MutexLockGuard guard(lock_);
    while (queue_.full()) {
      notFull_.Wait();
    }
    assert(!queue_.full());
    queue_.push_back(std::move(x));
    notEmpty_.Notify();
  }

  T Take() {
    MutexLockGuard guard(lock_);
    while (queue_.empty()) {
      notEmpty_.Wait();
    }
    T front(std::move(queue_.front()));
    queue_.pop_front();
    notFull_.Notify();
    return front;
  }

  size_t Size() const {
    MutexLockGuard guard(lock_);
    return queue_.size();
  }

  bool Empty() const {
    MutexLockGuard guard(lock_);
    return queue_.empty();
  }

  bool Full() const {
    MutexLockGuard guard(lock_);
    return queue_.full();
  }

  size_t Capacity() const {
    MutexLockGuard guard(lock_);
    return queue_.capacity();
  }

 private:
  // 固定大小的循环队列. 使用vector的简易实现.
  // 使用者应保证队列满时不调用push_back(), 队列空时不调用pop_front().
  // 非线程安全的.
  template <typename S>
  class CircularQueue {
   public:
    explicit CircularQueue(int maxSize)
        : beginIndex_(0),
          endIndex_(0),
          size_(0),
          count_(0),
          vec_() {
      vec_.reserve(maxSize);
    }

    void push_back(const T& x) {
      if (count_ < vec_.capacity()) {
        vec_.push_back(x);
        ++count_;
      } else {
        vec_[endIndex_] = x;
      }
      endIndex_ = (endIndex_ + 1) % vec_.capacity();
      ++size_;
    }

    void push_back(T&& x) {
      if (count_ < vec_.capacity()) {
        vec_.push_back(std::move(x));
        ++count_;
      } else {
        vec_[endIndex_] = std::move(x);
      }
      endIndex_ = (endIndex_ + 1) % vec_.capacity();
      ++size_;
    }

    void pop_front() {
      beginIndex_ = (beginIndex_ + 1) % vec_.capacity();
      --size_;
    }

    T& front() { return vec_[beginIndex_]; }
    const T& front() const { return vec_[beginIndex_]; }

    size_t size() const { return size_; }

    size_t capacity() const { return vec_.capacity(); }

    bool empty() const { return size_ == 0; }
    bool full() const { return size_ == vec_.capacity(); }

   private:
    size_t beginIndex_;  // 指向队首元素
    size_t endIndex_;  // 指向尾后元素
    size_t size_;
    int count_;
    std::vector<S> vec_;
  };

  mutable MutexLock lock_;
  Condition notEmpty_;
  Condition notFull_;
  CircularQueue<T> queue_;  // 最好使用boost::circular_buffer<T>
};

}  // namespace wethands

#endif  // SRC_UTILS_BOUNDEDBLOCKINGQUEUE_H_
