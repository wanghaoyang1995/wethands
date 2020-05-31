// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-23 21:31:56
// Description:

#include "src/reactor/Poller.h"
#include <errno.h>
#include <unistd.h>
#include <cstring>
#include "src/reactor/EventLoop.h"
#include "src/logger/Logger.h"
#include "src/utils/Timestamp.h"

using wethands::Poller;
using wethands::Timestamp;

Poller::Poller(EventLoop* loop)
    : loop_(loop),
      epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
      channels_(),
      events_(kEventListSize_) {
  if (epollfd_ < 0) {
    LOG_SYSFATAL << "epoll_create1() error.";
  }
}

Poller::~Poller() {
  ::close(epollfd_);
}

Timestamp Poller::Poll(int timeout, std::vector<Channel*>* activeChannels) {
  assert(loop_->IsInLoopThread());
  LOG_TRACE << "epoll_wait: events_.size() = " << events_.size()
            << ", timeout = " << timeout;

  int numEvents = ::epoll_wait(epollfd_,
                               events_.data(),
                               static_cast<int>(events_.size()),
                               timeout);
  int savedErrno = errno;
  Timestamp now = Timestamp::Now();
  LOG_TRACE << "epoll_wait return: numEvents = " << numEvents;

  // 将被激活的 Channel 放入activeChannels.
  for (int i = 0; i < numEvents; ++i) {
    Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
    activeChannels->push_back(channel);
  }
  // 如果 events_ 用满了, 就加倍扩容.
  if (numEvents == events_.size()) events_.resize(events_.size() * 2);
  errno = savedErrno;
  return now;
}

void Poller::UpdateChannel(Channel* channel) {
  assert(loop_->IsInLoopThread());
  struct epoll_event ev;
  ::memset(&ev, 0, sizeof(ev));
  ev.events = channel->Events();
  // ev.data.fd = channel->Fd();
  ev.data.ptr = channel;  // 将 Channel 指针存放data.

  // 添加或更新.
  int op = channel->Registered() ? EPOLL_CTL_MOD: EPOLL_CTL_ADD;
  LOG_TRACE << "UpdateChannel(): fd = " << channel->Fd();
  int ret = ::epoll_ctl(epollfd_, op, channel->Fd(), &ev);
  if (ret < 0) {
    LOG_SYSFATAL << "epoll_ctl() error.";
  }

  channels_[channel->Fd()] = channel;
}

void Poller::RemoveChannel(Channel* channel) {
  assert(loop_->IsInLoopThread());
  LOG_TRACE << "RemoveChannel(): fd = " << channel->Fd();
  int ret = ::epoll_ctl(epollfd_, EPOLL_CTL_DEL, channel->Fd(), nullptr);
  if (ret == ENOENT) {  // 试图删除未注册的 Channel.
    LOG_SYSERROR << "epoll_ctl() return ENOENT.";
  } else if (ret < 0) {
    LOG_SYSFATAL << "epoll_ctl() error.";
  }

  channels_.erase(channel->Fd());
}

bool Poller::HasChannel(Channel* channel) {
  assert(loop_->IsInLoopThread());
  std::map<int, Channel*>::iterator it = channels_.find(channel->Fd());
  return it != channels_.end() && it->second == channel;
}
