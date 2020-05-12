// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-12 20:27:03
// Description:

#ifndef SRC_THREAD_CONDITION_H_
#define SRC_THREAD_CONDITION_H_

#include <pthread.h>
#include <stdint.h>
#include "src/thread/Mutex.h"
#include "src/utils/Uncopyable.h"

// 检查返回值是否为0.
#define MCHECK(ret) { int errnum = ret; assert(errnum == 0); }

namespace wethands {

// UNIX条件变量的封装类. 不可拷贝.
class Condition : public Uncopyable {
 public:
  explicit Condition(MutexLock& lock) : lock_(lock) {
    MCHECK(pthread_cond_init(&cond_, nullptr));
  }

  ~Condition() {
    MCHECK(pthread_cond_destroy(&cond_));
  }

  // 等待直到条件出现.
  void Wait() {
    assert(lock_.LockedByThisThread());
    // 由于pthread_cond_wait会在内部解锁, 所以需要手动修改持有者
    lock_.UnassignHolder();
    MCHECK(pthread_cond_wait(&cond_, lock_.GetPthreadMutex()));
    lock_.AssignHolder();
  }

  // 等待microseconds微秒, 如果到期时条件还没有出现就返回true.
  bool WaitForSeconds(double seconds);

  // 至少唤醒一个线程.
  void Notify() {
    MCHECK(pthread_cond_signal(&cond_));
  }

  // 唤醒所有线程.
  void NotifyAll() {
    MCHECK(pthread_cond_broadcast(&cond_));
  }

 private:
  MutexLock& lock_;
  pthread_cond_t cond_;
};

}  // namespace wethands

#undef MCHECK

#endif  // SRC_THREAD_CONDITION_H_
