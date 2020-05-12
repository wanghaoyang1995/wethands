#include "src/thread/Condition.h"
#include <cstdio>
#include <thread>
#include "src/thread/CurrentThread.h"

using namespace wethands;

MutexLock lock;
Condition cond(lock);
bool ready = false;

void test1() {
  //CurrentThread::sleepUsec(1000000);
  MutexLockGuard guard(lock);
  while (!ready) { cond.Wait(); }
  printf("t1 continue.\n");
  ready = false;
}

void test2() {
  //CurrentThread::sleepUsec(1000000);
  {
    MutexLockGuard guard(lock);
    ready = true;
    printf("t2 lock then unlock.\n");
  }
  cond.Notify();
  printf("t2 notify.\n");
}

int main() {
  std::thread t1(test1);
  std::thread t2(test2);
  t1.join();
  t2.join();
  printf("Done.\n");
  return 0;
}
