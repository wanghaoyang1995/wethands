#include "src/utils/BlockingQueue.h"
#include "src/utils/BoundedBlockingQueue.h"
#include <cassert>
#include <cstdio>
#include <memory>
#include "src/thread/CurrentThread.h"
#include "src/utils/Timestamp.h"
#include "src/thread/Thread.h"

using namespace wethands;

void test1() {
  BlockingQueue<int> q1;
  BoundedBlockingQueue<int> q2(50);
  Thread t1([&q1, &q2](){
    Timestamp start = Timestamp::Now();
    for (int i = 0; i < 1000; ++i) {
      q1.Put(i);
      q2.Put(i);
    }
    Timestamp end = Timestamp::Now();
    printf("t1: %f\n", SecondsDifference(end, start));
  }, "t1");

  Thread t2([&q1, &q2](){
    Timestamp start = Timestamp::Now();
    for (int i = 0; i < 1000; ++i) {
      q1.Take();
      q2.Take();
    }
    Timestamp end = Timestamp::Now();
    printf("t2: %f\n", SecondsDifference(end, start));
  }, "t2");
  t1.Start();
  t2.Start();
  t1.Join();
  t2.Join();
}

void test2() {
  using Ptr = std::unique_ptr<int> ;
  BlockingQueue<Ptr> q1;
  BoundedBlockingQueue<Ptr> q2(50);
  Thread t1([&q1, &q2](){
    Timestamp start = Timestamp::Now();
    for (int i = 0; i < 1000; ++i) {
      q1.Put(Ptr(new int(i)));
      q2.Put(Ptr(new int(i)));
    }
    Timestamp end = Timestamp::Now();
    printf("t1: %f\n", SecondsDifference(end, start));
  }, "t1");

  Thread t2([&q1, &q2](){
    Timestamp start = Timestamp::Now();
    for (int i = 0; i < 1000; ++i) {
      q1.Take();
      q2.Take();
    }
    Timestamp end = Timestamp::Now();
    printf("t2: %f\n", SecondsDifference(end, start));
  }, "t2");
  t1.Start();
  t2.Start();
  t1.Join();
  t2.Join();
}

int main() {
  test1();
  test2();
  return 0;
}
