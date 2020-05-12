#include "src/thread/Atomic.h"
#include <cstdio>
#include <thread>
#include <cassert>

wethands::AtomicInt32 g_atomic_i;
int g_i = 0;

void test1() {
  for (int i = 0; i < 100000; ++i) {
    g_atomic_i.IncrementAndGet();
  }
}

void test2() {
  for (int i = 0; i < 100000; ++i) {
    g_i++;
  }
}

int main() {
  std::thread t1(test1);
  std::thread t2(test1);
  std::thread t3(test2);
  std::thread t4(test2);
  t1.join();
  t2.join();
  t3.join();
  t4.join();
  assert(g_atomic_i.Get() == 200000);
  printf("g_atomic_i = %d, g_i = %d\n", g_atomic_i.Get(), g_i);
  return 0;
}
