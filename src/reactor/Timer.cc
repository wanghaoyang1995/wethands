// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-22 22:54:36
// Description:

#include "src/reactor/Timer.h"

using wethands::Timer;

wethands::AtomicInt64 Timer::count_(0);

void Timer::Restart() {
  if (IsRepeated()) {
    expiration_ = wethands::AddSeconds(expiration_, interval_);
  } else {
    expiration_ = Timestamp::Invalid();
  }
}
