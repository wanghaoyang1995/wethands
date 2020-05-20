// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-12 23:23:12
// Description:

#include "src/thread/CountDownLatch.h"
#include <cassert>
#include <thread>
#include "src/thread/CurrentThread.h"
#include "src/utils/Timestamp.h"

using namespace wethands;

CountDownLatch latch(3);

void func1() {
  CurrentThread::SleepUsec(1000000);
  latch.CountDown();
}

void func2() {
  CurrentThread::SleepUsec(2000000);
  latch.CountDown();
}

void func3() {
  CurrentThread::SleepUsec(3000000);
  latch.CountDown();
}

int main() {
  std::thread t1(func1);
  std::thread t2(func2);
  std::thread t3(func3);
  t1.detach();
  t2.detach();
  t3.detach();
  printf("[%s] Waiting threads.\n",
         Timestamp::Now().ToFormattedString(true).c_str());
  latch.Wait();
  assert(latch.GetCount() == 0);
  printf("[%s] All threads ready.\n",
         Timestamp::Now().ToFormattedString(true).c_str());
  return 0;
}
