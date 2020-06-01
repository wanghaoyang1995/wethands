#include "src/reactor/EventLoopThread.h"
#include <cstdio>
#include "src/thread/CurrentThread.h"

using namespace wethands;

int main() {/*
  {
    EventLoopThread loopThread1;
  }

  {
    EventLoopThread loopThread2;
    EventLoop* loop = loopThread2.StartLoop();
    loop->RunInLoop([](){
      printf("run in loopThread2.\n");
    });
  }*/
  EventLoopThread loopThread3;
  EventLoop* loop = loopThread3.StartLoop();
  loop->RunEvery(1.0, [](){
    printf("run in loopThread3.\n");
  });
  printf("ok.\n");
  CurrentThread::SleepUsec(10000000);
  printf("ok.\n");
  return 0;
}
