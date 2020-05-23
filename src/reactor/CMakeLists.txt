set(reactor_SRCS
  Timer.cc
)

add_library(wethands_reactor OBJECT ${reactor_SRCS})

add_subdirectory(tests)
