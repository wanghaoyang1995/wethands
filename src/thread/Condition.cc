// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-12 20:27:14
// Description:

#include "src/thread/Condition.h"
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdint.h>

using wethands::Condition;

bool Condition::WaitForSeconds(double seconds) {
  assert(lock_.LockedByThisThread());
  const int64_t kNanosecondsPerSecond = 1000 * 1000 * 1000;

  struct timespec ts;
  ::clock_gettime(CLOCK_REALTIME, &ts);  // 获取绝对时间.

  int64_t nanoseconds =
    static_cast<int64_t>(seconds * kNanosecondsPerSecond) + ts.tv_nsec;
  ts.tv_sec +=
    static_cast<decltype(ts.tv_sec)>(nanoseconds / kNanosecondsPerSecond);
  ts.tv_nsec =
    static_cast<decltype(ts.tv_nsec)>(nanoseconds % kNanosecondsPerSecond);

  // pthread_cond_timedwait 第三个参数要求的是绝对时间.
  return ETIMEDOUT ==
         ::pthread_cond_timedwait(&cond_, lock_.GetPthreadMutex(), &ts);
}
