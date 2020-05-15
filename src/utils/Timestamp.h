// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-09 16:36:43
// Description:

#ifndef SRC_UTILS_TIMESTAMP_H_
#define SRC_UTILS_TIMESTAMP_H_

#include <time.h>
#include <cstdint>
#include <string>

#include "src/utils/Copyable.h"
#include "src/utils/operators.h"

namespace wethands {

// 用于记录时刻的时间戳类. 值语义.
class Timestamp : public wethands::Copyable,
                  public wethands::LessThanComparable<Timestamp>,
                  public wethands::EqualityComparable<Timestamp> {
 public:
  Timestamp() : microseconds_since_epoch_(0) {}
  explicit Timestamp(int64_t microseconds_since_epoch)
    : microseconds_since_epoch_(microseconds_since_epoch) {}

  int64_t MicrosecondsSinceEpoch() const { return microseconds_since_epoch_; }

  // 获取时间戳对应的日历时间
  time_t SecondsSinceEpoch() const {
    return static_cast<time_t>(microseconds_since_epoch_ /
                               kMicrosecondsPerSecond);
  }

  // 返回格式化的字符串(本地时间). 形如yyyy-MM-dd HH:mm:ss.
  // showMicroseconds 指明是否需要精确到微秒.
  std::string ToString(bool showMicroseconds) const;

  // 返回当前时刻的时间戳.
  static Timestamp Now();

  static const int kMicrosecondsPerSecond = 1000 * 1000;
  static const int kNanosecondsPerMicrosecond = 1000;

 private:
  // 自1970年1月1日00:00时刻至当前时刻的微秒数
  int64_t microseconds_since_epoch_;
};

inline bool operator<(const Timestamp& lhs, const Timestamp& rhs) {
  return lhs.MicrosecondsSinceEpoch() < rhs.MicrosecondsSinceEpoch();
}

inline bool operator==(const Timestamp& lhs, const Timestamp& rhs) {
  return lhs.MicrosecondsSinceEpoch() == rhs.MicrosecondsSinceEpoch();
}

}  // namespace wethands

#endif  // SRC_UTILS_TIMESTAMP_H_
