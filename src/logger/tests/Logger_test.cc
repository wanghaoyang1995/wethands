#include "src/logger/Logger.h"
#include <fcntl.h>
using namespace wethands;

int main() {
  /*
  Logger::SetLogLevel(Logger::TRACE);
  LOG_TRACE << "TRACE";
  LOG_DEBUG << "DEBUG";
  LOG_INFO << "INFO";
  LOG_ERROR << "ERROR";
  //LOG_FATAL << "FATAL";
  */

  Logger::SetLogLevel(Logger::DEBUG);
  LOG_TRACE << "TRACE";
  LOG_DEBUG << "DEBUG";
  LOG_INFO << "INFO";
  LOG_ERROR << "ERROR";
  //LOG_FATAL << "FATAL";

  int ret = ::open("nonexitent file", O_RDONLY);
  if (ret < 0)
    LOG_SYSERROR << "open failed.";

  return 0;
}
