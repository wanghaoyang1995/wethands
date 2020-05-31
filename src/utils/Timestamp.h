// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-09 16:36:43
// Description:

#ifndef SRC_UTILS_TIMESTAMP_H_
#define SRC_UTILS_TIMESTAMP_H_

#include <time.h>
#include <cstdint>
#include <cstring>
#include <string>

#include "src/utils/Copyable.h"
#include "src/utils/operators.h"

namespace wethands {

// 用于记录时刻的时间戳类. 值语义.
class Timestamp : public Copyable,
                  public LessThanComparable<Timestamp>,
                  public EqualityComparable<Timestamp> {
 public:
  Timestamp() : microsecondsSinceEpoch_(0) {}
  explicit Timestamp(int64_t microsecondsSinceEpoch)
    : microsecondsSinceEpoch_(microsecondsSinceEpoch) {}

  int64_t MicrosecondsSinceEpoch() const { return microsecondsSinceEpoch_; }

  // 获取时间戳对应的日历时间
  time_t SecondsSinceEpoch() const {
    return static_cast<time_t>(microsecondsSinceEpoch_ /
                               kMicrosecondsPerSecond);
  }

  // 将时间戳按天向下圆整.
  // 返回本地时间当天 00:00时刻的时间戳.
  Timestamp RoundByDay() const;

  // 返回格式化的字符串(本地时间). 形如 yyyyMMdd-HHmmss.
  // showMicroseconds 指明是否需要精确到微秒.
  std::string ToFormattedString(bool showMicroseconds = false) const;

  // 返回当前时刻的时间戳.
  static Timestamp Now();
  static Timestamp Invalid() { return Timestamp(); }

  static constexpr int kMicrosecondsPerSecond = 1000000;
  static constexpr int kNanosecondsPerMicrosecond = 1000;

 private:
  // 自1970年1月1日00:00时刻至当前时刻的微秒数
  int64_t microsecondsSinceEpoch_;
};

static_assert(sizeof(Timestamp) == sizeof(int64_t), "Unexpected size.");

inline bool operator<(const Timestamp& lhs, const Timestamp& rhs) {
  return lhs.MicrosecondsSinceEpoch() < rhs.MicrosecondsSinceEpoch();
}

inline bool operator==(const Timestamp& lhs, const Timestamp& rhs) {
  return lhs.MicrosecondsSinceEpoch() == rhs.MicrosecondsSinceEpoch();
}

// 返回high与low之间的秒数差.
// 用户需要保证 high > low, 否则返回为负值.
inline double SecondsDifference(Timestamp high, Timestamp low) {
  int64_t diff = high.MicrosecondsSinceEpoch() - low.MicrosecondsSinceEpoch();
  return static_cast<double>(diff) / Timestamp::kMicrosecondsPerSecond;
}

// 返回两时间差的 timespec 结构.
inline struct timespec TimeDifference(Timestamp high, Timestamp low) {
  struct timespec ts;
  ::memset(&ts, 0, sizeof(ts));
  int64_t msDiff = high.MicrosecondsSinceEpoch() - low.MicrosecondsSinceEpoch();
  ts.tv_sec = static_cast<decltype(ts.tv_sec)>(
              msDiff / Timestamp::kMicrosecondsPerSecond);
  ts.tv_nsec = static_cast<decltype(ts.tv_nsec)>(
               msDiff % Timestamp::kMicrosecondsPerSecond *
               Timestamp::kNanosecondsPerMicrosecond);
  return ts;
}

inline Timestamp AddSeconds(Timestamp timestamp, double seconds) {
  int64_t delta = static_cast<int64_t>(seconds *
                                       Timestamp::kMicrosecondsPerSecond);
  return Timestamp(timestamp.MicrosecondsSinceEpoch() + delta);
}

}  // namespace wethands

#endif  // SRC_UTILS_TIMESTAMP_H_
