#include "src/utils/WeakCallback.h"
#include <cstdio>
#include <memory>
#include <string>
#include "src/reactor/EventLoop.h"

using namespace wethands;

class A {
 public:
  explicit A(int i) : i_(i) {}
  ~A() = default;

  void Print(const std::string& str) {
    printf("%s, %d\n", str.c_str(), i_);
  }

 private:
  int i_;
};

int main() {
  EventLoop loop;
  {
    std::shared_ptr<A> sp = std::make_shared<A>(99);
    loop.RunAfter(2.0, std::bind(MakeWeakCallback(sp, &A::Print), "hello"));
  }
  {
    std::shared_ptr<A> sp = std::make_shared<A>(88);
    loop.RunAfter(3.0, std::bind(&A::Print, sp, "hello"));
  }

  loop.Loop();
  return 0;
}