#include "src/utils/Singleton.h"
#include "src/thread/Thread.h"
#include "src/thread/CurrentThread.h"
#include <cstdio>

using namespace wethands;

class A {
 public:
  A() : i_(0) {}
  ~A() = default;

  void set(int i) { i_ = i; }
  void print() { printf("i_ = %d, %p\n", i_, this); }

 private:
  int i_;
};

void test() {
  printf("tid = %d, ", CurrentThread::Tid());
  Singleton<A>::GetInstance().print();
}

int main() {
  Singleton<A>::GetInstance().set(99);
  Thread t1(test, "t1");
  Thread t2(test, "t2");
  Thread t3(test, "t3");
  Thread t4(test, "t4");

  t1.Start();
  t2.Start();
  t3.Start();
  t4.Start();

  t1.Join();
  t2.Join();
  t3.Join();
  t4.Join();

  return 0;
}
