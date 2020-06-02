// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-02 01:21:20
// Description:

#include "src/reactor/EventLoopThreadPool.h"
#include <cassert>

using wethands::EventLoop;
using wethands::EventLoopThreadPool;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop,
                                         const std::string& name)
    : baseLoop_(baseLoop),
      started_(false),
      next_(0),
      name_(name),
      threads_(),
      loops_() {}

void EventLoopThreadPool::Start(int numThreads, InitCallback cb) {
  assert(!started_);
  assert(baseLoop_->IsInLoopThread());
  for (int i = 0; i < numThreads; ++i) {
    threads_.emplace_back(
      new EventLoopThread(cb, "EventLoopThread" + std::to_string(i + 1)));
    loops_.push_back(threads_[i]->StartLoop());
  }
  started_ = true;

  if (numThreads <= 0 && cb) {
    cb();
  }
}

EventLoop* EventLoopThreadPool::NextLoop() {
  assert(started_);
  assert(baseLoop_->IsInLoopThread());
  if (loops_.empty()) return baseLoop_;

  if (next_ == loops_.size()) next_ = 0;
  return loops_[next_++];
}

std::vector<EventLoop*> EventLoopThreadPool::AllLoops() {
  assert(started_);
  assert(baseLoop_->IsInLoopThread());
  if (loops_.empty()) return { baseLoop_ };

  return loops_;
}
