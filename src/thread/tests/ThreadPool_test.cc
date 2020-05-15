#include "src/thread/ThreadPool.h"
#include <cstdio>
#include "src/thread/CurrentThread.h"

using namespace wethands;

void init() {
  printf("init(): thread id = %d, threadname = %s\n",
         CurrentThread::Tid(), CurrentThread::Name());
}

void threadfunc() {
  CurrentThread::SleepUsec(4000000);
  printf("threadfunc(): thread id = %d, threadname = %s\n",
         CurrentThread::Tid(), CurrentThread::Name());
}

void threadfunc2(ThreadPool& pool) {
  CurrentThread::SleepUsec(1000000);
  printf("thread %d stop the pool.\n", CurrentThread::Tid());
  pool.Stop();
}

int main() {
  ThreadPool pool;
  pool.SetInitCallback(init);
  pool.SetMaxQueueSize(2);  // 队列任务上限为1

  //pool.Run(threadfunc);  // 由主线程执行.

  pool.Start(2);  // 开启两个线程.
  //放两个耗时的任务进去, 线程1和线程2一人取出一个执行. 队列空了
  pool.Run(threadfunc);
  pool.Run(threadfunc);

  // 再用两个耗时任务占满队列
  pool.Run(threadfunc);
  pool.Run(threadfunc);

  Thread t(std::bind(threadfunc2, std::ref(pool)), "thread3");
  t.Start();// 1秒后由线程3关闭线程池

  // 因为队列已满, 这里应该会阻塞到线程1或线程2完成任务. 线程池关闭后它会永远阻塞.
  pool.Run(threadfunc);

  //CurrentThread::SleepUsec(10000000);
  //pool.Stop();
  return 0;
}
