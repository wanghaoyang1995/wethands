set(thread_SRCS
  CurrentThread.cc
  Condition.cc
  CountDownLatch.cc
)

add_library(wethands_thread OBJECT ${thread_SRCS})

add_subdirectory(tests)
