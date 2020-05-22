#include "src/logger/AsyncLogging.h"
#include <cstdio>
#include "src/thread/CurrentThread.h"
#include "src/logger/Logger.h"
#include "src/thread/Thread.h"

using namespace wethands;

void test() {
  printf("Thread %s start to log.\n", CurrentThread::Name());
  // 至少需要9秒完成.
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      LOG_INFO << "t2 log " << i << j;
    }
    CurrentThread::SleepUsec(1000000);
  }
  printf("log append done.\n");
}

void testFixedBuffer() {
  details::FixedBuffer<details::kLargeBufferSize> buffer;
  buffer.Append("abcd\n", 5);
  assert(buffer.WrittenBytes() == 5);
  assert(buffer.AvailableBytes() == details::kLargeBufferSize -
                                    buffer.WrittenBytes());

  buffer.Reset();
  assert(buffer.WrittenBytes() == 0);
  assert(buffer.AvailableBytes() == details::kLargeBufferSize);

}

int main() {
  testFixedBuffer();
  AsyncLogging asynclog("asynclog", 1024*64);
  LOG_INFO << "log output to stdout.";
  asynclog.Start();
  Thread t1(test, "t1");
  t1.Start();
  CurrentThread::SleepUsec(1000000);
  for (int i = 0; i < 10; ++i) {
      LOG_INFO << "main thread log " << i;
  }

  t1.Join();
  asynclog.Stop();

  LOG_INFO << "log output to stdout.";
  return 0;
}
