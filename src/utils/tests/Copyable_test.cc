#include "src/utils/Copyable.h"
#include "src/utils/Uncopyable.h"

#include <cstdio>
#include <cassert>

class Test1 : public wethands::Copyable {
 public:
  Test1() : x_(0) {}
  Test1(int x) : x_(x) {}
  ~Test1() = default;

  int getx() { return x_; }

 private:
  int x_;
};

class Test2 : public wethands::Uncopyable {
 public:
  Test2() : x_(0) {}
  Test2(int x) : x_(x) {}
  ~Test2() = default;

  int getx() { return x_; }

 private:
  int x_;
};

class Test3 : public Test2 {};

int main() {
  Test1 t1(10);
  Test1 t2 = t1;
  Test1 t3;
  assert(t3.getx() == 0);
  t3 = t2;
  assert(t3.getx() == 10);

  Test2 t4(20);
  //Test2 t5(t4);
  Test2 t6(30);
  //t6 = t4;

  Test3 t7;
  //Test3 t8 = t7;

  printf("All passed.\n");
  return 0;
}
