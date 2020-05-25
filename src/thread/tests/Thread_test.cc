#include "src/thread/Thread.h"
#include <cstdio>
#include <cassert>
#include <stdexcept>

using namespace wethands;

void test() {
  printf("tid = %d, name = %s\n", CurrentThread::Tid(), CurrentThread::Name());
}

int main() {
  {
    Thread t1(test, "t1");
    t1.Start();
  }
  {
    Thread t2(test, "");
    t2.Start();
  }
  {
    Thread t3(test, "t3");
  }

  Thread t4(test, "t4");
  t4.Start();
  t4.Join();

  assert(t4.Joined() == true);
  assert(t4.Started() == true);

  assert(Thread::NumCreated() == 4);
  return 0;
}
