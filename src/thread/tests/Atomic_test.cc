#include "src/thread/Atomic.h"
#include <cstdio>
#include <thread>
#include <cassert>

wethands::AtomicInt32 g_atomic_i;
int g_i;

void test() {
  for (int i = 0; i < 50; ++i) {
    g_atomic_i.DecrementAndGet();
    g_i;
  }
}

int main() {
  g_atomic_i.GetAndSet(100);
  g_i = 100;

  std::thread t1(test);
  std::thread t2(test);
  t1.join();
  t2.join();
  assert(g_atomic_i.Get() == 0);
  printf("g_atomic_i = %d, g_i = %d\n", g_atomic_i.Get(), g_i);
  return 0;
}