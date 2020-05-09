// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-09 16:36:50
// Description:

#include "src/utils/Timestamp.h"

#include <time.h>
#include <sys/time.h>

using std::string;
using wethands::Timestamp;

string Timestamp::ToString() const {
  time_t timer = SecondsSinceEpoch();
  struct tm tm_time;
  //::gmtime_r(&timer, &tm_time);  // UTC时间
  ::localtime_r(&timer, &tm_time);  // 日历时间转为本地时间
  char buf[32];
  ::strftime(buf, sizeof(buf), "%F %T", &tm_time);
  return buf;
}

Timestamp Timestamp::Now() {
  struct timespec ts;
  ::clock_gettime(CLOCK_REALTIME, &ts);
  return Timestamp(ts.tv_sec * Timestamp::kMicrosecondsPerSecond +
                   ts.tv_nsec / Timestamp::kNanosecondsPerMicrosecond);
}
