#include "src/thread/Mutex.h"
#include <cstdio>
#include <cassert>
#include <thread>

int g_i = 0;
wethands::MutexLock lock;

void test() {
  for (int i = 0; i < 100000; ++i) {
    wethands::MutexLockGuard guard(lock);
    g_i++;
  }
}

int main() {
  std::thread t1(test);
  std::thread t2(test);
  t1.join();
  t2.join();
  printf("%d\n", g_i);
  assert(g_i == 200000);
  return 0;
}
