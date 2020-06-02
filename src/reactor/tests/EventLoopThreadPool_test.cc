#include "src/reactor/EventLoopThreadPool.h"
#include "src/reactor/EventLoop.h"
#include "src/thread/CurrentThread.h"
#include "src/logger/Logger.h"
#include <cassert>
#include <cstdio>

using namespace wethands;

void init() {
  printf("init, tid = %d, name = %s\n",
    CurrentThread::Tid(), CurrentThread::Name());
}

void test() {
  printf("test, tid = %d, name = %s\n",
    CurrentThread::Tid(), CurrentThread::Name());
}

int main() {
  Logger::SetLogLevel(Logger::LogLevel::DEBUG);
  EventLoop loop;
  EventLoopThreadPool pool1(&loop);

  pool1.Start(1, init);
  assert(pool1.AllLoops().size() == 1);
  for (int i = 0; i < 500; ++i) {
    EventLoop* nextLoop = pool1.NextLoop();
    nextLoop->RunInLoop(test);
  }

  loop.RunAfter(10.0, [&loop](){
    printf("quit\n");
    loop.Quit();
  });
  loop.Loop();
  return 0;
}
