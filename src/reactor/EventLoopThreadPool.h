// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-06-02 01:21:04
// Description:

#ifndef SRC_REACTOR_EVENTLOOPTHREADPOOL_H_
#define SRC_REACTOR_EVENTLOOPTHREADPOOL_H_

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "src/reactor/EventLoop.h"
#include "src/reactor/EventLoopThread.h"
#include "src/utils/Uncopyable.h"

namespace wethands {
// loop线程池.
// 只能开启一次, 关闭后不支持重启.
class EventLoopThreadPool : public Uncopyable {
 public:
  using InitCallback = std::function<void ()>;

  EventLoopThreadPool(EventLoop* baseLoop,
    const std::string& name = std::string("EventLoopThreadPool"));
  // EventLoopThread可以自行析构, 这里使用默认析构即可.
  ~EventLoopThreadPool() = default;

  const std::string& Name() const { return name_; }
  bool Started() const { return started_; }

  void Start(int numThreads, InitCallback cb = InitCallback());
  EventLoop* NextLoop();
  std::vector<EventLoop*> AllLoops();

 private:
  EventLoop* baseLoop_;
  bool started_;
  size_t next_;
  std::string name_;
  std::vector<std::unique_ptr<EventLoopThread>> threads_;
  std::vector<EventLoop*> loops_;
};

}  // namespace wethands

#endif  // SRC_REACTOR_EVENTLOOPTHREADPOOL_H_
