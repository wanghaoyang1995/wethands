set(reactor_SRCS
  Channel.cc
  EventLoop.cc
  Poller.cc
  Timer.cc
  TimerQueue.cc
)

add_library(wethands_reactor OBJECT ${reactor_SRCS})

add_subdirectory(tests)
