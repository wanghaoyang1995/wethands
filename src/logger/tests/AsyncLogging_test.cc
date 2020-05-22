#include "src/logger/AsyncLogging.h"
#include "src/logger/Logger.h"
#include "src/thread/CurrentThread.h"

using namespace wethands;

AsyncLogging* gAsyncLog = nullptr;

void output(const char* line, size_t len) {
  gAsyncLog->Append(line, len);
}

void test() {
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 99999; ++j) {
      LOG_INFO << "log " << j;
    }
    CurrentThread::SleepUsec(600000);
  }
}

int main() {
  AsyncLogging asynclog("asynclog", 1024*2);
  gAsyncLog = &asynclog;
  Logger::setOutputFunc(output);
  asynclog.Start();
  test();
  return 0;
}
