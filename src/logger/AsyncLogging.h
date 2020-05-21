// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-21 02:28:49
// Description:

#ifndef SRC_LOGGER_ASYNCLOGGING_H_
#define SRC_LOGGER_ASYNCLOGGING_H_

#include <unistd.h>
#include <string>
#include "src/utils/Uncopyable.h"

namespace wethands {
// 异步的日志系统后端组件. 该类对象应该是全局唯一的.
class AsyncLogging : Uncopyable {
 public:
  AsyncLogging(const std::string& basename,
               off_t rollsize,
               int flushInterval = 3);
  ~AsyncLogging();
  // 将日志放入缓冲区队列, 等待后台线程写入文件.
  // 该函数由日志系统前端调用.
  void Append(const char* line, size_t len);
  // 启动异步日志系统.
  // 会创建一个后台线程做文件写入的工作.
  void start();
  // 关闭异步日志系统.
  void stop();
 private:


  // 后台线程反复地从缓冲队列中取出并写入文件.
  void threadFunc();

};

}  // namespace wethands

#endif  // SRC_LOGGER_ASYNCLOGGING_H_