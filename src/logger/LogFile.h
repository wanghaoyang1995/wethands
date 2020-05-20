// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-20 17:17:33
// Description:

#ifndef SRC_LOGGER_LOGFILE_H_
#define SRC_LOGGER_LOGFILE_H_

#include <memory>
#include <string>
#include "src/utils/FileUtil.h"
#include "src/thread/Mutex.h"
#include "src/utils/Timestamp.h"
#include "src/utils/Uncopyable.h"

namespace wethands {

// 日志文件类.
// 文件会按指定大小及一定时间间隔分批次保存.
// 可以指定是否要求线程安全版本.
class LogFile : public Uncopyable {
 public:
  LogFile(const std::string& basename,
          off_t rollsize,
          bool threadSafe = true,
          int flushInterval = 3,
          int checkEveryNAppend = 1024);
  ~LogFile();

  void Append(const char* line, size_t len);
  void Flush();
  // 创建新一轮的文件并修改相关信息.
  void RollFile();

 private:
  // 传入 Timestamp, 返回格式化的文件名.
  std::string FormattedName(Timestamp time);

  // 必须是文件名而不是路径名.
  const std::string basename_;
  // 检查时已写字节数若超过 rollsize_, 就转向下一轮文件.
  const off_t rollsize_;
  // 检查时距上次 flush 秒数若超过 flushInterval_, 就冲洗缓冲区.
  const int flushInterval_;
  // 每调用 Append() checkEveryNAppend_ 次, 就进行一次检查.
  // 为了提高效率, 避免每次调用 Append() 时都进行检查.
  const int checkEveryNAppend_;
  // Append() 调用计数.
  int appendCount_;

  Timestamp lastRoll_;  // 上一次调用 RollFile() 的时刻.
  Timestamp lastFlush_;  // 上一次调用 Flush() 的时刻.
  Timestamp currentPeriod_;  // 当前周期的起始时刻.

  std::unique_ptr<FileUtil::AppendFile> file_;
  std::unique_ptr<MutexLock> lock_;

  static const int kSecondsPerPeriod_ = 60 * 60 * 24;  // 每周期(24h)的秒数.
};

}

#endif  // SRC_LOGGER_LOGFILE_H_