// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-20 17:17:39
// Description:

#include "src/logger/LogFile.h"
#include <unistd.h>
#include <cassert>

using wethands::LogFile;

LogFile::LogFile(const std::string& basename,
                 size_t rollsize,
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
  RollFileUnlocked();
}

void LogFile::Append(const char* line, size_t len) {
  if (len == 0) return;
  if (lock_) {
    MutexLockGuard guard(*lock_);
    AppendUnlocked(line, len);
  } else {
    AppendUnlocked(line, len);
  }
}

void LogFile::RollFile() {
  if (lock_) {
    MutexLockGuard guard(*lock_);
    RollFileUnlocked();
  } else {
    RollFileUnlocked();
  }
}

void LogFile::Flush() {
  if (lock_) {
    MutexLockGuard guard(*lock_);
    file_->Flush();
  } else {
    file_->Flush();
  }
}

void LogFile::AppendUnlocked(const char* line, size_t len) {
  if (file_->WrittenBytes() + len > rollsize_) {  // 文件即将到达指定大小.
    RollFileUnlocked();  // 创建新文件.
    appendCount_ = 0;
  } else {  // 文件未达到指定大小.
    if (appendCount_ >= checkEveryNAppend_) {  // Append达到次数就会触发检查.
      appendCount_ = 0;
      Timestamp now = Timestamp::Now();
      Timestamp startOfToday = now.RoundByDay();

      if (currentPeriod_ < startOfToday) {  // 如果跨越了周期, 就RollFile().
        RollFileUnlocked();
        currentPeriod_ = startOfToday;
      } else if (SecondsDifference(now, lastFlush_) >=
                 static_cast<double>(flushInterval_)) {  // 超过了刷新间隔, 刷新.
        file_->Flush();
        lastFlush_ = now;
      }
    }
  }
  file_->Append(line, len);
  ++appendCount_;
}

void LogFile::RollFileUnlocked() {
  Timestamp now = Timestamp::Now();
  assert(now > lastRoll_);
  // 为了避免文件同名. 1秒内只能创建一个文件.
  if (SecondsDifference(now, lastRoll_) < 1) return;
  lastRoll_ = now;
  lastFlush_ = now;
  currentPeriod_ = now.RoundByDay();  // TODO(GGGGITFKBJG): 换用更高效的方式.
  file_.reset(new FileUtil::AppendFile(FormattedName(now)));
}

std::string LogFile::FormattedName(Timestamp time) const {
  // basename_.本地时间(精确到秒).进程号.log
  return basename_ + "." +
         time.ToFormattedString() + "." +
         std::to_string(::getpid()) + ".log";
}
