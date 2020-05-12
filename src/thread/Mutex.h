// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-12 17:06:06
// Description:

#ifndef SRC_THREAD_MUTEX_H_
#define SRC_THREAD_MUTEX_H_


#include <pthread.h>
#include <cassert>
#include "src/thread/CurrentThread.h"
#include "src/utils/Uncopyable.h"
// 断言函数返回值为0. 仅限本文件内使用.
#define MCHECK(ret) { int errnum = ret; assert(errnum == 0); }

namespace wethands {

// 对linux系统互斥锁的封装. 不可拷贝.
class MutexLock : public Uncopyable {
 public:
  MutexLock() : holder_(0) {
    MCHECK(pthread_mutex_init(&mutex_, nullptr));
  }

  ~MutexLock() {
    assert(holder_ == 0);
    MCHECK(pthread_mutex_destroy(&mutex_));
  }

  void Lock() {
    MCHECK(pthread_mutex_lock(&mutex_));
    AssignHolder();
  }

  void Unlock() {
    UnassignHolder();
    MCHECK(pthread_mutex_unlock(&mutex_));
  }

  bool LockedByThisThread() { return holder_ == CurrentThread::Tid(); }
  pthread_mutex_t* GetPthreadMutex() { return &mutex_; }

  void AssignHolder() { holder_ = CurrentThread::Tid(); }
  void UnassignHolder() { holder_ = 0; }

 private:
  pthread_mutex_t mutex_;
  pid_t holder_;  // 锁持有者的线程id.
};

// MutexLock的RAII包装.
class MutexLockGuard : public Uncopyable {
 public:
  explicit MutexLockGuard(MutexLock& lock) : lock_(lock) { lock_.Lock(); }
  ~MutexLockGuard() { lock_.Unlock(); }

 private:
  MutexLock& lock_;
};

}  // namespace wethands

#undef MCHECK

// 错误用法 MutexLockGuard(lock);
// 正确的用法是 MutexLockGuard guard(lock);
#define MutexLockGuard(x) wrong_usage!

#endif  // SRC_THREAD_MUTEX_H_
