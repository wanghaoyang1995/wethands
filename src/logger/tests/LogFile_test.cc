#include "src/logger/LogFile.h"
#include "src/logger/Logger.h"
#include "src/thread/CurrentThread.h"

using namespace wethands;

LogFile file("logfile", 2*1024);  // 2KB

void output(const char* line, size_t len) {
  file.Append(line, len);
}

void flush() {
  file.Flush();
}

void test() {
  for (int i = 0; i < 200; ++i) {
    LOG_INFO << "count " << i;
  }
  CurrentThread::SleepUsec(1000000);
  //file.RollFile();  // 一秒内只能生效一次
  for (int i = 0; i < 100; ++i) {
    LOG_INFO << "count " << i;
  }
}

int main() {
  Logger::setOutputFunc(output);
  Logger::setFlushFunc(flush);
  test();
  return 0;
}
