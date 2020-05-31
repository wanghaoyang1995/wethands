// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-27 20:30:09
// Description:

#include "src/reactor/EventLoop.h"
#include <unistd.h>
#include <sys/eventfd.h>
#include <utility>
#include "src/logger/Logger.h"

namespace wethands {
namespace details {

__thread EventLoop* t_loopOfCurrentThread = nullptr;

constexpr int kPollTimeoutsMs = 10000;  // 10秒.

// 创建并检查 eventfd.
// https://man7.org/linux/man-pages/man2/eventfd.2.html
int CreateEventfd() {
  int fd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
  if (fd < 0) {
    LOG_SYSFATAL << "eventfd() error.";
  }
  return fd;
}

}  // namespace details
}  // namespace wethands

using wethands::EventLoop;
using wethands::TimerIndex;

EventLoop::EventLoop()
    : lock_(),
      quit_(false),
      loopThreadId_(CurrentThread::Tid()),
      eventfd_(details::CreateEventfd()),
      eventfdChannel(new Channel(this, eventfd_)),
      poller_(new Poller(this)),
      activeChannels_(),
      pendingFunctors_(),
      timerQueue_(new TimerQueue(this)) {
  LOG_TRACE << "EventLoop created.";
  if (details::t_loopOfCurrentThread != nullptr) {
    LOG_FATAL << "Already exist a EventLoop in this thread.";
  }
  details::t_loopOfCurrentThread = this;
  eventfdChannel->SetReadCallback(
    std::bind(&EventLoop::HandleEventfdRead, this));
  eventfdChannel->EnableReading();
}

EventLoop::~EventLoop() {
  LOG_TRACE << "EventLoop destructed.";
  eventfdChannel->DisableAll();
  eventfdChannel->RemoveFromPoller();
  ::close(eventfd_);
  details::t_loopOfCurrentThread = nullptr;
}

void EventLoop::Loop() {
  assert(!quit_);
  assert(IsInLoopThread());
  LOG_TRACE << "Loop() started.";

  while (!quit_) {
    activeChannels_.clear();
    poller_->Poll(details::kPollTimeoutsMs, &activeChannels_);
    for (Channel* channel : activeChannels_) {
      channel->HandleEvent();
    }
    // 执行待处理的任务.
    for (const Functor& func : pendingFunctors_) {
      func();
    }
    pendingFunctors_.clear();
  }
}

void EventLoop::Quit() {
  quit_ = true;
  if (!IsInLoopThread()) {
    Wakeup();
  }
}

void EventLoop::RunInLoop(Functor cb) {
  if (IsInLoopThread()) {
    cb();
  } else {
    QueueInLoop(std::move(cb));
  }
}

void EventLoop::QueueInLoop(Functor cb) {
  {
    MutexLockGuard guard(lock_);
    pendingFunctors_.push_back(std::move(cb));
  }
  if (!IsInLoopThread()) Wakeup();
}

TimerIndex EventLoop::RunAt(Timestamp time, Timer::TimerCallback cb) {
  return timerQueue_->AddTimer(std::move(cb), time, 0.0);
}

TimerIndex EventLoop::RunAfter(double delay, Timer::TimerCallback cb) {
  Timestamp time = AddSeconds(Timestamp::Now(), delay);
  return timerQueue_->AddTimer(std::move(cb), time, 0.0);
}

TimerIndex EventLoop::RunEvery(double interval, Timer::TimerCallback cb) {
  Timestamp time = AddSeconds(Timestamp::Now(), interval);
  return timerQueue_->AddTimer(std::move(cb), time, interval);
}

void EventLoop::CancelTimer(TimerIndex timerIndex) {
  timerQueue_->CancelTimer(timerIndex);
}

void EventLoop::UpdateChannel(Channel* channel) {
  assert(channel->OwerLoop() == this);
  assert(IsInLoopThread());
  poller_->UpdateChannel(channel);
}

void EventLoop::RemoveChannel(Channel* channel) {
  assert(channel->OwerLoop() == this);
  assert(IsInLoopThread());
  poller_->RemoveChannel(channel);
}

bool EventLoop::HasChannel(Channel* channel) {
  assert(IsInLoopThread());
  return poller_->HasChannel(channel);
}

EventLoop* EventLoop::CurrentThreadLoop() {
  return details::t_loopOfCurrentThread;
}

void EventLoop::HandleEventfdRead() {
  uint64_t buf;
  ssize_t n = ::read(eventfd_, &buf, sizeof(buf));
  if (n != sizeof(buf)) {
    LOG_ERROR << "HandleEventfdRead(): unexcepted bytes read.";
  }
}

void EventLoop::Wakeup() {
  uint64_t buf = 1;
  ssize_t n = ::write(eventfd_, &buf, sizeof(buf));
  if (n != sizeof(buf)) {
    LOG_ERROR << "Wakeup(): unexcepted bytes writen.";
  }
}
