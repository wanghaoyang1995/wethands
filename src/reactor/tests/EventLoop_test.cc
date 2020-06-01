#include "src/reactor/EventLoop.h"
#include <cstdio>
#include <functional>
#include "src/logger/Logger.h"
#include "src/thread/Thread.h"

using namespace wethands;

void print(int i) {
  printf("%d\n", i);
}

int main() {
  //Logger::SetLogLevel(Logger::LogLevel::TRACE);
  EventLoop loop;
  assert(EventLoop::CurrentThreadLoop() == &loop);
  loop.RunAfter(1.0, std::bind(print, 2));
  loop.RunEvery(2.0, [](){
    printf("hello\n");
  });

  loop.RunAfter(20.0, [&loop](){
    loop.Quit();
  });

  Thread t1([&loop](){
    loop.RunInLoop([](){
      printf("t1\n");
    });
  }, "t1");
  t1.Start();
  loop.Loop();

  t1.Join();
  return 0;
}
