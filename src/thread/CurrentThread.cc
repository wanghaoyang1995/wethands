// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-11 22:59:58
// Description:

#include "src/thread/CurrentThread.h"
#include <unistd.h>
#include <sys/syscall.h>
#include <time.h>
#include <type_traits>

// 确保 pid_t 同 int 类型
static_assert(std::is_same<int, pid_t>::value, "pid_t should be int");

namespace wethands {
namespace CurrentThread {
namespace details {

// 为主线程设置初始信息.
class MainThreadInitializer {
 public:
  MainThreadInitializer() {
    CurrentThread::Tid();
    CurrentThread::t_name = "MainThread";
  }
};
// 一个全局变量, 主线程调用它的构造函数时完成初始信息设置.
MainThreadInitializer gInitializer;

}  // namespace details

__thread int t_tid = 0;  // 0代表没有缓存.
__thread const char* t_name = "unknown";

}  // namespace CurrentThread
}  // namespace wethands

// 显示地指出要缓存tid.
int wethands::CurrentThread::Tid() {
  if (t_tid == 0)
    t_tid = static_cast<pid_t>(::syscall(SYS_gettid));
  return t_tid;
}

const char* wethands::CurrentThread::Name() {
  return t_name;
}

bool wethands::CurrentThread::IsMainThread() {
  return Tid() == ::getpid();
}

void wethands::CurrentThread::SleepUsec(int64_t usec) {
  struct timespec ts;
  ts.tv_sec = static_cast<decltype(ts.tv_sec)>(usec / 1000000);
  ts.tv_nsec = static_cast<decltype(ts.tv_nsec)>(usec % 1000000 * 1000);
  ::nanosleep(&ts, nullptr);
}
