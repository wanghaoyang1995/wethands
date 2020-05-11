// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-11 22:59:58
// Description:

#include "src/thread/CurrentThread.h"
#include <unistd.h>
#include <sys/syscall.h>
#include <time.h>
#include <type_traits>
#include "src/utils/Timestamp.h"

namespace wethands {
namespace CurrentThread {

// 确保 pid_t 同 int 类型
static_assert(std::is_same<int, pid_t>::value, "pid_t should be int");

__thread int t_tid = 0;  // 0代表没有缓存.

}  // namespace CurrentThread
}  // namespace wethands

// 显示地指出要缓存tid.
int wethands::CurrentThread::Tid() {
  if (t_tid == 0)
    t_tid = static_cast<pid_t>(::syscall(SYS_gettid));
  return t_tid;
}

bool wethands::CurrentThread::IsMainThread() {
  return Tid() == ::getpid();
}

void wethands::CurrentThread::sleepUsec(int64_t usec) {
  struct timespec ts;
  ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicrosecondsPerSecond);
  ts.tv_nsec = static_cast<long>(usec % Timestamp::kMicrosecondsPerSecond *
                                        Timestamp::kNanosecondsPerMicrosecond);
  ::nanosleep(&ts, nullptr);
}
