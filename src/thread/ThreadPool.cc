// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-15 19:41:20
// Description:

#include "src/thread/ThreadPool.h"
#include <cassert>

using wethands::ThreadPool;

ThreadPool::ThreadPool(const std::string& name)
    : name_(name),
      lock_(),
      notEmpty_(lock_),
      notFull_(lock_),
      maxQueueSize_(0),
      running_(false) {}

ThreadPool::~ThreadPool() {
  if (running_) {
    Stop();
  }
}

void ThreadPool::Start(int numThreads) {
  assert(numThreads >= 0);
  running_ = true;
  threads_.reserve(numThreads);
  for (int i = 0; i < numThreads; ++i) {
    threads_.emplace_back(new Thread(std::bind(&ThreadPool::RunInThread, this),
                                     name_ + std::to_string(i + 1)));
    threads_[i]->Start();
  }
  if (numThreads == 0 && initCallback_) initCallback_();
}

void ThreadPool::Stop() {
  {
    MutexLockGuard guard(lock_);
    running_ = false;
  }
  notEmpty_.NotifyAll();
  for (auto& thread : threads_) {
    thread->Join();
  }
}

int ThreadPool::QueueSize() const {
  MutexLockGuard guard(lock_);
  return queue_.size();
}

void ThreadPool::SetInitCallback(const Task& cb) {
  initCallback_ = cb;
}

void ThreadPool::SetMaxQueueSize(int size) {
  maxQueueSize_ = size;
}

bool ThreadPool::QueueIsFull() const {
  assert(lock_.LockedByThisThread());  // 必须确保已上锁
  return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_;
}

void ThreadPool::Run(Task task) {
  if (threads_.empty()) {
    task();
  } else {
    MutexLockGuard guard(lock_);
    while (QueueIsFull()) {
      notFull_.Wait();
    }
    queue_.push_back(std::move(task));
    notEmpty_.Notify();
  }
}

ThreadPool::Task ThreadPool::Take() {
  MutexLockGuard guard(lock_);
  while (queue_.empty() && running_) {
    notEmpty_.Wait();
  }
  if (queue_.empty()) return Task();  // queue_为空说明消费者被Stop唤醒.

  Task task = queue_.front();
  queue_.pop_front();
  // 如果队列有上限(说明生产者可能阻塞), 就通知一下生产者继续生产.
  if (maxQueueSize_ > 0) {
    notFull_.Notify();
  }
  return task;
}

void ThreadPool::RunInThread() {
  if (initCallback_) {
    initCallback_();
  }
  while (running_) {
    Task task = Take();
    if (task) task();
  }
}
