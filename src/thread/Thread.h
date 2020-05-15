// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-12 23:56:29
// Description:

#ifndef SRC_THREAD_THREAD_H_
#define SRC_THREAD_THREAD_H_

#include <functional>
#include <string>
#include "src/thread/Atomic.h"
#include "src/thread/CountDownLatch.h"
#include "src/utils/Uncopyable.h"

namespace wethands {
// pthread线程的封装类. 不可拷贝.
class Thread : public Uncopyable {
 public:
  typedef std::function<void ()> ThreadFunc;

  explicit Thread(ThreadFunc func, const std::string& name);
  // 如果正在运行且没有join, 就将线程分离.
  ~Thread();

  // 线程开始执行. 在这里真正地创建了pthread线程.
  void Start();
  void Join();
  bool Started() const { return started_; }
  bool Joined() const { return joined_; }
  pid_t GetTid() const { return tid_; }
  pthread_t GetPhtreadId() const { return pthreadId_; }
  std::string GetName() const { return name_; }
  static int GetNumCreated() { return numCreated_.Get(); }

 private:
  ThreadFunc func_;
  std::string name_;
  bool started_;
  bool joined_;
  pid_t tid_;
  pthread_t pthreadId_;
  static AtomicInt32 numCreated_;  // 已创建线程数.
  CountDownLatch latch_;  // 用于线程创建时与子线程间的同步.
};

}  // namespace wethands

#endif  // SRC_THREAD_THREAD_H_
