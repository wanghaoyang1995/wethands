#include "src/thread/ThreadPool.h"
#include <cstdio>
#include "src/thread/CurrentThread.h"

using namespace wethands;

void init() {
  printf("init(): thread id = %d, threadname = %s\n",
         CurrentThread::Tid(), CurrentThread::Name());
}

void threadfunc() {
  printf("threadfunc(): thread id = %d, threadname = %s taken one task.\n",
         CurrentThread::Tid(), CurrentThread::Name());
  CurrentThread::SleepUsec(4000000);
  printf("threadfunc(): thread id = %d, threadname = %s done the task.\n",
         CurrentThread::Tid(), CurrentThread::Name());
}

void threadfunc2(ThreadPool& pool) {
  CurrentThread::SleepUsec(1000000);
  printf("thread %d will stop the pool in one second.\n", CurrentThread::Tid());
  pool.Stop();
  printf("thread %d stoped the pool.\n", CurrentThread::Tid());
}

int main() {
  ThreadPool pool;
  //pool.SetInitCallback(init);
  pool.SetMaxQueueSize(5);

  //pool.Run(threadfunc);  // 由主线程执行.

  pool.Start(3);
  pool.Run(threadfunc);
  pool.Run(threadfunc);

  Thread t(std::bind(threadfunc2, std::ref(pool)), "thread3");
  t.Start();

  pool.Run(threadfunc);
  pool.Run(threadfunc);
  t.Join();

  return 0;
}
