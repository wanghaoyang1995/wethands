#include "src/logger/AsyncLogging.h"
#include "src/logger/Logger.h"

using namespace wethands;

void test() {
  for (int i = 0; i < 99999; ++i) {
    LOG_INFO << "log " << i;
  }
}

int main() {
  AsyncLogging asynclogging("asynclog", 1024*2);

  asynclogging.Start();
  test();
  return 0;
}
