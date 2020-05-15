// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-12 23:56:39
// Description:

#include "src/thread/Thread.h"
#include <cassert>
#include <cstdio>
#include <utility>
#include "src/thread/CurrentThread.h"
#include "src/logger/Logger.h"

namespace wethands {
namespace details {

class ThreadData {
 public:
  ThreadData(Thread::ThreadFunc func,
             std::string name,
             pid_t* tid,
             CountDownLatch* latch)
      : func_(func),
        name_(name),
        tid_(tid),
        latch_(latch) {}

  void RunInThread() {
    *tid_ = CurrentThread::Tid();  // 更新父线程中的tid_.
    latch_->CountDown();  // 唤醒父线程.
    CurrentThread::t_name = name_.c_str();
    func_();  // 在这里调用了线程回调函数
  }

 private:
  Thread::ThreadFunc func_;
  std::string name_;
  pid_t* tid_;  // 指向 父线程中 Thread对象中存储的子线程id.
  CountDownLatch* latch_;
};

// 用作传递给pthread_create函数的第三个参数.
void* pthreadFunc(void* arg) {
  ThreadData* data = static_cast<details::ThreadData*>(arg);
  data->RunInThread();
  delete data;  // 重要.
}

}  // namespace details
}  // namespace wethands

using wethands::Thread;

wethands::AtomicInt32 Thread::numCreated_;

Thread::Thread(ThreadFunc func, const std::string& name)
    : func_(std::move(func)),
      name_(name),
      started_(false),
      joined_(false),
      tid_(0),
      pthreadId_(0),
      latch_(1) {
  int num = numCreated_.IncrementAndGet();  // 递增计数
  if (name.empty()) {
    name_ = "Thread" + std::to_string(num);  // 确保name_不为空
  }
}

Thread::~Thread() {
  if (started_ && !joined_) {
    pthread_detach(pthreadId_);
  }
}

void Thread::Start() {
  using details::ThreadData;
  started_ = true;
  // 用data给子线程传递信息, 注意这里使用了new, 记得delete.
  ThreadData* data = new ThreadData(func_, name_, &tid_, &latch_);
  if (pthread_create(&pthreadId_, nullptr, details::pthreadFunc, data) != 0) {
    started_ = false;
    delete data;
    LOG_SYSFATAL << "pthread_create failed.";
  } else {
    latch_.Wait();
    assert(tid_ > 0);
  }
}

void Thread::Join() {
  assert(started_ == true);
  assert(joined_ == false);
  joined_ = true;
  pthread_join(pthreadId_, nullptr);
}
