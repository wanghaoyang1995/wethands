// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-12 00:42:12
// Description:

#include "src/thread/CurrentThread.h"
#include <cassert>
#include <cstdio>
#include <thread>

using namespace wethands;

void test() {
  printf("name: %s\n", CurrentThread::Name());
  printf("tid = %d\n", CurrentThread::Tid());
  printf("Is main thread : %d\n\n", CurrentThread::IsMainThread());
}

int main() {
  assert(CurrentThread::IsMainThread() == true);
  CurrentThread::SleepUsec(2 * 1000 * 1000);
  test();

  std::thread t1(test);
  std::thread t2(test);
  t1.join();
  t2.join();

  printf("All done.\n");
  return 0;
}
