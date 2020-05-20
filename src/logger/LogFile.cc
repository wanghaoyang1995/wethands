// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-20 17:17:39
// Description:

#include "src/logger/LogFile.h"
#include <unistd.h>
#include <cassert>

using wethands::LogFile;

LogFile::LogFile(const std::string& basename,
                 off_t rollsize,
                 bool threadSafe,
                 int flushInterval,
                 int checkEveryNAppend)
    : basename_(basename),
      rollsize_(rollsize),
      flushInterval_(flushInterval),
      checkEveryNAppend_(checkEveryNAppend),
      appendCount_(0),
      lastRoll_(0),
      lastFlush_(0),
      currentPeriod_(0),
      lock_(threadSafe ? new MutexLock : nullptr) {
  assert(basename_.find('/') == std::string::npos);
  RollFile();
}

LogFile::~LogFile() {

}

void LogFile::Append(const char* line, size_t len) {

}

void LogFile::Flush() {

}

void LogFile::RollFile() {
  Timestamp now = Timestamp::Now();
  lastRoll_ = now;
  lastFlush_ = now;



}

std::string LogFile::FormattedName(Timestamp time) {
  // basename_.本地时间.进程号.log
  return basename_ + "." +
         time.ToFormattedString() + "." +
         std::to_string(::getpid()) + ".log";
}
