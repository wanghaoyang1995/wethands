// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-11 19:09:10
// Description:

#ifndef SRC_LOGGER_LOGGER_H_
#define SRC_LOGGER_LOGGER_H_

#include <sstream>

namespace wethands {

class Logger {
 public:
  typedef void (*OutputFunc)(const char*, int);
  typedef void (*FlushFunc)();
  enum LogLevel {
    TRACE,           // 细粒度的记录. Release编译时也会输出.
    DEBUG,           // 用于调试的细粒度记录. 只有在Debug编译模式下会输出.
    INFO,            // 用于粗粒度的程序运行过程记录.
    WARN,            // 用于可能存在潜在错误的事件记录.
    ERROR,           // 出错但不影响程序运行的事件记录.
    FATAL,           // 出错影响程序继续进行, 记录并终止进程.
    NUM_OF_LOGLEVEL
  };

  Logger(const char* file, int line, LogLevel level, const char* func);
  ~Logger();

  static LogLevel getLogLevel();
  static void setLogLevel(LogLevel level);
  static void setOutputFunc(OutputFunc output);
  static void setFlushFunc(FlushFunc flush);
  std::ostringstream& stream() { return stream_; }

 private:
  std::ostringstream stream_;  // TODO(GGGGITFKBJG): 使用自定义输出流以改善效率.
};

// 一个全局变量, 记录当前日志级别.
extern Logger::LogLevel gLogLevel;

#define LOG_TRACE \
  if (gLogLevel <= Logger::LogLevel::TRACE) \
    Logger(__FILE__, __LINE__, Logger::LogLevel::TRACE).stream()

#ifdef NDEBUG
#define LOG_DEBUG \
  if (false) \
    Logger(__FILE__, __LINE__, Logger::LogLevel::DEBUG).stream()
#else
#define LOG_DEBUG \
  if (gLogLevel <= Logger::LogLevel::DEBUG) \
    Logger(__FILE__, __LINE__, Logger::LogLevel::DEBUG).stream()
#endif

#define LOG_INFO \
  if (gLogLevel <= Logger::LogLevel::INFO) \
    Logger(__FILE__, __LINE__, Logger::LogLevel::INFO).stream()

#define LOG_WARN \
  Logger(__FILE__, __LINE__, Logger::LogLevel::WARN).stream()

#define LOG_ERROR \
  Logger(__FILE__, __LINE__, Logger::LogLevel::ERROR).stream()

#define LOG_FATAL \
  Logger(__FILE__, __LINE__, Logger::LogLevel::FATAL).stream()

}  // namespace wethands

#endif  // SRC_LOGGER_LOGGER_H_
