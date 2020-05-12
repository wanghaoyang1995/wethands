// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-12 12:46:37
// Description:

#ifndef SRC_THREAD_ATOMIC_H_
#define SRC_THREAD_ATOMIC_H_

#include <stdint.h>
#include "src/utils/Uncopyable.h"

namespace wethands {
namespace details {

// 原子整型. 不可拷贝.
// 使用__ATOMIC_SEQ_CST内存模型.
// https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
template <typename T>
class AtomicInterger : public Uncopyable {
 public:
  AtomicInterger() : value_(0) {}

  T Get() {
    return __atomic_load_n(&value_, __ATOMIC_SEQ_CST);
  }

  T GetAndSet(T newValue) {
    return __atomic_exchange_n(&value_, newValue, __ATOMIC_SEQ_CST);
  }

  T GetAndAdd(T x) {
    return __atomic_fetch_add(&value, x, __ATOMIC_SEQ_CST);
  }

  T AddAndGet(T x) {
    return __atomic_add_fetch(&value, x, __ATOMIC_SEQ_CST);
  }

  T IncrementAndGet() {
    return AddAndGet(1);
  }

  T DecrementAndGet() {
    return AddAndGet(-1);
  }

 private:
  volatile T value_;
};

}  // namespace details

typedef details::AtomicInterger<int32_t> AtomicInt32;
typedef details::AtomicInterger<int64_t> AtomicInt64;

}  // namespace wethands


#endif  // SRC_THREAD_ATOMIC_H_