// Copyright (c) 2020 GGGGITFKBJG
//
// Date: 2020-05-11 19:09:10
// Description:

#ifndef SRC_LOGGER_LOGGER_H_
#define SRC_LOGGER_LOGGER_H_

#include <sstream>

namespace wethands {

void DefaultOutput(const char* msg, size_t len);
void DefaultFlush();

// 日志系统前端的记录器.
class Logger {
 public:
  using OutputFunc = void (*)(const char*, size_t);
  using FlushFunc = void (*)();
  enum LogLevel {
    TRACE,           // 细粒度的记录. Release编译时也会输出.
    DEBUG,           // 用于调试的细粒度记录. 只有在Debug编译模式下会输出.
    INFO,            // 用于粗粒度的程序运行过程记录.
    WARN,            // 用于可能存在潜在错误的事件记录.
    ERROR,           // 出错但不影响程序运行的事件记录.
    FATAL,           // 出错影响程序继续进行, 记录并终止进程.
    NUM_OF_LOGLEVEL
  };

  // 构造日志头, 及错误信息(如果有的话)
  Logger(const char* file, int line, LogLevel level, int savedErrno = 0);
  // 在析构时传递给后端作实际的输出工作
  ~Logger();

  static LogLevel GetLogLevel();
  static void SetLogLevel(LogLevel level);
  static void SetOutputFunc(OutputFunc output);
  static void SetFlushFunc(FlushFunc flush);
  std::ostringstream& Stream() { return stream_; }

 private:
  std::ostringstream stream_;  // TODO(GGGGITFKBJG): 使用自定义输出流以改善效率.
  LogLevel level_;  // 当前输出所用的类型
};

}  // namespace wethands

#define LOG_TRACE \
  if (wethands::Logger::GetLogLevel() <= wethands::Logger::TRACE) \
    wethands::Logger(__FILE__, __LINE__, wethands::Logger::TRACE).Stream()

#ifdef NDEBUG
// 如果是release编译模式, 就隐藏LOG_DEBUG的输出.
#define LOG_DEBUG \
  if (false) \
    wethands::Logger(__FILE__, __LINE__, wethands::Logger::DEBUG).Stream()
#else
#define LOG_DEBUG \
  if (wethands::Logger::GetLogLevel() <= wethands::Logger::DEBUG) \
    wethands::Logger(__FILE__, __LINE__, wethands::Logger::DEBUG).Stream()
#endif

#define LOG_INFO \
  if (wethands::Logger::GetLogLevel() <= wethands::Logger::INFO) \
    wethands::Logger(__FILE__, __LINE__, wethands::Logger::INFO).Stream()

#define LOG_WARN \
  wethands::Logger(__FILE__, __LINE__, wethands::Logger::WARN).Stream()

#define LOG_ERROR \
  wethands::Logger(__FILE__, __LINE__, wethands::Logger::ERROR).Stream()

#define LOG_FATAL \
  wethands::Logger(__FILE__, __LINE__, wethands::Logger::FATAL).Stream()

// 系统调用错误. 输出错误码及描述.
#define LOG_SYSERROR \
  wethands::Logger(__FILE__, __LINE__, wethands::Logger::ERROR, errno).Stream()
// 严重系统调用错误. 输出错误码及描述并终止进程.
#define LOG_SYSFATAL \
  wethands::Logger(__FILE__, __LINE__, wethands::Logger::FATAL, errno).Stream()

#endif  // SRC_LOGGER_LOGGER_H_
