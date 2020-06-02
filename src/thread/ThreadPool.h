// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-15 19:41:13
// Description:

#ifndef SRC_THREAD_THREADPOOL_H_
#define SRC_THREAD_THREADPOOL_H_

#include <deque>
#include <functional>
#include <vector>
#include <memory>
#include <string>
#include "src/thread/Condition.h"
#include "src/thread/Mutex.h"
#include "src/thread/Thread.h"
#include "src/utils/Uncopyable.h"

namespace wethands {
// 线程池.
// 只能 开启-关闭 一次, 关闭后不支持重启.
class ThreadPool : public Uncopyable {
 public:
  using Task = std::function<void ()>;

  explicit ThreadPool(const std::string& name = std::string("ThreadPool"));
  ~ThreadPool();

  void Start(int numThreads);
  // Stop() 后应销毁, 不可再调用 Start().
  void Stop();

  const std::string& Name() const { return name_; }
  int QueueSize() const;

  // 内部未加锁, 必须在Start()之前调用.
  void SetInitCallback(const Task& cb);
  // 内部未加锁, 必须在Start()之前调用.
  void SetMaxQueueSize(int size);

  // 以下两种情况时返回false:
  // 1. maxQueueSize_小于等于0, 此时相当于任务队列不设上限;
  // 2. maxQueueSize_大于0, 且队列大小不超过maxQueueSize_.
  bool QueueIsFull() const;
  // 向任务队列中放置任务. 队列满时会阻塞.
  // 如果在Start()之前调用, 会由主线程调用task.
  void Run(Task task);

 private:
  Task Take();  // 从任务队列中取出任务. 队列空时会阻塞.
  void RunInThread();  // 在工作线程内执行. 从队列中不断取出任务并运行.

  std::string name_;
  mutable MutexLock lock_;
  Condition notEmpty_;
  Condition notFull_;
  Task initCallback_;
  std::vector<std::shared_ptr<Thread>> threads_;  // 消费者线程.
  std::deque<Task> queue_;
  int maxQueueSize_;  // 0或负数代表不设上限.
  bool running_;
};

}

#endif  // SRC_THREAD_THREADPOOL_H_
